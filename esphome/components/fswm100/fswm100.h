#pragma once

#include "esphome/core/component.h"
// #include "esphome/components/sensor/sensor.h"
#include "pulse_sensor.h"

// #include "esphome/core/hal.h"
// #include "esphome/components/binary_sensor/binary_sensor.h"

// #include <vector>

// number of pulses per gallon (Pulse/Gallon)
// #define PULSE_RATE 1.0

/**
 * @brief frequency in milliseconds, to debounce the pulses.
 * in case the pulse switch toggles too fast for some reason within the defined
 * period, it will be ignored.
 */
#define PULSE_DEBOUNCE_FREQUENCY 250

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
  // void set_flow_sensor(sensor::Sensor *flow_sensor) { flow_sensor_ = flow_sensor; }

  /**
   * @brief Set the pulse sensor object
   * @see sensor.py:to_code(config)
   *
   * @param pulse_sensor
   */
  // void set_pulse_sensor(sensor::Sensor *pulse_sensor) { pulse_sensor_ = pulse_sensor; }

  /**
   * @brief Set the pressure sensor object
   * @see sensor.py:to_code(config)
   *
   * @param pressure_sensor
   */
  // void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }

  /**
   * @brief Set the pulse sensor gpio pin
   * @see sensor.py:to_code(config)
   *
   * @param pulse_sensor_gpio_pin
   */
  void set_pulse_sensor_gpio_pin(GPIOPin *pulse_sensor_gpio_pin) {
    this->pulse_sensor_gpio_pin_ = pulse_sensor_gpio_pin;
  }

  void setup() override;
  void loop() override;
  void dump_config() override;

  /**
   * @brief Get the setup priority object. It affects the order/priority,
   *        that this component, should get initialized by ESPHome.
   *
   * @return float
   */
  float get_setup_priority() const override;

  void set_pulse_sensor(PulseSensor *binary_sensor) { pulse_sensor_ = binary_sensor; }

 protected:
  /**
   * @brief the flow sensor
   *
   */
  // sensor::Sensor *flow_sensor_{nullptr};

  /**
   * @brief the pulse sensor
   *
   */
  PulseSensor *pulse_sensor_{nullptr};

  /**
   * @brief the pressure sensor
   *
   */
  // sensor::Sensor *pressure_sensor_{nullptr};

  /**
   * @brief the GPIO pin number to use,
   * in order to read the digital value of the pulse sensor
   *
   */
  GPIOPin *pulse_sensor_gpio_pin_;

  /**
   * @brief the previous value of the pulse sensor read
   *
   */
  // bool pulse_sensor_prev_value;

  /**
   * @brief if the pulse sensor is considered active (after debounce)
   *
   */
  bool pulse_sensor_active;

  /**
   * @brief the time (millis)
   * when the "pulse_sensor_value" was last toggled
   *
   */
  uint32_t pulse_sensor_active_time_{0};

  uint32_t last_transmission_{0};
};

}  // namespace fswm100
}  // namespace esphome
