#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor_calibration.h"

namespace esphome {
namespace fswm100 {

// PressureSensorCalibration::PressureSensorCalibration(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensorCalibration::setup() {
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration setup start.");
  // ensure the current value is a number and within the min/max range
  if (std::isnan(this->state) || this->state < this->traits.get_min_value() ||
      this->state > this->traits.get_max_value()) {
    ESP_LOGW(TAG, "  Initial value is invalid, setting to default: %.2f", PRESSURE_SENSOR_DEFAULT_VALUE);
    this->publish_state(PRESSURE_SENSOR_DEFAULT_VALUE);
  }
  ESP_LOGI(TAG, "  Publishing initial value from YAML: %.2f", this->state);
  this->publish_state(this->state);  // Publish initial state set via YAML
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration setup complete.");
}

void PressureSensorCalibration::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration:");
  ESP_LOGCONFIG(TAG, "    Value: %.2f", this->state);
}

// This method is called when Home Assistant or another service changes the number's value.
void PressureSensorCalibration::control(float value) {
  ESP_LOGI(TAG, "PressureSensorCalibration received control call from HA/frontend with value: %.2f", value);
  this->publish_state(value);  // This updates internal state & informs HA.
}

// Optional: If you had a set_value_from_cpp method:
// void MyCustomDeviceNumber::set_value_from_cpp(float value) {
//   ESP_LOGI(TAG, "Number '%s' set_value_from_cpp called with: %.2f", this->get_name_pretty().c_str(), value);
//   // Clamp value to min/max traits
//   if (value < this->traits.get_min_value()) value = this->traits.get_min_value();
//   if (value > this->traits.get_max_value()) value = this->traits.get_max_value();
//   this->publish_state(value);
// }

}  // namespace fswm100
}  // namespace esphome
