#include "esphome/core/log.h"
#include "fswm100.h"
#include "pulse_sensor.h"

namespace esphome {
namespace fswm100 {

PulseSensor::PulseSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PulseSensor::setup(GPIOPin *pulse_sensor_gpio_pin) {
  ESP_LOGCONFIG(TAG, "PulseSensor setup start.");
  if (pulse_sensor_gpio_pin == nullptr) {
    ESP_LOGE(TAG, "PulseSensor pin not set!");
    return;
  }
  pin_ = pulse_sensor_gpio_pin;
  pin_->pin_mode(gpio::Flags::FLAG_INPUT);
  this->publish_state(false);
  ESP_LOGCONFIG(TAG, "PulseSensor setup complete.");
}

void PulseSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PulseSensor:");
  LOG_PIN("  Pin:", this->pin_);
}

bool PulseSensor::get_state() { return pin_->digital_read(); }

void PulseSensor::loop() {
  // get the state
  bool new_state = this->get_state();
  // check if the state has changed or if this is the first time
  if (new_state != this->last_publish_state_) {
    this->last_publish_state_ = new_state;
    this->publish_state(new_state);
  }
}

}  // namespace fswm100
}  // namespace esphome
