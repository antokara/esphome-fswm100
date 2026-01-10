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
 * @brief the problem sensor, of the flow IR sensor.
 *        when true, the flow IR sensor is having a problem:
 *           - we are getting voltage readings that are not within the expected range
 *           - I2C or ADS1115 errors
 *        when false, the flow IR sensor is working properly.
 */
class FlowIrSensorProblemSensor : public binary_sensor::BinarySensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  FlowIrSensorProblemSensor(FSWM100 *fswm100);

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
