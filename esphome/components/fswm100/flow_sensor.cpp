#include "esphome/core/log.h"
#include "fswm100.h"
#include "flow_sensor.h"

namespace esphome {
namespace fswm100 {

FlowSensor::FlowSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void FlowSensor::setup(const std::function<std::vector<sensor::Filter *>()> &filters_factory,
                       float effective_noise_floor, float min_volume, float max_volume, float rate_time,
                       ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
                       ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "FlowSensor setup start.");
  this->filters_factory_ = filters_factory;
  this->effective_noise_floor_ = effective_noise_floor;
  this->min_volume_ = min_volume;
  this->max_volume_ = max_volume;
  this->rate_time_ = rate_time;
  // ADS1115
  this->multiplexer_ = multiplexer;
  this->gain_ = gain;
  this->sample_rate_ = sample_rate;
  this->resolution_ = resolution;
  // initialize values...
  this->publish(0, true);
  ESP_LOGCONFIG(TAG, "FlowSensor setup complete.");
}

void FlowSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "FlowSensor:");
  ESP_LOGCONFIG(TAG, "  effective noise floor:", this->effective_noise_floor_);
  ESP_LOGCONFIG(TAG, "  min volume:", this->min_volume_);
  ESP_LOGCONFIG(TAG, "  max volume:", this->max_volume_);
  ESP_LOGCONFIG(TAG, "  rate time:", this->rate_time_);
  ESP_LOGCONFIG(TAG, "  multiplexer:", this->multiplexer_);
  ESP_LOGCONFIG(TAG, "  gain:", this->gain_);
  ESP_LOGCONFIG(TAG, "  sample rate:", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  resolution:", this->resolution_);
}

bool FlowSensor::has_fault() { return this->has_fault_; }

void FlowSensor::active() {
  this->last_active_time_ = millis();
  this->publish(this->calculate_active_flow());
}

float FlowSensor::calculate_active_flow() {
  // start with the minimum flow volume
  float flow = this->min_volume_;

  // don't attempt to calculate flow if we don't have a pulse sensor active time
  if (this->oldest_pulse_sensor_active_time_ > 0) {
    const uint32_t time_between_pulses =
        abs(long(this->newest_pulse_sensor_active_time_ - this->oldest_pulse_sensor_active_time_));
    const uint32_t time_since_newest_pulse = abs(long(millis() - this->newest_pulse_sensor_active_time_));
    const float calculated_flow = (this->rate_time_ * 1000) / (millis() - this->oldest_pulse_sensor_active_time_) *
                                  this->fswm100_->get_pulse_rate_volume();
    if (time_since_newest_pulse > long(time_between_pulses * FLOW_RATE_TIME_BETWEEN_PULSES_MULTIPLIER) ||
        (calculated_flow > this->state)) {
      // if the the current flow is greater than the last published one or
      // if time since the last pulse is greater than the time between pulses,
      // we use the newly calculated flow rate.
      flow = calculated_flow;
    } else if (this->state > 0) {
      // when not enough time has passed to calculate the new flow rate but there is an active flow,
      // use the last published flow rate
      flow = this->state;
    }
  }

  // ensure the flow is not below the minimum volume
  if (flow < this->min_volume_) {
    flow = this->min_volume_;
  }

  return flow;
}

void FlowSensor::publish(float flow, bool immediate) {
  if (immediate || this->state == 0 && flow > 0 || this->state > 0 && flow == 0) {
    this->reset_filters();
  }
  this->publish_state(flow);
}

float FlowSensor::get_state() {
  /**
   *  - Black Surface/Low reflection leads to
   *    decreased phototransistor conductivity,
   *    causing the output voltage to be higher, closer to Vcc.
   *
   *  - White Surface/High reflection leads to
   *    increased phototransistor conductivity,
   *    causing the output voltage to be lower, closer to GND.
   *
   * The precision we can achieve with our circuit
   * (power supply, ADS1115 and the TCR5000) is about 0.05V.
   * This means that any voltage flactuation below 0.05V
   * should be ignored...
   */
  float new_sensor_state = abs(this->fswm100_->get_ads1115()->request_measurement(
      this->multiplexer_, this->gain_, this->resolution_, this->sample_rate_));

  if (std::isnan(new_sensor_state)) {
    ESP_LOGD(TAG, "Failed to read from ADS1115 channel for '%s'. Result was NaN.", this->get_name().c_str());
    return 0;  // when it fails
  }

  ESP_LOGVV(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
            static_cast<int>(this->multiplexer_), new_sensor_state);

  return new_sensor_state;
}

void FlowSensor::loop() {
  float new_sensor_state = this->get_state();

  // has the state changed enough to publish?
  float state_delta = abs(this->last_sensor_state_ - new_sensor_state);
  bool pulse_sensor_state = this->fswm100_->get_pulse_sensor();
  if (pulse_sensor_state != this->last_pulse_sensor_state_ && pulse_sensor_state) {
    // active due to new pulse
    ESP_LOGD(TAG, "Flow: active due to new pulse");
    this->active();
    this->last_pulse_sensor_state_ = pulse_sensor_state;
    this->oldest_pulse_sensor_active_time_ = this->newest_pulse_sensor_active_time_;
    this->newest_pulse_sensor_active_time_ = millis();

  } else if (state_delta > this->effective_noise_floor_) {
    if (this->last_sensor_state_ == 0) {
      // first time we read the sensor, or it was 0 before
      ESP_LOGD(TAG, "Flow: first reading voltage %.4f", new_sensor_state);
      this->last_sensor_state_ = new_sensor_state;
      return;  // no need to publish, as we just started
    }
    // active due to IR movement
    ESP_LOGV(TAG, "Flow: active due to IR voltage %.4f delta", state_delta);
    this->active();
    this->last_sensor_state_ = new_sensor_state;
  } else if (millis() - this->last_active_time_ > this->fswm100_->get_flow_sensor_min_duration()) {
    // inactive. no pulse or IR and timed out
    if (this->state > 0) {
      // just switched to inactive
      ESP_LOGD(TAG, "Flow: switched to inactive");
      this->publish(0);
    }

  } else if (this->state > 0) {
    // active but not yet timed out
    this->publish(this->calculate_active_flow());
  }

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
  // publish debug state every 30 seconds
  if (millis() - this->last_debug_state_time_ > 30000) {
    if (this->state == 0) {
      ESP_LOGD(TAG, "Flow: inactive due to IR voltage %.4f state_delta_max, counts: %d", this->debug_state_delta_max_,
               this->debug_state_active_counts_);
    } else {
      ESP_LOGD(TAG, "Flow: active due to IR voltage %.4f state_delta_max, counts: %d", this->debug_state_delta_max_,
               this->debug_state_active_counts_);
    }
    // reset
    this->last_debug_state_time_ = millis();
    this->debug_state_delta_max_ = 0.0f;
    this->debug_state_active_counts_ = 0;
  }

  // update if it just switched (to false)
  if (!pulse_sensor_state && pulse_sensor_state != this->last_pulse_sensor_state_) {
    this->last_pulse_sensor_state_ = pulse_sensor_state;
  }
}

void FlowSensor::reset_filters() {
  if (this->filters_factory_) {
    ESP_LOGD(TAG, "FlowSensor Resetting filters by creating new filter instances...");
    this->set_filters(this->filters_factory_());
  } else {
    ESP_LOGW(TAG, "FlowSensor No filter factory set, cannot reset filters.");
  }
}

}  // namespace fswm100
}  // namespace esphome
