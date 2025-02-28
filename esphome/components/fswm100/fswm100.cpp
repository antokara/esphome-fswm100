#include "fswm100.h"
#include "esphome/core/log.h"

namespace esphome {
namespace fswm100 {

static const char *TAG = "fswm100";

void FSWM100::loop() {
  const uint32_t now = millis();
  if (now - this->last_transmission_ >= 1000) {
    this->last_transmission_ = now;
    this->pressure_sensor_->publish_state(1.23f);
    this->flow_sensor_->publish_state(2.34f);
    this->pulse_sensor_->publish_state(1);
  }
}
void FSWM100::dump_config() { ESP_LOGCONFIG(TAG, "FSWM100..."); }

}  // namespace fswm100
}  // namespace esphome
