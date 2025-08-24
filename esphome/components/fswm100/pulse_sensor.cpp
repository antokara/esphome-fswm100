#include "esphome/core/log.h"
#include "fswm100.h"
#include "pulse_sensor.h"

namespace esphome {
namespace fswm100 {

PulseSensor::PulseSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PulseSensor::setup(GPIOPin *pulse_sensor_gpio_pin, float rate_volume) {
  ESP_LOGCONFIG(TAG, "PulseSensor setup start.");
  if (pulse_sensor_gpio_pin == nullptr) {
    ESP_LOGE(TAG, "PulseSensor pin not set!");
    return;
  }
  this->pin_ = pulse_sensor_gpio_pin;
  this->pin_->pin_mode(gpio::Flags::FLAG_INPUT);
  this->rate_volume_ = rate_volume;
  // initial state publish
  this->publish_state(false);
  ESP_LOGCONFIG(TAG, "PulseSensor setup complete.");
}

bool PulseSensor::has_fault() { return this->has_fault_; }

void PulseSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PulseSensor:");
  LOG_PIN("  Pin:", this->pin_);
  ESP_LOGCONFIG(TAG, "  Pin:", this->rate_volume_);
}

void PulseSensor::loop() {
  // get the state
  bool new_state = this->pin_->digital_read();
  // check if the state has changed or if this is the first time
  if (new_state != this->state) {
    this->publish_state(new_state);
    this->last_toggle_time_ = millis();

    // check if the pulse toggles too fast to be correct...
    if (!this->has_fault_ && this->fswm100_->get_flow_sensor_state() > this->fswm100_->get_flow_sensor_max_volume()) {
      ESP_LOGW(TAG, "'%s': pulse rate volume exceeded max flow volume: %.4f > %.4f", this->get_name().c_str(),
               this->fswm100_->get_flow_sensor_state(), this->fswm100_->get_flow_sensor_max_volume());
      this->has_fault_ = true;
    }
  }
}

float PulseSensor::get_rate_volume() { return this->rate_volume_; }
uint32_t PulseSensor::get_last_toggle_time() { return this->last_toggle_time_; }

}  // namespace fswm100
}  // namespace esphome
