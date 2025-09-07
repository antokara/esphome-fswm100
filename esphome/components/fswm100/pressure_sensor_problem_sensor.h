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
 * @brief the problem sensor, of the pressure sensor.
 *        when true, the pressure sensor is having a problem:
 *           - we are getting voltage readings that are not within the expected range
 *           - the pressure has not dropped significantly, even though the flow sensor
 *             has switched to active, a number of times in a row
 *           - I2C or ADS1115 errors
 *        when false, the pressure sensor is working properly.
 */
class PressureSensorProblemSensor : public binary_sensor::BinarySensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PressureSensorProblemSensor(FSWM100 *fswm100);

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
