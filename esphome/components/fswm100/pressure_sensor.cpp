#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor.h"

namespace esphome {
namespace fswm100 {

double time_constant = 7.0;
int window_size = 3;

PressureSensor::PressureSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensor::setup(float effective_noise_floor, float min_voltage, float max_voltage, float min_pressure,
                           float max_pressure, ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
                           ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "PressureSensor setup start.");
  this->effective_noise_floor_ = effective_noise_floor;
  this->min_voltage_ = min_voltage;
  this->max_voltage_ = max_voltage;
  this->min_pressure_ = min_pressure;
  this->max_pressure_ = max_pressure;
  // calculate
  this->voltage_factor_ = (max_pressure - min_pressure) / (max_voltage - min_voltage);
  // ADS1115
  this->multiplexer_ = multiplexer;
  this->gain_ = gain;
  this->sample_rate_ = sample_rate;
  this->resolution_ = resolution;
  // initial state publish
  this->publish_state(0);

  ESP_LOGCONFIG(TAG, "PressureSensor setup complete.");
}

void PressureSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensor:");
  ESP_LOGCONFIG(TAG, "  effective noise floor:", this->effective_noise_floor_);
  ESP_LOGCONFIG(TAG, "  min voltage:", this->min_voltage_);
  ESP_LOGCONFIG(TAG, "  max voltage:", this->max_voltage_);
  ESP_LOGCONFIG(TAG, "  min pressure:", this->min_pressure_);
  ESP_LOGCONFIG(TAG, "  max pressure:", this->max_pressure_);
  // calculated
  ESP_LOGCONFIG(TAG, "  voltage factor:", this->voltage_factor_);
  // ADS1115
  ESP_LOGCONFIG(TAG, "  multiplexer:", this->multiplexer_);
  ESP_LOGCONFIG(TAG, "  gain:", this->gain_);
  ESP_LOGCONFIG(TAG, "  sample rate:", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  resolution:", this->resolution_);
}

float PressureSensor::voltage_to_pressure(float voltage) {
  return (voltage - this->min_voltage_) * this->voltage_factor_ + this->min_pressure_;
}

void PressureSensor::loop() {
  /**
   * the voltage has already been adjusted for gain and resolution.
   * meaning, we get the proper min/max voltage for the range it can provide.
   *
   * @example 0-4.96 (V) for the IC L7805
   */
  float new_sensor_state = abs(this->fswm100_->get_ads1115()->request_measurement(
      this->multiplexer_, this->gain_, this->resolution_, this->sample_rate_));

  if (std::isnan(new_sensor_state)) {
    ESP_LOGW(TAG, "Failed to read from ADS1115 channel for '%s'. Result was NaN.", this->get_name().c_str());
    return;
  }

  ESP_LOGVV(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
            static_cast<int>(this->multiplexer_), new_sensor_state);

  // has the state changed enough to publish?
  if (abs(this->last_sensor_state_ - new_sensor_state) > this->effective_noise_floor_) {
    float pressure =
        this->voltage_to_pressure(new_sensor_state) * this->fswm100_->get_pressure_sensor_calibration_multiplier();
    ESP_LOGV(TAG, "'%s': Voltage Delta %.4f", this->get_name().c_str(),
             abs(this->last_sensor_state_ - new_sensor_state));
    ESP_LOGV(TAG, "'%s': Converted to %.4f pressure", this->get_name().c_str(), pressure);

    if (pressure < this->min_pressure_) {
      pressure = this->min_pressure_;
    }
    if (pressure > this->max_pressure_) {
      pressure = this->max_pressure_;
    }
    this->publish_state(pressure);

    if (this->prev_pressure_sensor_test_flag_ != this->fswm100_->get_pressure_sensor_test_flag()) {
      this->prev_pressure_sensor_test_flag_ = this->fswm100_->get_pressure_sensor_test_flag();
      if (this->prev_pressure_sensor_test_flag_) {
        // started
        this->pressure_sensor_test_start_pressure_ = pressure;
        this->prev_filtered_value_ = 0;
        this->fswm100_->publish_pressure_test_sensor(0);
        // TODO: make the time constant and window size configurable
        this->filter_ = new LowPassFilter(time_constant, window_size);
      } else {
        // stopped. reset
        this->fswm100_->publish_pressure_test_sensor(0);
      }
    } else if (this->prev_pressure_sensor_test_flag_) {
      // test in progress
      float filtered_val_opt = this->filter_->add_sample(pressure - this->pressure_sensor_test_start_pressure_);
      // TODO: make this configurable
      if (filtered_val_opt && abs(prev_filtered_value_ - filtered_val_opt) > 0.1) {
        prev_filtered_value_ = filtered_val_opt;
        this->fswm100_->publish_pressure_test_sensor(filtered_val_opt);
      }
    }
  }
}

}  // namespace fswm100
}  // namespace esphome
