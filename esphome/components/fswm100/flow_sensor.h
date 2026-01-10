#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief the flow sensor class that reads various sensors (IR, pulse) and
 *        converts that to a flow rate, which it publishes.
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
  void setup(const std::function<std::vector<sensor::Filter *>()> &filters_factory, float min_volume, float max_volume,
             float rate_time, float flow_rate_time_between_pulses_multiplier,
             float fault_time_since_flow_ir_activity_multiplier, int inactivity_fault_counter_threshold);

  /**
   * @brief get the state of the flow sensor
   * @return the flow rate
   */
  float get_state();

  /**
   * @brief get the min flow volume the meter is supposed to detect.
   *        This is useful for diagnostics.
   */
  float get_min_volume();

  /**
   * @brief get the max flow volume the meter is supposed to detect.
   *        This is useful for diagnostics.
   */
  float get_max_volume();

  /**
   * @brief returns the rate time in seconds.
   *       This is useful for diagnostics.
   */
  float get_rate_time();

  /**
   * @brief returns the last time (in milliseconds since boot)
   *        the flow switched to active from inactive.
   *        This is useful for diagnostics.
   */
  uint32_t get_last_switched_to_active_time();

  /**
   * @brief returns the last time (in milliseconds since boot)
   *        the flow switched to inactive from active.
   *        This is useful for diagnostics.
   */
  uint32_t get_last_switched_to_inactive_time();

  /**
   * @brief to be called in the loop() method of the parent component
   * it checks if the state has changed and if it should be published.
   * if yes, it publishes the state.
   */
  void loop();

  void dump_config();

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
   * @brief to be called when there's active flow.
   *        it could be when it just switched to active or
   *        when it's sustained active flow.
   */
  void active();

  /**
   * @brief the minimum flow volume the water meter can detect
   * @example for a gal/min water meter with Min Flow Rate: 0.125GPM
   *             this should be set to 0.125
   */
  float min_volume_{0.0f};

  /**
   * @brief the maximum flow volume the water meter can detect or
   *       is expected to be detected (e.g. the pipe is unable to provide more than that).
   *       This is used to detect faults in the flow sensor (e.g. when flow exceeds this).
   *
   *       If left/set to 0.0, it will not be used.
   * @example 15GPM for a residential 3/4" pipe is a good value.
   */
  float max_volume_{0.0f};

  /**
   * @brief the rate of time used by the water meter, in seconds.
   *        most of the times that's 1 minute.
   * @example for a gal/min water meter, this should be set to 60.0
   */
  float rate_time_{60.0f};

  /**
   * @brief the last flow IR sensor state
   */
  bool last_ir_sensor_state_{false};

  /**
   * @brief the last time the flow was active (not switched to active).
   *        (could be due to pulse sensor or IR voltage change).
   * @see last_switched_to_active_time_ for the switched to active time.
   */
  uint32_t last_active_time_{0};

  /**
   * @brief the time (millis) when the flow last switched to active
   *        from inactive (zero flow).
   */
  uint32_t last_switched_to_active_time_{0};

  /**
   * @brief the time (millis) when the flow last switched to inactive
   *        from active (non-zero flow).
   */
  uint32_t last_switched_to_inactive_time_{0};

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
   * @brief the last time we had any IR activity above the noise floor.
   *        This is used for diagnostics.
   */
  uint32_t last_ir_activity_time_{0};

  /**
   *  A function object that knows how to create a new set of filters
   * e.g. []() -> std::vector<esphome::sensor::Filter *> { return { new
   * esphome::sensor::SlidingWindowMovingAverageFilter(15, 5, 1), new esphome::sensor::OffsetFilter(10.0) }; }
   */
  std::function<std::vector<sensor::Filter *>()> filters_factory_;

  /**
   * @brief The multiplier applied to the time between pulses
   *        to determine if the flow rate should be re-calculated.
   *
   *        This is used to avoid re-calculating the flow rate too often,
   *        which leads to spikes in the flow rate.
   */
  float flow_rate_time_between_pulses_multiplier_{1.25f};

  /**
   * @brief The multiplier applied to the time since last activity
   *        to determine if the IR sensor is faulty (stuck on active),
   *        when there is a pulse sensor toggle.
   *
   *        This is needed because the IR sensor can be active for small
   *        "bursts/periods" and then inactive longer than the flow_sensor_min_duration,
   *        because the flow is ultra low, to the point that it gets missed by the flow sensor.
   *
   *        Therefore, we need to give it some extra time before we declare it faulty.
   */
  float fault_time_since_flow_ir_activity_multiplier_{3.0f};

  /**
   * @brief the current number of inactivity faults
   * that have happened in a row.
   */
  int inactivity_fault_counter_{0};

  /**
   * @brief the number of inactivity faults that must happen
   * in a row, to consider the sensor as faulty.
   */
  int inactivity_fault_counter_threshold_{3};

  /**
   * @brief for diagnostics, if true, a flow/pulse correlation check is pending.
   */
  bool flow_pulse_correlation_pending_{false};
};

}  // namespace fswm100
}  // namespace esphome
