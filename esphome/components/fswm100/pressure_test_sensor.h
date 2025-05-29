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
};

}  // namespace fswm100
}  // namespace esphome
