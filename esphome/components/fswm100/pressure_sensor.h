#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/ads1115/ads1115.h"

/**
 * @brief The considerable percentage drop in pressure that can be correlated
 *        to water flow switching to active.
 */
#define CONSIDERABLE_PRESSURE_DROP_PERCENTAGE -5.0

/**
 * @brief The number of consecutive mismatches between
 *       active flow and pressure drop, to consider
 *       it a fault.
 */
#define FLOW_PRESSURE_CORRELATION_FAULT_THRESHOLD 5

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief the pressure sensor class that reads the transducer voltage
 *        from the ADS1115 and converts it to pressure,
 *        using the specs of the transducer and the calibration config.
 *
 *        It publishes the pressure value depending on the noise floor and
 *        also performs immediate publishes (bypassing any defined sensor filters)
 *        as needed (e.g. when the pressure sensor calibration multiplier changes and
 *        when the pressure test starts/stops).
 */
class PressureSensor : public sensor::Sensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PressureSensor(FSWM100 *fswm100);

  /**
   * setup the pressure sensor
   */
  void setup(float effective_noise_floor, float min_voltage, float max_voltage, float min_pressure, float max_pressure,
             ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain, ads1115::ADS1115Samplerate sample_rate,
             ads1115::ADS1115Resolution resolution);

  void dump_config();

  /**
   * @brief to be called in the loop() method of the parent component
   * it checks if the state has changed and if it should be published.
   * if yes, it publishes the state.
   *
   * it takes into account the publish frequency, delta values and
   * whether the test is enabled or not.
   */
  void loop();

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
   * @brief the last sensor state
   */
  float last_sensor_state_{0};

  /**
   * @brief the pressure sensor calibration multiplier state
   *        last time we published the state.
   *
   *        if this changed, it means we should publish without
   *        using filters, to allow the user to calibrate the sensor
   *        and see the new state immediatelly, even for small changes.
   */
  float last_pressure_sensor_calibration_multiplier_{0};

  /**
   * @brief the last state value of the pressure sensor test flag.
   *       This is used to determine if the pressure sensor test has started or stopped.
   */
  bool last_pressure_sensor_test_flag_{false};

  /**
   * @brief voltage fluctuations less than, or equal to this value
   *        will be ignored, as noise.
   *        This is used to filter out noise from the pressure sensor.
   *
   *        Unfortuately, the pressure sensor has too much fluctuation/noise and
   *        we can't really use the floor to completely eliminate it.
   *        It actually fluctuates up to +/-1V at times, therefore,
   *        we must use a decent delta filter and only use the effective noise floor
   *        to filter out the "power supply" noise and in case we find a more
   *        stable pressure sensor in the future...
   */
  float effective_noise_floor_{0.0f};

  /**
   * @brief the last time (in milliseconds since boot)
   *        the pressure dropped considerably (
   *        more than the defined threshold).
   * @see CONSIDERABLE_PRESSURE_DROP_PERCENTAGE
   */
  uint32_t last_time_pressure_dropped_considerably_{0};

  /**
   * @brief counts how many times in a row
   *        we had a mismatch between active flow and pressure drop.
   *
   *       if this exceeds a threshold, we have a fault.
   * @see FLOW_PRESSURE_CORRELATION_FAULT_THRESHOLD
   */
  int flow_pressure_correlation_fault_counter_{0};

  /**
   * @brief for diagnostics, we keep the last flow sensor state
   *        to know if is just switched to active, inactive, etc.
   */
  float last_flow_sensor_state_{0.0f};
};

}  // namespace fswm100
}  // namespace esphome
