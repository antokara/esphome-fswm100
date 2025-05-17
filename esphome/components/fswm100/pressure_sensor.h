#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/ads1115/ads1115.h"

namespace esphome {
namespace fswm100 {

class PressureSensor : public sensor::Sensor {
 public:
  void setup(ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain, ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution); 
//   void dump_config();
//   bool get_state();

 private:
   /**
   * @brief ADS1115 multiplexer
   */
  ads1115::ADS1115Multiplexer multiplexer_;
  /**
   * @brief ADS1115 gain
   */
  ads1115::ADS1115Gain gain_;
  /**
   * @brief ADS1115 sample rate
   */
  ads1115::ADS1115Samplerate sample_rate_;
  /**
   * @brief ADS1115 resolution
   */
  ads1115::ADS1115Resolution resolution_;
};

}  // namespace fswm100
}  // namespace esphome
