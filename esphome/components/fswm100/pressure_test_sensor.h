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
  void setup(float publish_delta, float time_constant, int window_size);

  void dump_config();

  void process(float pressure);

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
   * @brief the low pass filter used to smooth the pressure sensor readings
   */
  LowPassFilter *filter_{nullptr};

  /**
   * @brief the previously filtered value from the filter
   */
  float prev_filtered_value_{0.0f};  // Last filtered value from the filter

  /**
   * @brief the delta value to publish the pressure test sensor value
   *
   * This is used to determine if the pressure test sensor value has changed enough
   * to be published. If the change is less than this value, it will not be published.
   */
  float publish_delta_{0.0f};

  /**
   * @brief the time constant for the low pass filter
   *
   * This is used to determine how quickly the filter responds to changes in the pressure sensor readings.
   * A lower value means the filter will respond more quickly, while a higher value means it will respond more slowly.
   */
  float time_constant_{7.0f};

  /**
   * @brief the window size for the low pass filter
   *
   * This is used to determine how many samples are needed before the filter output is considered valid.
   * It must be at least 1, and it defines how many samples are averaged in the filter.
   */
  int window_size_{3};
};

}  // namespace fswm100
}  // namespace esphome
