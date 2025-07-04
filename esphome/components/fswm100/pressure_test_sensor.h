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
   * @param filters_factory a function that returns a vector of sensor filters
   *                        that should be used for this sensor.
   */
  void setup(const std::function<std::vector<sensor::Filter *>()> &filters_factory);

  void dump_config();

  /**
   * @brief attempt to publish
   */
  void publish(float pressure, bool immediate);

  void process(float pressure);
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
   *  A function object that knows how to create a new set of filters
   * e.g. []() -> std::vector<esphome::sensor::Filter *> { return { new
   * esphome::sensor::SlidingWindowMovingAverageFilter(15, 5, 1), new esphome::sensor::OffsetFilter(10.0) }; }
   */
  std::function<std::vector<sensor::Filter *>()> filters_factory_;
};

}  // namespace fswm100
}  // namespace esphome
