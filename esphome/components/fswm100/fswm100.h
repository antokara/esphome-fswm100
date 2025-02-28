#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace fswm100 {

class FSWM100 : public Component {
 public:
  // void setup() override;
  void loop() override;
  void set_flow_sensor(sensor::Sensor *flow_sensor) { flow_sensor_ = flow_sensor; }
  // void update() override;
  void dump_config() override;

 protected:
  sensor::Sensor *flow_sensor_{nullptr};
};

}  // namespace fswm100
}  // namespace esphome
