#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_test_sensor.h"

namespace esphome {
namespace fswm100 {

PressureTestSensor::PressureTestSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureTestSensor::setup(const std::function<std::vector<sensor::Filter *>()> &filters_factory) {
  ESP_LOGCONFIG(TAG, "PressureTestSensor setup start.");
  this->filters_factory_ = filters_factory;
  // initial state publish
  this->publish_state(0);
  ESP_LOGCONFIG(TAG, "PressureTestSensor setup complete.");
}

void PressureTestSensor::dump_config() { ESP_LOGCONFIG(TAG, "PressureTestSensor:"); }

void PressureTestSensor::publish(float pressure, bool immediate) {
  if (immediate) {
    this->reset_filters();
  }
  this->publish_state(pressure);
}

void PressureTestSensor::process(float pressure) {
  if (this->prev_pressure_sensor_test_flag_ != this->fswm100_->get_pressure_sensor_test_flag()) {
    this->prev_pressure_sensor_test_flag_ = this->fswm100_->get_pressure_sensor_test_flag();
    if (this->prev_pressure_sensor_test_flag_) {
      // started
      this->start_pressure_ = pressure;
      this->publish(0, true);
    } else {
      // stopped
      this->publish(0, true);
    }
  } else if (this->prev_pressure_sensor_test_flag_) {
    // test in progress
    this->publish(pressure - this->start_pressure_, false);
  }
}

void PressureTestSensor::reset_filters() {
  if (this->filters_factory_) {
    ESP_LOGD(TAG, "PressureTestSensor Resetting filters by creating new filter instances...");
    this->clear_filters();
    for (auto *filter : this->filters_factory_()) {
      this->add_filter(filter);
    }
  } else {
    ESP_LOGW(TAG, "PressureTestSensor No filter factory set, cannot reset filters.");
  }
}

}  // namespace fswm100
}  // namespace esphome
