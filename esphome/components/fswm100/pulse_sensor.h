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

/**
 * @brief the pulse sensor class that reads the digital input
 *        from the GPIO pin connected to the Water Meter's Reed Switch.
 *
 *        It is used in tandem with the FlowSensor to calculate the flow rate.
 */
class PulseSensor : public binary_sensor::BinarySensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PulseSensor(FSWM100 *fswm100);

  /**
   * @brief setup the pulse sensor
   */
  void setup(GPIOPin *pulse_sensor_gpio_pin, float rate_volume, float fault_flow_timeout_multiplier);
  void dump_config();

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

  /**
   * @brief returns the last time the pulse sensor toggled.
   *        it is used for diagnostics.
   */
  uint32_t get_last_toggle_time();

  /**
   * @brief if true, the sensor has a self detected fault.
   *        those are faults that can be determined in isolation.
   *        e.g. out of range values, etc. but does not include
   *        faults the require readings/timings from other sensors.
   *        if false, the sensor as far as it knows, is working fine.
   */
  bool has_fault();

 private:
  /**
   * @brief if true, the sensor has a self detected fault.
   *        those are faults that can be determined in isolation.
   *        e.g. out of range values, etc. but does not include
   *        faults the require readings/timings from other sensors.
   *        if false, the sensor as far as it knows, is working fine.
   */
  bool has_fault_{false};

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

  /**
   * @brief the last time the pulse sensor toggled.
   *        it is used for diagnostics.
   */
  uint32_t last_toggle_time_{0};

  /**
   * @brief The multiplier applied to the flow time out period
   *        to determine if the pulse sensor is faulty (has not toggled for too long).
   *
   *        We use a multiplier to avoid false positives due to slight variations
   *        in the real flow time out period and the expected one.
   */
  float fault_flow_timeout_multiplier_{1.5f};
};

}  // namespace fswm100
}  // namespace esphome
