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

void PressureTestSensor::dump_config() { ESP_LOGCONFIG(TAG, "PressureTestSensor:"); }

void PressureTestSensor::publish(float pressure, bool immediate) {
  this->publish_state(pressure);
  if (immediate) {
    this->internal_send_state_to_frontend(pressure);
  }
}

void PressureTestSensor::process(float pressure) {
  if (this->prev_pressure_sensor_test_flag_ != this->fswm100_->get_pressure_sensor_test_flag()) {
    this->prev_pressure_sensor_test_flag_ = this->fswm100_->get_pressure_sensor_test_flag();
    if (this->prev_pressure_sensor_test_flag_) {
      // started
      this->start_pressure_ = pressure;
      this->publish(0, true);
    } else {
      // stopped. reset
      this->publish(0, true);
    }
  } else if (this->prev_pressure_sensor_test_flag_) {
    // test in progress
    this->publish(pressure - this->start_pressure_, false);
  }
}

}  // namespace fswm100
}  // namespace esphome
