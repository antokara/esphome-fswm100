#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace water_flow_sensor {

class WaterFlowSensor : public sensor::Sensor, public PollingComponent {
  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;
};

}  // namespace water_flow_sensor
}  // namespace esphome
