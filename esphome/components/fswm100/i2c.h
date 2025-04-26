#pragma once

#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace fswm100 {

class I2C : public i2c::I2CDevice {
 public:
  void setup();

 private:
};

}  // namespace fswm100
}  // namespace esphome
