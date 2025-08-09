#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor_problem_sensor.h"

namespace esphome {
namespace fswm100 {

PressureSensorProblemSensor::PressureSensorProblemSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensorProblemSensor::setup() {
  ESP_LOGCONFIG(TAG, "PressureSensorProblemSensor setup start.");
  // initial state publish
  this->publish_state(false);
  ESP_LOGCONFIG(TAG, "PressureSensorProblemSensor setup complete.");
}

void PressureSensorProblemSensor::dump_config() { ESP_LOGCONFIG(TAG, "PressureSensorProblemSensor:"); }

void PressureSensorProblemSensor::loop() {}

}  // namespace fswm100
}  // namespace esphome
