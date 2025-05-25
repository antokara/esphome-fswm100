#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor_test.h"

namespace esphome {
namespace fswm100 {

PressureSensorTest::PressureSensorTest(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensorTest::setup() {
  ESP_LOGCONFIG(TAG, "PressureSensorTest setup start.");
  // initial state publish
  this->publish_state(this->state);
  ESP_LOGCONFIG(TAG, "PressureSensorTest setup complete.");
}

void PressureSensorTest::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensorTest:");
  ESP_LOGCONFIG(TAG, "    Value: %", this->state);
}

void PressureSensorTest::write_state(bool state) {
  ESP_LOGI(TAG, "PressureSensorTest received control with value: %", state);

  // This updates the "state" property of this class and
  // the Home Assistant entity's state, to the new value.
  this->publish_state(state);
}

}  // namespace fswm100
}  // namespace esphome
