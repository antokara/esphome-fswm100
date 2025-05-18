#pragma once

#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

#define PRESSURE_SENSOR_DEFAULT_VALUE 1.0f

namespace esphome {
namespace fswm100 {

/**
 * @brief forward declaration class, to avoid circular includes.
 *        the .cpp implementation file must include the actual class.h though.
 */
// class FSWM100;

class PressureSensorCalibration : public number::Number, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Called when Home Assistant (or other service) sends a command to change the number value
  void control(float value) override;

  // Optional: Method to allow C++ code to change the number value more directly
  // void set_value_from_cpp(float value);

  // If you were linking this to a "hub" (MyCustomDeviceMain instance):
  // void set_hub(MyCustomDeviceMain *hub) { this->hub_ = hub; }

 protected:
  // MyCustomDeviceMain *hub_{nullptr}; // Example if linked to a hub
};

}  // namespace fswm100
}  // namespace esphome
