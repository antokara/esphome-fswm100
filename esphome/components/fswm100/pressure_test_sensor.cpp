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

}  // namespace fswm100
}  // namespace esphome
