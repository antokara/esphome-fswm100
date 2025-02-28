#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace fswm100 {

class FSWM100 : public Component {
 public:
  void loop() override;
  void set_flow_sensor(sensor::Sensor *flow_sensor) { flow_sensor_ = flow_sensor; }
  void set_pulse_sensor(sensor::Sensor *pulse_sensor) { pulse_sensor_ = pulse_sensor; }
  void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }
  void dump_config() override;

 protected:
  sensor::Sensor *flow_sensor_{nullptr};
  sensor::Sensor *pulse_sensor_{nullptr};
  sensor::Sensor *pressure_sensor_{nullptr};
};

}  // namespace fswm100
}  // namespace esphome
