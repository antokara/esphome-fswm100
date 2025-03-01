#include "pulse_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace fswm100 {

static const char *const TAG = "pulse_sensor";

void PulseSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PulseSensor...");
  if (pin_ != nullptr) {
    pin_->pin_mode(gpio::Flags::FLAG_PULLUP);
  } else {
    ESP_LOGE(TAG, "Binary sensor pin not set!");
  }
}

void PulseSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "MyCustomBinarySensor:");
  LOG_PIN("  Pin:", this->pin_);
}

bool PulseSensor::get_state() {
  if (pin_ != nullptr) {
    return pin_->digital_read();
  }
  return false;  // Return false if pin is not set.
}

}  // namespace fswm100
}  // namespace esphome
