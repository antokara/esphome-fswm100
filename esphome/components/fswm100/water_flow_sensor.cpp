#include "water_flow_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace water_flow_sensor {

static const char *TAG = "water_flow_sensor.sensor";

void WaterFlowSensor::setup() {}

void WaterFlowSensor::loop() {}

void WaterFlowSensor::update() { this->publish_state(1.2345); }

void WaterFlowSensor::dump_config() { ESP_LOGCONFIG(TAG, "Water Flow Sensor..."); }

}  // namespace water_flow_sensor
}  // namespace esphome
