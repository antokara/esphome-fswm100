#include "esphome/core/log.h"
#include "fswm100.h"
#include "flow_sensor.h"

namespace esphome {
namespace fswm100 {

FlowSensor::FlowSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void FlowSensor::setup(float effective_noise_floor, float min_volume, float rate_time, uint32_t publish_frequency,
                       ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
                       ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "FlowSensor setup start.");
  this->effective_noise_floor_ = effective_noise_floor;
  this->min_volume_ = min_volume;
  this->rate_time_ = rate_time;
  this->publish_frequency_ = publish_frequency;
  // ADS1115
  this->multiplexer_ = multiplexer;
  this->gain_ = gain;
  this->sample_rate_ = sample_rate;
  this->resolution_ = resolution;
  // initial state publish
  this->publish_state(0);
  // prevent a false positive initial flow...
  this->last_sensor_state_ = this->get_state();
  ESP_LOGCONFIG(TAG, "FlowSensor setup complete.");
}

void FlowSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "FlowSensor:");
  ESP_LOGCONFIG(TAG, "  effective noise floor:", this->effective_noise_floor_);
  ESP_LOGCONFIG(TAG, "  min volume:", this->min_volume_);
  ESP_LOGCONFIG(TAG, "  rate time:", this->rate_time_);
  ESP_LOGCONFIG(TAG, "  publish frequency:", this->publish_frequency_);
  ESP_LOGCONFIG(TAG, "  multiplexer:", this->multiplexer_);
  ESP_LOGCONFIG(TAG, "  gain:", this->gain_);
  ESP_LOGCONFIG(TAG, "  sample rate:", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  resolution:", this->resolution_);
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
  float voltage = abs(this->fswm100_->get_ads1115()->request_measurement(this->multiplexer_, this->gain_,
                                                                         this->resolution_, this->sample_rate_));

  if (std::isnan(voltage)) {
    ESP_LOGVV(TAG, "Failed to read from ADS1115 channel for '%s'. Result was NaN.", this->get_name().c_str());
    return -1;  // when it fails
  }

  ESP_LOGVV(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
            static_cast<int>(this->multiplexer_), voltage);

  return voltage;
}

void FlowSensor::active() {
  this->last_active_time_ = millis();
  if (this->state == 0) {
    // publish immediately, if this is a new active flow
    this->publish_state(this->min_volume_);
  } else {
    float flow = this->calculate_active_flow();
    if (flow > this->state) {
      // publish immediately if the flow is higher than the last state
      // this happens when the pulse triggers...
      this->publish_state(flow);
    } else {
      // otherwise, just try to publish the flow rate
      // as it most likely starts to go down
      this->try_publish(flow);
    }
  }
}

float FlowSensor::calculate_active_flow() {
  // start with the minimum flow volume
  float flow = this->min_volume_;
  // don't attempt to calculate flow if we don't have a pulse sensor active time
  if (this->oldest_pulse_sensor_active_time_ > 0) {
    flow = (this->rate_time_ * 1000) / (millis() - this->oldest_pulse_sensor_active_time_) *
           this->fswm100_->get_pulse_rate_volume();
  }
  return flow;
}

void FlowSensor::try_publish(float flow) {
  if (abs(long(millis() - this->last_publish_time_ > this->publish_frequency_))) {
    this->last_publish_time_ = millis();
    this->publish_state(flow);
  }
}

void FlowSensor::loop() {
  // has the state changed enough to publish?
  float new_sensor_state = this->get_state();
  bool pulse_sensor_state = this->fswm100_->get_pulse_sensor();
  if (pulse_sensor_state != this->last_pulse_sensor_state_ && pulse_sensor_state) {
    // active due to new pulse
    this->active();
    this->last_pulse_sensor_state_ = pulse_sensor_state;
    this->oldest_pulse_sensor_active_time_ = this->newest_pulse_sensor_active_time_;
    this->newest_pulse_sensor_active_time_ = millis();
    ESP_LOGD(TAG, "Flow: active due to new pulse");

  } else if (abs(this->last_sensor_state_ - new_sensor_state) > this->effective_noise_floor_) {
    // active due to IR movement
    ESP_LOGVV(TAG, "Flow: active due to IR %.4f delta", abs(this->last_sensor_state_ - new_sensor_state));
    this->active();
    this->last_sensor_state_ = new_sensor_state;

  } else if (millis() - this->last_active_time_ > this->fswm100_->get_flow_sensor_min_duration()) {
    // inactive. no pulse or IR and timed out
    if (this->state > 0) {
      ESP_LOGD(TAG, "Flow: inactive");
      this->publish_state(0);
    } else {
      // keep trying to send, in case the event gets missed, to avoid false positive active flow
      this->try_publish(0);
    }

  } else if (this->state > 0) {
    // active but not yet timed out
    this->try_publish(this->calculate_active_flow());
  }

  // update if it just switched (to false)
  if (!pulse_sensor_state && pulse_sensor_state != this->last_pulse_sensor_state_) {
    this->last_pulse_sensor_state_ = pulse_sensor_state;
  }
}

}  // namespace fswm100
}  // namespace esphome
