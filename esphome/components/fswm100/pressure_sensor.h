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

class PressureSensor : public sensor::Sensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PressureSensor(FSWM100 *fswm100);

  /**
   * setup the pressure sensor
   */
  void setup(float min_voltage, float max_voltage, float min_pressure, float max_pressure, float publish_delta,
             float publish_frequency, float publish_delta_test, float publish_frequency_test,
             ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain, ads1115::ADS1115Samplerate sample_rate,
             ads1115::ADS1115Resolution resolution);

  void dump_config();

  /**
   * @brief get the state of the pressure sensor
   *
   * @return float the pressure sensor state
   */
  float get_state();

  /**
   * @brief to be called in the loop() method of the parent component
   * it checks if the state has changed and if it should be published.
   * if yes, it publishes the state.
   *
   * it takes into account the publish frequency, delta values and
   * whether the test is enabled or not.
   */
  void loop();

 private:
  /**
   * @brief the parent component
   *
   */
  FSWM100 *fswm100_{nullptr};

  /**
   * @brief ADS1115 multiplexer
   */
  ads1115::ADS1115Multiplexer multiplexer_;
  /**
   * @brief ADS1115 gain
   */
  ads1115::ADS1115Gain gain_;
  /**
   * @brief ADS1115 sample rate
   */
  ads1115::ADS1115Samplerate sample_rate_;
  /**
   * @brief ADS1115 resolution
   */
  ads1115::ADS1115Resolution resolution_;

  /**
   * @brief the minimum voltage that the
   * Pressure Transducer Sensor can output
   */
  float min_voltage_;

  /**
   * @brief the maximum voltage that the
   * Pressure Transducer Sensor can output
   */
  float max_voltage_;

  /**
   * @brief the minimum pressure that the
   * Pressure Transducer Sensor can sense
   */
  float min_pressure_;

  /**
   * @brief the maximum pressure that the
   * Pressure Transducer Sensor can sense
   */
  float max_pressure_;

  /**
   * @brief the delta value to publish
   * the pressure sensor state
   */
  float publish_delta_;

  /**
   * @brief the frequency to publish
   * the pressure sensor state
   */
  float publish_frequency_;

  /**
   * @brief the delta value to publish
   * the pressure sensor during a test
   */
  float publish_delta_test_;

  /**
   * @brief the frequency to publish
   * the pressure sensor during a test
   */
  float publish_frequency_test_;

  /**
   * @brief the voltage factor
   *
   * This is used to convert the voltage to pressure.
   * It is calculated as follows:
   * voltage_factor = (max_pressure - min_pressure) / (max_voltage - min_voltage)
   *
   * @see setup()
   */
  float voltage_factor_;

  /**
   * @brief converts voltage to pressure,
   * using all the parameters set in setup() plus
   * the pressure sensor calibration (multiplier).
   *
   * Warning: It does not restrict the pressure to the min/max range but
   * it uses the min/maxs to calculate the pressure. Meaning,
   * if the voltage is outside the expected min/max voltage range,
   * the pressure will be outside the expected min/max pressure range.
   *
   * @param voltage the voltage to convert
   * @return the converted pressure
   */
  float voltage_to_pressure(float voltage);

  /**
   * @brief the last time we published the state
   */
  uint32_t last_publish_time_{0};
};

}  // namespace fswm100
}  // namespace esphome
