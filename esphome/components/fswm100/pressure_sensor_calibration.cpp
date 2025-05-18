#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor_calibration.h"

namespace esphome {
namespace fswm100 {

PressureSensorCalibration::PressureSensorCalibration(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensorCalibration::setup() {
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration setup start.");
  // ensure the current value is a number and within the min/max range
  if (std::isnan(this->state) || this->state < this->traits.get_min_value() ||
      this->state > this->traits.get_max_value()) {
    ESP_LOGW(TAG, "  Initial value is invalid, setting to default: %.2f", PRESSURE_SENSOR_DEFAULT_VALUE);
    this->publish_state(PRESSURE_SENSOR_DEFAULT_VALUE);
  }
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration setup complete.");
}

void PressureSensorCalibration::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration:");
  ESP_LOGCONFIG(TAG, "    Value: %.2f", this->state);
  ESP_LOGCONFIG(TAG, "    Min. Value: %.2f", this->traits.get_min_value());
  ESP_LOGCONFIG(TAG, "    Max. Value: %.2f", this->traits.get_max_value());
  ESP_LOGCONFIG(TAG, "    Step: %.2f", this->traits.get_step());
}

// This method is called when Home Assistant or another service changes the number's value.
void PressureSensorCalibration::control(float value) {
  ESP_LOGI(TAG, "PressureSensorCalibration received control with value: %.2f", value);

  // This updates the "state" property of this class and
  // the Home Assistant entity's state, to the new value.
  this->publish_state(value);

  // save the new value
  this->fswm100_->save_state();
}

}  // namespace fswm100
}  // namespace esphome
