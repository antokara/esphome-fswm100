#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/ads1115/ads1115.h"

/**
 * @brief The multiplier applied to the time between pulses
 *        to determine if the flow rate should be re-calculated.
 *        This is used to avoid re-calculating the flow rate too often,
 *        which leads to spikes in the flow rate.
 */
#define FLOW_RATE_TIME_BETWEEN_PULSES_MULTIPLIER 1.25

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief the flow sensor class that reads the IR fluctuations
 *        from the ADS1115 in tandem with the pulse sensor and
 *        converts that to a flow rate, which it publishes.
 *
 *        It uses the ADS1115 multiplexer, gain, sample rate and resolution
 *        to read the IR fluctuations.
 *
 *        It also calculates the flow rate based on the time since the last pulse sensor state change,
 *        the rate time and the pulse rate volume.
 */
class FlowSensor : public sensor::Sensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  FlowSensor(FSWM100 *fswm100);

  /**
   * setup the flow sensor
   */
  void setup(const std::function<std::vector<sensor::Filter *>()> &filters_factory, float effective_noise_floor,
             float min_volume, float rate_time, ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
             ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution);

  /**
   * @brief get the state of the flow sensor
   *
   * @return float the voltage read from the ADS1115 channel
   *               that corresponds to the flow sensor.
   */
  float get_state();

  /**
   * @brief to be called in the loop() method of the parent component
   * it checks if the state has changed and if it should be published.
   * if yes, it publishes the state.
   */
  void loop();

  void dump_config();

  /**
   * @brief to be called when there's active flow.
   *        it could be when it just switched to active or
   *        when it's sustained active flow.
   */
  void active();

  /**
   * @brief calculate and return the active flow,
   *        based on the time since the last pulse sensor state change,
   *       the rate time and the pulse rate volume.
   */
  float calculate_active_flow();

  /**
   * @brief attempt to publish the given flow rate
   * @param rate the flow rate to publish
   * @param immediate if true, it will publish the rate immediately,
   *                  bypassing any filters that might be defined.
   *                  Even if false, if the flow just stopped or started,
   *                  it will publish the rate immediately.
   */
  void publish(float rate, bool immediate = false);

  /**
   * @brief resets the filters to the filters defined in the yaml configuration.
   *        this basically, clears any "history" or "cache" of the filters and
   *        thus, allows us to immediatelly publish a new state value
   *        when needed, without any previous values affecting it.
   *
   *        This is useful when the pressure sensor test starts or stops and
   *        we do not want any previous test values to affect the new...
   */
  void reset_filters();

  /**
   *
   */
  uint32_t time_since_pulse();

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
   * @brief the last sensor state
   */
  float last_sensor_state_{0};

  /**
   * @brief the max state delta value,
   *        since the last debug state publish
   */
  float debug_state_delta_max_{0.0f};

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
   * @brief voltage fluctuations less than, or equal to this value
   *        will be ignored, as noise.
   *        This is used to filter out noise from the flow sensor.
   */
  float effective_noise_floor_{0.0f};

  /**
   * @brief the minimum flow volume the water meter can detect
   * @example for a gal/min water meter with Min Flow Rate: 0.125GPM
   *             this should be set to 0.125
   */
  float min_volume_{0.0f};

  /**
   * @brief the rate of time used by the water meter, in seconds.
   *        most of the times that's 1 minute.
   * @example for a gal/min water meter, this should be set to 60.0
   */
  float rate_time_{60.0f};

  /**
   * @brief the last time the flow was active
   */
  uint32_t last_active_time_{0};

  /**
   * @brief the last pulse sensor state we got.
   *        start with true, in case the device boots up
   *        and the pulse sensor is already active.
   *        to avoid sending a false flow rate.
   */
  bool last_pulse_sensor_state_{true};

  /**
   * @brief the oldest time we have, of
   *        when switched to an active pulse sensor reading.
   *
   *        we need 2 points in time to calculate the flow rate,
   *        since we need to know how fast the pulse sensor
   *        triggered, in order to calculate the flow rate...
   */
  uint32_t oldest_pulse_sensor_active_time_{0};

  /**
   * @brief the newest time we have, of
   *        when switched to an active pulse sensor reading.
   */
  uint32_t newest_pulse_sensor_active_time_{0};

  /**
   *  A function object that knows how to create a new set of filters
   * e.g. []() -> std::vector<esphome::sensor::Filter *> { return { new
   * esphome::sensor::SlidingWindowMovingAverageFilter(15, 5, 1), new esphome::sensor::OffsetFilter(10.0) }; }
   */
  std::function<std::vector<sensor::Filter *>()> filters_factory_;
};

}  // namespace fswm100
}  // namespace esphome
