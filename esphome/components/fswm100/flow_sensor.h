#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/ads1115/ads1115.h"

/**
 * time in milliseconds for our target rate
 * 60000msecs = 60secs = 1minute rate for GPM
 */
#define FLOW_RATE_TIME 60000.0

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

class FlowSensor : public sensor::Sensor {
 public:
  /**
   * @param fswm100 the parent component class
   */
  FlowSensor(FSWM100 *fswm100);

  /**
   * setup the flow sensor
   */
  void setup(float effective_noise_floor, float min_volume, float rate_time, uint32_t publish_frequency,
             ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain, ads1115::ADS1115Samplerate sample_rate,
             ads1115::ADS1115Resolution resolution);

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
   * @brief calculate the active flow
   */
  float calculate_active_flow();

  /**
   * @brief attempt to publish the given flow rate
   */
  void try_publish(float rate);

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
   * @brief the last pulse sensor state we got
   */
  bool last_pulse_sensor_state_{false};

  /**
   * @brief the last time we switched to
   *        an active pulse sensor reading.
   *
   *        useful, so we can calculate the current flow rate...
   *
   */
  uint32_t last_pulse_sensor_active_time_{0};

  /**
   * @brief how frequently to publish the flow rate.
   *        it does not apply when the flow starts/stops,
   *        so that we immediatelly publish those events.
   *        This only applies while the flow is active,
   *        basically, for flow rate change events.
   * @see last_publish_time_
   */
  uint32_t publish_frequency_{0};

  /**
   * @brief the time when the last publish
   *        of flow rate change took place
   * @see publish_frequency_
   */
  uint32_t last_publish_time_{0};
};

}  // namespace fswm100
}  // namespace esphome
