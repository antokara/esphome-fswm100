#include "esphome/core/log.h"
#include "fswm100.h"
#include "flow_sensor_problem_sensor.h"

namespace esphome {
namespace fswm100 {

FlowSensorProblemSensor::FlowSensorProblemSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void FlowSensorProblemSensor::setup() {
  ESP_LOGCONFIG(TAG, "FlowSensorProblemSensor setup start.");
  // initial state publish
  this->publish_state(false);
  ESP_LOGCONFIG(TAG, "FlowSensorProblemSensor setup complete.");
}

void FlowSensorProblemSensor::dump_config() { ESP_LOGCONFIG(TAG, "FlowSensorProblemSensor:"); }

void FlowSensorProblemSensor::loop() {}

}  // namespace fswm100
}  // namespace esphome
