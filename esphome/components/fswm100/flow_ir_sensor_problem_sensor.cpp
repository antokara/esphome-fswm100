#include "esphome/core/log.h"
#include "fswm100.h"
#include "flow_sensor_problem_sensor.h"

namespace esphome {
namespace fswm100 {

FlowIrSensorProblemSensor::FlowIrSensorProblemSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void FlowIrSensorProblemSensor::setup() {
  ESP_LOGCONFIG(TAG, "FlowIrSensorProblemSensor setup start.");
  // initial state publish
  this->publish_state(false);
  ESP_LOGCONFIG(TAG, "FlowIrSensorProblemSensor setup complete.");
}

void FlowIrSensorProblemSensor::dump_config() { ESP_LOGCONFIG(TAG, "FlowIrSensorProblemSensor:"); }

void FlowIrSensorProblemSensor::loop() {}

}  // namespace fswm100
}  // namespace esphome
