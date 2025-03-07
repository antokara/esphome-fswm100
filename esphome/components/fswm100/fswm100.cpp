#include "fswm100.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace fswm100 {

static const char *TAG = "fswm100";

// Random 32bit value; If this changes existing restore preferences are invalidated
static const uint32_t RESTORE_STATE_VERSION = 0x848EA6ADUL;

void FSWM100::load_state_() {
  uint32_t myhash = this->get_object_id_hash() ^ RESTORE_STATE_VERSION;
  this->pref_ = global_preferences->make_preference<State>('test' ^ RESTORE_STATE_VERSION);
  State recovered{};
  if (this->pref_.load(&recovered))
    ESP_LOGCONFIG(TAG, "restored testValue", recovered.testValue);
  else
    ESP_LOGCONFIG(TAG, "unabled to restore state");
  this->save_state_();
}

void FSWM100::save_state_() {
  State state{};
  // initialize as zero to prevent random data on stack triggering erase
  memset(&state, 0, sizeof(State));
  state.testValue = 12.34;
  this->pref_.save(&state);
  ESP_LOGCONFIG(TAG, "saved state");
}

void FSWM100::setup() {
  ESP_LOGCONFIG(TAG, "Setting up FSWM100...");
  this->load_state_();
};

void FSWM100::loop() {
  const uint32_t now = millis();

  if (now - this->last_transmission_ >= 15000) {
    this->last_transmission_ = now;
    this->pressure_sensor_->publish_state(1.23f);
    this->flow_sensor_->publish_state(2.34f);
  }

  // check the pulse sensor
  if (this->pulse_sensor_->get_state()) {
    // when the pulse sensor is in active state
    if (!this->pulse_sensor_active && abs(long(now - this->pulse_sensor_active_time_)) > PULSE_DEBOUNCE_FREQUENCY) {
      // and it just turned active
      this->pulse_sensor_active = true;
      // update the time it was last active
      this->pulse_sensor_active_time_ = now;
      this->pulse_sensor_->publish_state(true);
    }
  } else if (this->pulse_sensor_active) {
    // it just turned inactive
    this->pulse_sensor_active = false;
    this->pulse_sensor_->publish_state(false);
  } else if (!this->pulse_sensor_first_transmission) {
    // sensor is inactive but this is the first transmission
    // to avoid having unknown state...
    this->pulse_sensor_first_transmission = true;
    this->pulse_sensor_->publish_state(false);
  }

  //   // this->status_set_warning();
  //   //  this->status_clear_warning();
  //   // ESP_LOGE(TAG, "error log!");
  //   // ESP_LOGE - error
  //   // ESP_LOGW - warning
  //   // ESP_LOGI - info
  //   // ESP_LOGD - debug
  //   // ESP_LOGV - verbose
  //   // ESP_LOGVV - very verbose
  //   // ESP_LOGCONFIG - config log
  //   //   #if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
  //   //   {
  //   //     std::string s = format_hex_pretty(this->raw_data_, sizeof(this->raw_data_));
  //   //     ESP_LOGVV(TAG, "Raw data: %s", s.c_str());
  //   //   }
  //   // #endif
}
void FSWM100::dump_config() { ESP_LOGCONFIG(TAG, "FSWM100..."); }
float FSWM100::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace fswm100
}  // namespace esphome
