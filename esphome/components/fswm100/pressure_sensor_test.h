#pragma once

#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief a switch that when enabled, a pressure test should be
 *        in-progress and thus, the "sensitivity" and "frequency"
 *        of state publishes, is increased.
 * @see publish_delta_test, publish_frequency_test
 */
class PressureSensorTest : public switch_::Switch, public Component {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PressureSensorTest(FSWM100 *fswm100);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Called when Home Assistant (or other service) sends a command to change the bool value
  void write_state(bool state) override;

 private:
  /**
   * @brief the parent component
   *
   */
  FSWM100 *fswm100_{nullptr};
};

}  // namespace fswm100
}  // namespace esphome
