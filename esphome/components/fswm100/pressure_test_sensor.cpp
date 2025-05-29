#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_test_sensor.h"

namespace esphome {
namespace fswm100 {

PressureTestSensor::PressureTestSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureTestSensor::setup(float publish_delta, float time_constant, int window_size) {
  ESP_LOGCONFIG(TAG, "PressureTestSensor setup start.");
  this->publish_delta_ = publish_delta;
  this->time_constant_ = time_constant;
  this->window_size_ = window_size;
  // initial state publish
  this->publish_state(0);
  ESP_LOGCONFIG(TAG, "PressureTestSensor setup complete.");
}

void PressureTestSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureTestSensor:");
  ESP_LOGCONFIG(TAG, "  publish delta: %.2f", this->publish_delta_);
  ESP_LOGCONFIG(TAG, "  time constant: %.2f", this->time_constant_);
  ESP_LOGCONFIG(TAG, "  window size: %d", this->window_size_);
}

void PressureTestSensor::process(float pressure) {
  if (this->prev_pressure_sensor_test_flag_ != this->fswm100_->get_pressure_sensor_test_flag()) {
    this->prev_pressure_sensor_test_flag_ = this->fswm100_->get_pressure_sensor_test_flag();
    if (this->prev_pressure_sensor_test_flag_) {
      // started
      this->start_pressure_ = pressure;
      this->prev_filtered_value_ = 0;
      this->fswm100_->pressure_test_sensor_publish(0);
      this->filter_ = new LowPassFilter(this->time_constant_, this->window_size_);
    } else {
      // stopped. reset
      this->fswm100_->pressure_test_sensor_publish(0);
    }
  } else if (this->prev_pressure_sensor_test_flag_) {
    // test in progress
    float filtered_val_opt = this->filter_->add_sample(pressure - this->start_pressure_);
    if (filtered_val_opt && abs(prev_filtered_value_ - filtered_val_opt) > this->publish_delta_) {
      prev_filtered_value_ = filtered_val_opt;
      this->fswm100_->pressure_test_sensor_publish(filtered_val_opt);
    }
  }
}

}  // namespace fswm100
}  // namespace esphome
