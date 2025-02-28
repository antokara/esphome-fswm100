#include "fswm100.h"
#include "esphome/core/log.h"

namespace esphome {
namespace fswm100 {

static const char *TAG = "fswm100";

// void FSWM100::setup() {}

void FSWM100::loop() {}

// void FSWM100::update() { this->publish_state(1.2345); }

void FSWM100::dump_config() { ESP_LOGCONFIG(TAG, "FSWM100..."); }

}  // namespace fswm100
}  // namespace esphome
