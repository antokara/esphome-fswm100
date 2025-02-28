#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace fswm100 {

class FSWM100 : public Component {
 public:
  /**
   * @brief Sets the flow sensor object.
   * @see sensor.py:to_code(config)
   *
   * @param flow_sensor
   */
  void set_flow_sensor(sensor::Sensor *flow_sensor) { flow_sensor_ = flow_sensor; }

  /**
   * @brief Set the pulse sensor object
   * @see sensor.py:to_code(config)
   *
   * @param pulse_sensor
   */
  void set_pulse_sensor(sensor::Sensor *pulse_sensor) { pulse_sensor_ = pulse_sensor; }

  /**
   * @brief Set the pressure sensor object
   * @see sensor.py:to_code(config)
   *
   * @param pressure_sensor
   */
  void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }

  void loop() override;
  void dump_config() override;

 protected:
  /**
   * @brief the flow sensor
   *
   */
  sensor::Sensor *flow_sensor_{nullptr};

  /**
   * @brief the pulse sensor
   *
   */
  sensor::Sensor *pulse_sensor_{nullptr};

  /**
   * @brief the pressure sensor
   *
   */
  sensor::Sensor *pressure_sensor_{nullptr};

  uint32_t last_transmission_{0};
};

}  // namespace fswm100
}  // namespace esphome
