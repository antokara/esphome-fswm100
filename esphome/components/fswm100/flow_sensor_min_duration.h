#pragma once

#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

/**
 * @brief the default value for the flow sensor minimum duration
 *        which is 20 seconds.
 */
#define FLOW_SENSOR_MIN_DURATION_DEFAULT_VALUE 20.0f

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief the minimum duration the water meter should detect flow for.
 *        this is required because at very low flow rates, the water
 *        meter is unable to consistently keep the flow propeller spinning.
 *        Instead, the propeller spins intermittently (e.g. a slight spin
 *        then stops spinning for 20 seconds and then spins slightly again,
 *        even though there could constant water flow of very low 0.1GPM).
 *        Therefore, we basically use a mechanism, similar to the
 *        ESPHome delayed_off filter.
 *
 * @example if a pulse is detected or if flow is detected from the IR
 *          of the water meter, the flow should be >0 for this duration.
 */
class FlowSensorMinDuration : public number::Number, public Component {
 public:
  /**
   * @param fswm100 the parent component class
   */
  FlowSensorMinDuration(FSWM100 *fswm100);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Called when Home Assistant (or other service) sends a command to change the number value
  void control(float value) override;

 private:
  /**
   * @brief the parent component
   *
   */
  FSWM100 *fswm100_{nullptr};
};

}  // namespace fswm100
}  // namespace esphome
