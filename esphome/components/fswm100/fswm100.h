#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace fswm100 {

class FSWM100 : public Component {
  // void setup() override;
  void loop() override;
  // void update() override;
  void dump_config() override;
};

}  // namespace fswm100
}  // namespace esphome
