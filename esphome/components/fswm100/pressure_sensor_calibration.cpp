#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor_calibration.h"

namespace esphome {
namespace fswm100 {

// PressureSensorCalibration::PressureSensorCalibration(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensorCalibration::setup() {
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration setup start.");

  if (this->has_initial_value_) {
    ESP_LOGI(TAG, "  Publishing initial value from YAML: %.2f", this->initial_value_);
    this->publish_state(this->initial_value_);  // Publish initial state set via YAML
  } else {
    // If no initial_value in YAML, the number component will initialize to NaN or its lower bound.
    // You can choose to publish a different default here if needed.
    ESP_LOGI(TAG, "  No initial value set in YAML. Current state: %.2f (may be NaN or min_value)", this->state);
    // Example: publish min_value if no initial value is set and state is NaN
    // if (std::isnan(this->state)) {
    //   this->publish_state(this->traits.get_min_value());
    // }
  }

  ESP_LOGCONFIG(TAG, "PressureSensorCalibration setup complete.");
}

void PressureSensorCalibration::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensorCalibration:");
  if (this->has_initial_value_) {
    ESP_LOGCONFIG(TAG, "    Initial Value (from YAML): %.2f", this->initial_value_);
  } else {
    ESP_LOGCONFIG(TAG, "    Initial Value: Not set in YAML");
  }
}

// This method is called when Home Assistant or another service changes the number's value.
void PressureSensorCalibration::control(float value) {
  // float value_from_ha = *call.get_value();  // Get the value from the call object
  ESP_LOGI(TAG, "PressureSensorCalibration received control call from HA/frontend with value: %.2f", value);

  // // Here, you could add validation or interact with hardware.
  // // The Number base class already clamps the value to min/max traits before this call if mode is slider.
  // // For mode 'box', it's good practice to clamp it here too, or rely on publish_state's clamping.
  // // float clamped_value = value_from_ha;
  // // if (clamped_value < this->traits.get_min_value()) clamped_value = this->traits.get_min_value();
  // // if (clamped_value > this->traits.get_max_value()) clamped_value = this->traits.get_max_value();

  // this->publish_state(value_from_ha);  // This updates internal state & informs HA.
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
