#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor.h"

namespace esphome {
namespace fswm100 {

static const char *TAG = "fswm100";

PressureSensor::PressureSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensor::setup(ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
                           ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "PressureSensor setup start.");

  multiplexer_ = multiplexer;
  gain_ = gain;
  sample_rate_ = sample_rate;
  resolution_ = resolution;
  ESP_LOGCONFIG(TAG, "PressureSensor setup complete.");
}

void PressureSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensor:");
  ESP_LOGCONFIG(TAG, "  multiplexer:", this->multiplexer_);
  ESP_LOGCONFIG(TAG, "  gain:", this->gain_);
  ESP_LOGCONFIG(TAG, "  sample rate:", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  resolution:", this->resolution_);
}

float PressureSensor::get_state() {
  // uint32_t now = millis();
  // if (now - this->last_internal_scheduled_update_ >= this->internal_schedule_interval_) {
  //   ESP_LOGD(TAG, "'%s': Internal schedule triggered in loop().", this->get_name().c_str());
  //   this->process_and_publish_sensor_value();
  //   this->last_internal_scheduled_update_ = now;
  // }
  return 12.78;
}

// void PressureSensor::process_and_publish_sensor_value() {
//   // float voltage = this->fswm100_->get_ads1115()->get_value(this->channel_);

//   // if (std::isnan(voltage)) {
//   //   ESP_LOGW(TAG, "Failed to read from ADS1115 channel for '%s'.", this->get_name().c_str());
//   //   this->publish_state(NAN);
//   //   return;
//   // }

//   // ESP_LOGD(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
//   //          static_cast<int>(this->channel_), voltage);

//   // float processed_value = voltage;
//   // if (this->my_custom_parameter_ == "scale_factor_1.5") {
//   //   processed_value = (voltage * 1.5f) + 0.1f;
//   // } else if (this->my_custom_parameter_ == "invert_and_add_10") {
//   //   processed_value = (voltage * -1.0f) + 10.0f;
//   // } else {
//   //   processed_value = voltage + 0.05f;
//   // }
//   // ESP_LOGD(TAG, "'%s': Processed value after custom operation: %.4f", this->get_name().c_str(), processed_value);

//   // this->publish_state(processed_value);
// }

}  // namespace fswm100
}  // namespace esphome
