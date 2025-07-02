#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/ads1115/ads1115.h"

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief the sensor that keeps track of the pressure during
 * the pressure test. This is the value that starts and ends at 0
 * with the goal to show positive/negative pressure changes
 * during the pressure test.
 */
class PressureTestSensor : public sensor::Sensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PressureTestSensor(FSWM100 *fswm100);

  /**
   * setup the pressure test sensor
   */
  void setup();

  void dump_config();

  /**
   * @brief attempt to publish
   */
  void publish(float pressure, bool immediate);

  void process(float pressure);

  /**
   * @brief internal method to send the state to the frontend.
   *        this does not go through the filters and it actually
   *        what the last filter uses in its Filter::output method.
   *
   *        When we override this method, we can skip sending
   *        the state to the frontend, which is useful for
   *        resetting the filters without sending "invalid/reset" state values.
   *        That's because the filters will process the state
   *        but we can control whether to send it to the frontend or not.
   *
   * @param state the state to send
   */
  void internal_send_state_to_frontend(float state);

 private:
  /**
   * @brief the parent component
   *
   */
  FSWM100 *fswm100_{nullptr};

  /**
   * @brief the previous pressure sensor test flag
   */
  bool prev_pressure_sensor_test_flag_{false};

  /**
   * @brief the starting pressure
   *
   * This is used to store the pressure when the pressure sensor test starts.
   * It is used to calculate the pressure difference during the test.
   */
  float start_pressure_{0.0f};

  /**
   * @brief whether to skip sending the state to the frontend.
   *        This is useful for allowing us to reset any applied filters
   *        without sending "invalid/reset" state values to the frontend.
   */
  bool skip_send_to_frontend_{false};
};

}  // namespace fswm100
}  // namespace esphome
