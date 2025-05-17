#include "pressure_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace fswm100 {

static const char *TAG = "fswm100";

void PressureSensor::setup(ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain, ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "PressureSensor setup start.");
  multiplexer_ = multiplexer;
  gain_ = gain;
  sample_rate_ = sample_rate;
  resolution_ = resolution;
  ESP_LOGCONFIG(TAG, "PressureSensor setup complete.");
}


}  // namespace fswm100
}  // namespace esphome
