#pragma once

#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

/**
 * @brief the default value for the pressure sensor calibration multiplier
 *        which is 1.0 (i.e. no calibration)
 */
#define PRESSURE_SENSOR_CALIBRATION_DEFAULT_VALUE 1.0f

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
class FSWM100;

/**
 * @brief a calibration multiplier for the pressure sensor,
 *        to account for any offset correction.
 */
class PressureSensorCalibration : public number::Number, public Component {
 public:
  /**
   * @param fswm100 the parent component class
   */
  PressureSensorCalibration(FSWM100 *fswm100);

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
