#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_test_sensor.h"

namespace esphome {
namespace fswm100 {

PressureTestSensor::PressureTestSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureTestSensor::setup() {
  ESP_LOGCONFIG(TAG, "PressureTestSensor setup start.");
  // initial state publish
  this->publish_state(0);
  ESP_LOGCONFIG(TAG, "PressureTestSensor setup complete.");
}

void PressureTestSensor::process(float pressure) {
  if (this->prev_pressure_sensor_test_flag_ != this->fswm100_->get_pressure_sensor_test_flag()) {
    this->prev_pressure_sensor_test_flag_ = this->fswm100_->get_pressure_sensor_test_flag();
    if (this->prev_pressure_sensor_test_flag_) {
      // started
      this->start_pressure_ = pressure;
      this->prev_filtered_value_ = 0;
      this->fswm100_->pressure_test_sensor_publish(0);
      // TODO: make the time constant and window size configurable
      double time_constant = 7.0;
      int window_size = 3;
      this->filter_ = new LowPassFilter(time_constant, window_size);
    } else {
      // stopped. reset
      this->fswm100_->pressure_test_sensor_publish(0);
    }
  } else if (this->prev_pressure_sensor_test_flag_) {
    // test in progress
    float filtered_val_opt = this->filter_->add_sample(pressure - this->start_pressure_);
    // TODO: make this configurable
    if (filtered_val_opt && abs(prev_filtered_value_ - filtered_val_opt) > 0.1) {
      prev_filtered_value_ = filtered_val_opt;
      this->fswm100_->pressure_test_sensor_publish(filtered_val_opt);
    }
  }
}

void PressureTestSensor::dump_config() { ESP_LOGCONFIG(TAG, "PressureTestSensor:"); }

}  // namespace fswm100
}  // namespace esphome
