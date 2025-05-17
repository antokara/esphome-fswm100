#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor.h"

namespace esphome {
namespace fswm100 {

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
  float voltage = abs(this->fswm100_->get_ads1115()->request_measurement(this->multiplexer_, this->gain_,
                                                                         this->resolution_, this->sample_rate_));

  if (std::isnan(voltage)) {
    ESP_LOGW(TAG, "Failed to read from ADS1115 channel for '%s'. Result was NaN.", this->get_name().c_str());
    return -1;
  }

  ESP_LOGD(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
           static_cast<int>(this->multiplexer_), voltage);

  return voltage;
}

}  // namespace fswm100
}  // namespace esphome
