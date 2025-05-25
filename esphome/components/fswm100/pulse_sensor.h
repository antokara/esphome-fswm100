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
   */
  void setup(GPIOPin *pulse_sensor_gpio_pin);
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
   * @brief the published state
   */
  bool last_publish_state_{false};

  /**
   * @brief how much to increase the Water Meter Counter by
   * @example 1, with Gallons as a Volume Unit,
   *          will increase by 1gal, per pulse
   *
   */
  // unsigned count_volume = 1;

  // /**
  //  * @brief the frequency in relation to the volume, in Seconds.
  //  * @example 60, with Gallons as a Volume Unit and count_volume 1,
  //  *          one Pulse will be 1gal/min.
  //  *
  //  */
  // unsigned int count_frequency = 60;

  // /**
  //  * @brief duration for debouncing pulses, in milliseconds.
  //  *        helps in case the pulse switch/sensor,
  //  *        toggles with noise too fast.
  //  *
  //  */
  // unsigned int debounce = 300;

  // /**
  //  * @brief minimum Water Flow Volume (in relation to the count_frequency),
  //  *        that the Water Meter can detect and therefore, the Pulse Sensor can indicate.
  //  * @example For a good Positive Displacement Water Meter, that can detect about 0.1 gal/min.
  //  *          we would set this to 0.1, assuming count_frequency was already set to 60.
  //  *
  //  */
  // float min_flow_volume = 0.1;
};

}  // namespace fswm100
}  // namespace esphome
