#include "esphome/core/log.h"
#include "fswm100.h"
#include "flow_sensor.h"

namespace esphome {
namespace fswm100 {

FlowSensor::FlowSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void FlowSensor::setup(float effective_noise_floor, ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
                       ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "FlowSensor setup start.");
  this->effective_noise_floor_ = effective_noise_floor;
  // ADS1115
  this->multiplexer_ = multiplexer;
  this->gain_ = gain;
  this->sample_rate_ = sample_rate;
  this->resolution_ = resolution;
  // initial state publish
  this->publish_state(this->last_publish_state_);
  ESP_LOGCONFIG(TAG, "FlowSensor setup complete.");
}

void FlowSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "FlowSensor:");
  ESP_LOGCONFIG(TAG, "  effective noise floor:", this->effective_noise_floor_);
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

  // TODO: detect deltas

  ESP_LOGVV(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
            static_cast<int>(this->multiplexer_), voltage);

  return voltage;
}

void FlowSensor::loop() {
  // has the state changed enough to publish?
  float new_pressure_sensor_state = this->get_state();
  if (abs(this->last_publish_state_ - new_pressure_sensor_state) > this->effective_noise_floor_) {
    this->last_publish_state_ = new_pressure_sensor_state;
    this->publish_state(new_pressure_sensor_state);
  }
}

}  // namespace fswm100
}  // namespace esphome
