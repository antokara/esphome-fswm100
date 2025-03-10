#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace fswm100 {

class PulseSensor : public binary_sensor::BinarySensor {
 public:
  void setup(GPIOPin *pulse_sensor_gpio_pin);
  void dump_config();
  bool get_state();

 private:
  GPIOPin *pin_ = nullptr;
};

}  // namespace fswm100
}  // namespace esphome
