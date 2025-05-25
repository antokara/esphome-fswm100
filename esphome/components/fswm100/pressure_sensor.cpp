#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor.h"

namespace esphome {
namespace fswm100 {

PressureSensor::PressureSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensor::setup(float min_voltage, float max_voltage, float min_pressure, float max_pressure,
                           float publish_delta, float publish_frequency, float publish_delta_test,
                           float publish_frequency_test, ads1115::ADS1115Multiplexer multiplexer,
                           ads1115::ADS1115Gain gain, ads1115::ADS1115Samplerate sample_rate,
                           ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "PressureSensor setup start.");
  min_voltage_ = min_voltage;
  max_voltage_ = max_voltage;
  min_pressure_ = min_pressure;
  max_pressure_ = max_pressure;
  // publish
  publish_delta_ = publish_delta;
  publish_frequency_ = publish_frequency;
  publish_delta_test_ = publish_delta_test;
  publish_frequency_test_ = publish_frequency_test;
  // calculate
  voltage_factor_ = (max_pressure - min_pressure) / (max_voltage - min_voltage);
  // ADS1115
  multiplexer_ = multiplexer;
  gain_ = gain;
  sample_rate_ = sample_rate;
  resolution_ = resolution;
  // initial state publish
  this->publish_state(this->last_publish_state_);
  ESP_LOGCONFIG(TAG, "PressureSensor setup complete.");
}

void PressureSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensor:");
  ESP_LOGCONFIG(TAG, "  min voltage:", this->min_voltage_);
  ESP_LOGCONFIG(TAG, "  max voltage:", this->max_voltage_);
  ESP_LOGCONFIG(TAG, "  min pressure:", this->min_pressure_);
  ESP_LOGCONFIG(TAG, "  max pressure:", this->max_pressure_);
  // publish
  ESP_LOGCONFIG(TAG, "  publish delta:", this->publish_delta_);
  ESP_LOGCONFIG(TAG, "  publish frequency:", this->publish_frequency_);
  ESP_LOGCONFIG(TAG, "  publish delta test:", this->publish_delta_test_);
  ESP_LOGCONFIG(TAG, "  publish frequency test:", this->publish_frequency_test_);
  // calculated
  ESP_LOGCONFIG(TAG, "  voltage factor:", this->voltage_factor_);
  // ADS1115
  ESP_LOGCONFIG(TAG, "  multiplexer:", this->multiplexer_);
  ESP_LOGCONFIG(TAG, "  gain:", this->gain_);
  ESP_LOGCONFIG(TAG, "  sample rate:", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  resolution:", this->resolution_);
}

float PressureSensor::get_state() {
  /**
   * the voltage has already been adjusted for gain and resolution.
   * meaning, we get the proper min/max voltage for the range it can provide.
   *
   * @example 0-4.96 (V) for the IC L7805
   */
  float voltage = abs(this->fswm100_->get_ads1115()->request_measurement(this->multiplexer_, this->gain_,
                                                                         this->resolution_, this->sample_rate_));

  if (std::isnan(voltage)) {
    ESP_LOGW(TAG, "Failed to read from ADS1115 channel for '%s'. Result was NaN.", this->get_name().c_str());
    return -1;  // when it fails
  }

  ESP_LOGVV(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
            static_cast<int>(this->multiplexer_), voltage);

  float pressure = this->voltage_to_pressure(voltage) * this->fswm100_->get_pressure_sensor_calibration_multiplier();
  ESP_LOGVV(TAG, "'%s': Converted to %.4f pressure", this->get_name().c_str(), pressure);

  if (pressure < this->min_pressure_) {
    return this->min_pressure_;
  }
  if (pressure > this->max_pressure_) {
    return this->max_pressure_;
  }
  return pressure;
}

float PressureSensor::voltage_to_pressure(float voltage) {
  return (voltage - this->min_voltage_) * this->voltage_factor_ + this->min_pressure_;
}

void PressureSensor::loop() {
  uint32_t time_since_publish = millis() - this->last_publish_time_;
  // has enough time passed to get the state?
  if ((this->fswm100_->get_pressure_sensor_test_flag() && time_since_publish > this->publish_frequency_test_) ||
      (!this->fswm100_->get_pressure_sensor_test_flag() && time_since_publish > this->publish_frequency_)) {
    // has the state changed enough to publish?
    float new_pressure_sensor_state = this->get_state();
    if (abs(this->last_publish_state_ - new_pressure_sensor_state) > this->publish_delta_) {
      this->last_publish_time_ = millis();
      this->last_publish_state_ = new_pressure_sensor_state;
      this->publish_state(new_pressure_sensor_state);
    }
  }
}

}  // namespace fswm100
}  // namespace esphome
