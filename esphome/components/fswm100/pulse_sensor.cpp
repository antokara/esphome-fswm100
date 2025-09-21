#include "esphome/core/log.h"
#include "fswm100.h"
#include "pulse_sensor.h"

namespace esphome {
namespace fswm100 {

PulseSensor::PulseSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PulseSensor::setup(GPIOPin *pulse_sensor_gpio_pin, float rate_volume, float fault_flow_timeout_multiplier,
                        int inactivity_fault_counter_threshold) {
  ESP_LOGCONFIG(TAG, "PulseSensor setup start.");
  if (pulse_sensor_gpio_pin == nullptr) {
    ESP_LOGE(TAG, "PulseSensor pin not set!");
    return;
  }
  this->pin_ = pulse_sensor_gpio_pin;
  this->pin_->pin_mode(gpio::Flags::FLAG_INPUT);
  this->rate_volume_ = rate_volume;
  this->fault_flow_timeout_multiplier_ = fault_flow_timeout_multiplier;
  this->inactivity_fault_counter_threshold_ = inactivity_fault_counter_threshold;
  // initial state publish
  this->publish_state(false);
  ESP_LOGCONFIG(TAG, "PulseSensor setup complete.");
}

bool PulseSensor::has_fault() { return this->has_fault_; }

void PulseSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PulseSensor:");
  LOG_PIN("  Pin:", this->pin_);
  ESP_LOGCONFIG(TAG, "  Pin:", this->rate_volume_);
  ESP_LOGCONFIG(TAG, "  Fault flow timeout multiplier:", this->fault_flow_timeout_multiplier_);
  ESP_LOGCONFIG(TAG, "  Inactivity fault counter threshold:", this->inactivity_fault_counter_threshold_);
}

void PulseSensor::loop() {
  // get the state
  bool new_state = this->pin_->digital_read();
  // check if the state has changed or if this is the first time
  if (new_state != this->state) {
    this->publish_state(new_state);
    this->last_toggle_time_ = millis();

    // diagnostics: check if the pulse toggles too fast to be correct...
    if (!this->has_fault_ && this->fswm100_->get_flow_sensor_state() > this->fswm100_->get_flow_sensor_max_volume()) {
      ESP_LOGW(TAG, "'%s': pulse rate volume exceeded max flow volume: %.4f > %.4f", this->get_name().c_str(),
               this->fswm100_->get_flow_sensor_state(), this->fswm100_->get_flow_sensor_max_volume());
      this->fswm100_->add_fault(
          "Pulse rate volume exceeded max flow volume: " + std::to_string(this->fswm100_->get_flow_sensor_state()) +
          " > " + std::to_string(this->fswm100_->get_flow_sensor_max_volume()));
      this->has_fault_ = true;
    }
  }

  // diagnostics:check if the pulse sensor has not been toggled for a long time while there is active flow
  // (i.e. IR sensor appears to be active)
  // by comparing the time passed since the last pulse (for the current "flow active" period)
  // against the calculated pulse flow timeout
  if (!this->has_fault_ && this->fswm100_->get_flow_sensor_state() > 0) {
    // start by when the flow switched to active.
    // we must ensure that we take into consideration the time the flow sensor switched to active
    // because a pulse could have been toggled hours ago on a previous "flow active" period and
    // that would create false positive faults. Therefore, we need to check the time since the last pulse
    // for the current "flow active" period.
    uint32_t last_time = this->fswm100_->get_flow_sensor_last_switched_to_active_time();
    // but if the pulse sensor toggled more recently, use that time instead
    if (last_time < this->last_toggle_time_)
      last_time = this->last_toggle_time_;
    // calculate the maximum time it takes for the flow to time out (switch to no flow),
    // when there is no pulse activity...
    float flow_timeout_period = this->rate_volume_ / this->fswm100_->get_flow_sensor_min_volume() *
                                this->fswm100_->get_flow_sensor_rate_time() * 1000;
    if (millis() - last_time > (flow_timeout_period * this->fault_flow_timeout_multiplier_)) {
      // increment the inactivity fault counter
      this->inactivity_fault_counter_++;
      ESP_LOGW(TAG, "'%s': inactivity fault counter: %d", this->get_name().c_str(), this->inactivity_fault_counter_);

      // if the inactivity fault counter has reached the threshold, consider the sensor as faulty
      if (this->inactivity_fault_counter_ >= this->inactivity_fault_counter_threshold_) {
        ESP_LOGW(
            TAG,
            "'%s': No pulse has been detected for a long period, while the IR appears to be active. "
            "Either the IR is having problems (false positive), or the pulse sensor is not working (false negative). "
            "The minimum volume may also be set too high.",
            this->get_name().c_str());
        this->fswm100_->add_fault(
            "No pulse has been detected for a long period, while the IR appears to be active. "
            "Either the IR is having problems (false positive), or the pulse sensor is not working (false negative). "
            "The minimum volume may also be set too high.");
        this->has_fault_ = true;
      }
    } else {
      // reset the inactivity fault counter, as we have seen a pulse within the expected time
      this->inactivity_fault_counter_ = 0;
    }
  }
}

float PulseSensor::get_rate_volume() { return this->rate_volume_; }
uint32_t PulseSensor::get_last_toggle_time() { return this->last_toggle_time_; }

}  // namespace fswm100
}  // namespace esphome
