#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/ads1115/ads1115.h"

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief the flow IR sensor class that reads the IR fluctuations
 *        from the ADS1115.
 *
 *        This IR sensor points at the flow turbine/indicator of the water meter.
 *        The one that rotates when the smallest amount of water flows.
 *
 *        It uses the ADS1115 multiplexer, gain, sample rate and resolution
 *        to read the IR fluctuations.
 */
class FlowIrSensor : public binary_sensor::BinarySensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  FlowIrSensor(FSWM100 *fswm100);

  /**
   * setup the flow ir sensor
   */
  void setup(float noise_inverse_curve_k_, float noise_inverse_curve_c_, float min_voltage, float max_voltage,
             uint32_t debug_publish_interval_ms, ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
             ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution);

  /**
   * @brief to be called in the loop() method of the parent component
   * it checks if the state has changed and if it should be published.
   * if yes, it publishes the state.
   */
  void loop();

  void dump_config();

  /**
   * @brief sets the raw_state_ and attempts to publish the IR active/inactive state.
   * @param ir_active IR active (true) or inactive (false)
   */
  void publish(bool ir_active);

  /**
   * @brief if true, the sensor has a self detected fault.
   *        those are faults that can be determined in isolation.
   *        e.g. out of range values, etc. but does not include
   *        faults the require readings/timings from other sensors.
   *        if false, the sensor as far as it knows, is working fine.
   */
  bool has_fault();

  /**
   * @brief returns the raw state (unaffected by filters)
   */
  bool get_raw_state();

 private:
  /**
   * @brief get the state of the flow sensor
   *
   *  - Black Surface/Low reflection leads to
   *    decreased phototransistor conductivity,
   *    causing the output voltage to be higher, closer to Vcc (4.9V raw).
   *
   *  - White Surface/High reflection leads to
   *    increased phototransistor conductivity,
   *    causing the output voltage to be lower, closer to GND (0.1V raw).
   *
   * The precision we can achieve with our circuit
   * (power supply, ADS1115 and the TCR5000) is about 0.05V.
   * This means that any voltage flactuation below 0.05V
   * should be ignored...
   *
   * @return float the voltage read from the ADS1115 channel
   *               that corresponds to the flow sensor.
   */
  float get_state();

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
   * @brief raw state (unaffected by filters)
   */
  bool raw_state_{false};

  /**
   * @brief the last sensor state
   */
  float last_sensor_state_{0};

  /**
   * @brief the max state delta value,
   *        since the last debug state publish
   */
  float debug_state_delta_max_{0.0f};

  /**
   * @brief the min state value,
   *        since the last debug state publish.
   *        useful to determine expected/normal range of voltage.
   *
   *  @example 2.5059
   */
  float debug_state_min_{0.0f};

  /**
   * @brief the max state value,
   *        since the last debug state publish
   *        useful to determine expected/normal range of voltage.
   *
   * @example 2.6359
   */
  float debug_state_max_{0.0f};

  /**
   * @brief counts of how many times
   * within the last debug state period
   * the state went from under the noise floor,
   * to above the noise floor.
   */
  int debug_state_active_counts_{0};

  /**
   * @brief the previous state used in the
   * debug state counts calculation.
   *
   * (true if it was above the noise floor,
   * false if it was below the noise floor)
   */
  bool debug_state_active_counts_previous_{false};

  /**
   * @brief the last time we published a debug state
   */
  uint32_t last_debug_state_time_{0};

  /**
   * @brief the inverse curve k parameter
   *        used to calculate the effective noise floor
   *
   * @example with m 3.0V, k 0.1, c 1.0, when the sensor is new/sensitive:
   *          f = 0.1 / (3.0 - 1) = 0.05V
   *          with m 3.0V, k 0.1, c 1.5:
   *          f = 0.1 / (3.0 - 1.5) = 0.066V
   *          with m 3.0V, k 0.1, c 0.5:
   *          f = 0.1 / (3.0 - 0.5) = 0.04V
   */
  float noise_inverse_curve_k_{0.1f};

  /**
   * @brief the inverse curve c parameter
   *        used to calculate the effective noise floor
   * @example with m 3.0V, k 0.1, c 1.0, when the sensor is new/sensitive:
   *          f = 0.1 / (3.0 - 1) = 0.05V
   *          with m 3.0V, k 0.1, c 1.5:
   *          f = 0.1 / (3.0 - 1.5) = 0.066V
   *          with m 3.0V, k 0.1, c 0.5:
   *          f = 0.1 / (3.0 - 0.5) = 0.04V
   */
  float noise_inverse_curve_c_{1.0f};

  /**
   * @brief the max voltage recorded over time
   *        used to calculate the effective noise floor.
   *
   *        This is different from max_voltage_, which is a fixed threshold
   *        above which the sensor is considered faulty.
   *
   *        The max_voltage_recorded_ should be within the min_voltage_ and max_voltage_ range.
   *
   * @example 2.9359V
   */
  float max_voltage_recorded_{0.0f};

  /**
   * @brief voltage fluctuations less than, or equal to this value
   *        will be ignored, as noise.
   *        This is used to filter out noise from the flow sensor.
   *
   *        We calculate the effective noise floor, using the inverse curve:
   *          f = k / (m - c)
   *        where:
   *          f = effective noise floor. [effective_noise_floor_]
   *          k = constant/strength. Scaling factor (e.g. 0.1). [noise_inverse_curve_k_]
   *          m = max voltage (e.g. 3.0V or 3.5V). [max_voltage_recorded_]
   *          c = limit/shift. Horizonal shift (e.g. 1). [noise_inverse_curve_c_]
   */
  float effective_noise_floor_{0.0f};

  /**
   * @brief calculates the effective noise floor
   * @param max_voltage the max voltage to use in the calculation
   */
  void calculate_effective_noise_floor(float max_voltage);

  /**
   * @brief rounds a float value to the specified number of decimal places
   * @param value the value to round
   * @param decimal_places the number of decimal places to round to
   * @return float the rounded value
   */
  float round_to_decimal_places(float value, int decimal_places);

  /**
   * @brief The minimum IR voltage that is considered valid.
   *        Below this value, the sensor is considered to be faulty or
   *        should be replaced because it may have reached its end of life.
   */
  float min_voltage_{2.3f};

  /**
   * @brief The maximum IR voltage that is considered valid.
   *        Above this value, the sensor is considered to be faulty or
   *        should be replaced because it may have reached its end of life.
   */
  float max_voltage_{2.8f};

  /**
   * @brief The interval in milliseconds to publish the debug state meta information
   *        of the flow sensor.
   */
  uint32_t debug_publish_interval_ms_{30000};
};

}  // namespace fswm100
}  // namespace esphome
