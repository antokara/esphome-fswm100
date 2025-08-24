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
 * @brief the problem sensor, of the pulse sensor.
 *        when true, the pulse sensor is having a problem:
 *           - we got an IR signal but no pulse longer than the minimum duration (could be pulse or IR)
 *           - we are getting pulses faster than the maximum rate
 *        when false, the pulse sensor is working properly.
 */
class PulseSensorProblemSensor : public binary_sensor::BinarySensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PulseSensorProblemSensor(FSWM100 *fswm100);

  /**
   * @brief setup the sensor
   */
  void setup();
  void dump_config();

  /**
   * @brief to be called in the loop() method of the parent component
   */
  void loop();

 private:
  /**
   * @brief the parent component
   *
   */
  FSWM100 *fswm100_{nullptr};
};

}  // namespace fswm100
}  // namespace esphome
