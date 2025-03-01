#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace fswm100 {

class PulseSensor : public binary_sensor::BinarySensor {
 public:
  void setup();
  void dump_config();
  void set_pin(GPIOPin *pin) { pin_ = pin; }
  GPIOPin *get_pin() { return pin_; }
  bool get_state();

 private:
  GPIOPin *pin_ = nullptr;
};

}  // namespace fswm100
}  // namespace esphome
