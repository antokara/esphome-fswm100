#include "fswm100.h"
#include "esphome/core/log.h"

namespace esphome {
namespace fswm100 {

static const char *TAG = "fswm100";

void FSWM100::setup() { pinMode(this->pulse_sensor_gpio_pin_, INPUT); }

void FSWM100::loop() {
  const uint32_t now = millis();
  bool pulse_sensor_value = digitalRead(this->pulse_sensor_gpio_pin_);

  if (now - this->last_transmission_ >= 5000) {
    this->last_transmission_ = now;
    this->pressure_sensor_->publish_state(1.23f);
    this->flow_sensor_->publish_state(2.34f);
    this->pulse_sensor_->publish_state(pulse_sensor_value);
  }

  // this->status_set_warning();
  //  this->status_clear_warning();
  // ESP_LOGE(TAG, "error log!");
  // ESP_LOGE - error
  // ESP_LOGW - warning
  // ESP_LOGI - info
  // ESP_LOGD - debug
  // ESP_LOGV - verbose
  // ESP_LOGVV - very verbose
  // ESP_LOGCONFIG - config log
  //   #if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
  //   {
  //     std::string s = format_hex_pretty(this->raw_data_, sizeof(this->raw_data_));
  //     ESP_LOGVV(TAG, "Raw data: %s", s.c_str());
  //   }
  // #endif
}
void FSWM100::dump_config() { ESP_LOGCONFIG(TAG, "FSWM100..."); }
float FSWM100::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace fswm100
}  // namespace esphome
