#include "esphome/core/log.h"
#include "fswm100.h"
#include "flow_ir_sensor.h"

namespace esphome {
namespace fswm100 {

FlowIrSensor::FlowIrSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void FlowIrSensor::setup(float effective_noise_floor, float min_voltage, float max_voltage,
                         uint32_t debug_publish_interval_ms, ads1115::ADS1115Multiplexer multiplexer,
                         ads1115::ADS1115Gain gain, ads1115::ADS1115Samplerate sample_rate,
                         ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "FlowIrSensor setup start.");
  this->effective_noise_floor_ = effective_noise_floor;
  this->min_voltage_ = min_voltage;
  this->max_voltage_ = max_voltage;
  this->debug_publish_interval_ms_ = debug_publish_interval_ms;
  // ADS1115
  this->multiplexer_ = multiplexer;
  this->gain_ = gain;
  this->sample_rate_ = sample_rate;
  this->resolution_ = resolution;
  // initialize values...
  this->publish(false);
  ESP_LOGCONFIG(TAG, "FlowIrSensor setup complete.");
}

void FlowIrSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "FlowIrSensor:");
  ESP_LOGCONFIG(TAG, "  effective noise floor:", this->effective_noise_floor_);
  ESP_LOGCONFIG(TAG, "  min voltage:", this->min_voltage_);
  ESP_LOGCONFIG(TAG, "  max voltage:", this->max_voltage_);
  ESP_LOGCONFIG(TAG, "  debug publish interval ms:", this->debug_publish_interval_ms_);
  ESP_LOGCONFIG(TAG, "  ADS1115 configuration:");
  ESP_LOGCONFIG(TAG, "  multiplexer:", this->multiplexer_);
  ESP_LOGCONFIG(TAG, "  gain:", this->gain_);
  ESP_LOGCONFIG(TAG, "  sample rate:", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  resolution:", this->resolution_);
}

bool FlowIrSensor::has_fault() { return this->has_fault_; }

void FlowIrSensor::publish(bool ir_active) { this->publish_state(ir_active); }

bool FlowIrSensor::get_raw_state() { return this->raw_state_; }

float FlowIrSensor::get_state() {
  float new_sensor_state = abs(this->fswm100_->get_ads1115()->request_measurement(
      this->multiplexer_, this->gain_, this->resolution_, this->sample_rate_));

  // diagnostics: check for invalid reading
  if (!this->has_fault_ && std::isnan(new_sensor_state)) {
    ESP_LOGE(TAG, "Failed to read from ADS1115 channel for '%s'. Result was NaN.", this->get_name().c_str());
    this->fswm100_->add_fault("Failed to read from ADS1115 channel for flow sensor. Result was NaN.");
    this->has_fault_ = true;
    return 0;  // when it fails
  }

  ESP_LOGVV(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
            static_cast<int>(this->multiplexer_), new_sensor_state);

  // diagnostics: check if the voltage is within the expected range
  if (!this->has_fault_ && new_sensor_state < this->min_voltage_ || new_sensor_state > this->max_voltage_) {
    ESP_LOGW(TAG, "'%s': voltage out of range: %.4f V", this->get_name().c_str(), new_sensor_state);
    this->fswm100_->add_fault("Flow IR sensor voltage out of range: " + std::to_string(new_sensor_state) + " V");
    this->has_fault_ = true;
  }

  return new_sensor_state;
}

void FlowIrSensor::loop() {
  float new_sensor_state = this->get_state();

  // calculate the state delta
  float state_delta = abs(this->last_sensor_state_ - new_sensor_state);

  // calculate the state active counts
  if (state_delta > this->effective_noise_floor_ && !this->debug_state_active_counts_previous_) {
    // increase the counts
    this->debug_state_active_counts_++;
    // set the previous state to true
    this->debug_state_active_counts_previous_ = true;
  } else if (state_delta <= this->effective_noise_floor_ && this->debug_state_active_counts_previous_) {
    // reset the previous state to false
    this->debug_state_active_counts_previous_ = false;
  }
  // keep the max state delta for debug purposes
  if (state_delta > this->debug_state_delta_max_) {
    this->debug_state_delta_max_ = state_delta;
  }
  // keep the min/max state for debug purposes
  if (this->debug_state_min_ == 0.0f || new_sensor_state < this->debug_state_min_) {
    this->debug_state_min_ = new_sensor_state;
  }
  if (this->debug_state_max_ == 0.0f || new_sensor_state > this->debug_state_max_) {
    this->debug_state_max_ = new_sensor_state;
  }

  // determine if IR movement was detected (above the noise floor)
  this->raw_state_ = state_delta > this->effective_noise_floor_;
  if (this->raw_state_) {
    // has the state changed enough to publish?
    if (this->last_sensor_state_ == 0) {
      // first time we read the sensor, or it was 0 before
      ESP_LOGD(TAG, "Flow IR: first reading voltage %.4f", new_sensor_state);
      this->last_sensor_state_ = new_sensor_state;
      return;  // no need to publish, as we just started
    }
    // active due to IR movement
    ESP_LOGV(TAG, "Flow IR: active due to IR voltage %.4f delta", state_delta);
    this->publish(true);
    this->last_sensor_state_ = new_sensor_state;
  } else if (this->raw_state_ > 0) {
    // just switched to inactive
    ESP_LOGD(TAG, "Flow IR: inactive");
    this->publish(false);
  }

  // publish debug state meta info every FLOW_SENSOR_DEBUG_PUBLISH_INTERVAL_MS
  if (millis() - this->last_debug_state_time_ > this->debug_publish_interval_ms_) {
    if (this->state == 0) {
      ESP_LOGD(TAG, "Flow IR: inactive due to IR voltage %.4f state_delta_max, counts: %d, min: %.4f, max: %.4f",
               this->debug_state_delta_max_, this->debug_state_active_counts_, this->debug_state_min_,
               this->debug_state_max_);
    } else {
      ESP_LOGD(TAG, "Flow IR: active due to IR voltage %.4f state_delta_max, counts: %d, min: %.4f, max: %.4f",
               this->debug_state_delta_max_, this->debug_state_active_counts_, this->debug_state_min_,
               this->debug_state_max_);
    }
    // reset
    this->last_debug_state_time_ = millis();
    this->debug_state_delta_max_ = 0.0f;
    this->debug_state_active_counts_ = 0;
    this->debug_state_min_ = 0.0f;
    this->debug_state_max_ = 0.0f;
  }
}

}  // namespace fswm100
}  // namespace esphome
