#include "esphome/core/log.h"
#include "fswm100.h"
#include "pulse_sensor_problem_sensor.h"

namespace esphome {
namespace fswm100 {

PulseSensorProblemSensor::PulseSensorProblemSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PulseSensorProblemSensor::setup() {
  ESP_LOGCONFIG(TAG, "PulseSensorProblemSensor setup start.");
  // initial state publish
  this->publish_state(false);
  ESP_LOGCONFIG(TAG, "PulseSensorProblemSensor setup complete.");
}

void PulseSensorProblemSensor::dump_config() { ESP_LOGCONFIG(TAG, "PulseSensorProblemSensor:"); }

void PulseSensorProblemSensor::loop() {}

}  // namespace fswm100
}  // namespace esphome
