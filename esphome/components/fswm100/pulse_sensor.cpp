#include "pulse_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace fswm100 {

static const char *TAG = "fswm100";

void PulseSensor::setup(GPIOPin *pulse_sensor_gpio_pin) {
  ESP_LOGCONFIG(TAG, "PulseSensor setup start.");
  pin_ = pulse_sensor_gpio_pin;
  pin_->pin_mode(gpio::Flags::FLAG_INPUT);
  ESP_LOGCONFIG(TAG, "PulseSensor setup complete.");
}

void PulseSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PulseSensor:");
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
