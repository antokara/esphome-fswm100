#include "pulse_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace fswm100 {

static const char *const TAG = "pulse_sensor";

void PulseSensor::setup(GPIOPin *pulse_sensor_gpio_pin) {
  ESP_LOGCONFIG(TAG, "Setting up PulseSensor...");
  pin_ = pulse_sensor_gpio_pin;
  pin_->pin_mode(gpio::Flags::FLAG_INPUT);
}

void PulseSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "MyCustomBinarySensor:");
  LOG_PIN("  Pin:", this->pin_);
}

bool PulseSensor::get_state() {
  if (pin_ != nullptr) {
    return pin_->digital_read();
  }
  ESP_LOGE(TAG, "Binary sensor pin not set!");
  return false;  // Return false if pin is not set.
}

}  // namespace fswm100
}  // namespace esphome
