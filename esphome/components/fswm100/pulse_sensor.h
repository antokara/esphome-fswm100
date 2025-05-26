#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

class PulseSensor : public binary_sensor::BinarySensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PulseSensor(FSWM100 *fswm100);

  /**
   * @brief setup the pulse sensor
   *
   * @param pulse_sensor_gpio_pin the GPIO pin to use
   * @param rate_volume the rate volume
   */
  void setup(GPIOPin *pulse_sensor_gpio_pin, float rate_volume);
  void dump_config();

  /**
   * @brief get the state of the pulse sensor
   *
   * @return true if the pulse sensor is active
   * @return false if the pulse sensor is inactive
   */
  bool get_state();

  /**
   * @brief to be called in the loop() method of the parent component
   * it checks if the state has changed and if it should be published.
   * if yes, it publishes the state.
   */
  void loop();

  /**
   * @brief returns the rate volume
   */
  float get_rate_volume();

 private:
  /**
   * @brief the parent component
   *
   */
  FSWM100 *fswm100_{nullptr};

  /**
   * @brief the GPIO pin that will be a digital input
   * without any internal pull up/down resistor and
   * when HIGH, a pulse from the Water Meter will be counted.
   *
   * This must be connected to the Reed Switch of the Water Meter.
   *
   */
  GPIOPin *pin_ = nullptr;

  /**
   * @brief the rate volume increase, per pulse
   * @example for a water meter with Pulse Rate of 1 Pulse/Gallon
   *          this should be set to 1.0
   */
  float rate_volume_{1.0};
};

}  // namespace fswm100
}  // namespace esphome
