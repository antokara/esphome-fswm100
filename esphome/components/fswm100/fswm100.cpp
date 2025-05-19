#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "fswm100.h"

namespace esphome {
namespace fswm100 {

// Random 32bit value; If this changes existing restore preferences are invalidated
static const uint32_t RESTORE_STATE_VERSION = 0x112EA6ADUL;

// setters
void FSWM100::set_ads1115(ads1115::ADS1115Component *ads1115) { this->ads1115_ = ads1115; }
ads1115::ADS1115Component *FSWM100::get_ads1115() { return this->ads1115_; }
void FSWM100::set_flow_sensor(FlowSensor *flow_sensor) { flow_sensor_ = flow_sensor; }
void FSWM100::set_pulse_sensor(PulseSensor *pulse_sensor) { pulse_sensor_ = pulse_sensor; }
void FSWM100::set_pressure_sensor(PressureSensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }
void FSWM100::set_pressure_sensor_calibration(PressureSensorCalibration *pressure_sensor_calibration) {
  pressure_sensor_calibration_ = pressure_sensor_calibration;
}
void FSWM100::set_pressure_sensor_test(PressureSensorTest *pressure_sensor_test) {
  pressure_sensor_test_ = pressure_sensor_test;
}

// getters
float FSWM100::get_pressure_sensor_calibration_multiplier() { return this->pressure_sensor_calibration_->state; }
bool FSWM100::get_pressure_sensor_test_flag() { return this->pressure_sensor_test_->state; }

void FSWM100::load_state_() {
  this->pref_ = global_preferences->make_preference<State>(this->get_object_id_hash() ^ RESTORE_STATE_VERSION);
  State recovered{};
  if (this->pref_.load(&recovered)) {
    ESP_LOGCONFIG(TAG, "restored pressure_sensor_calibration_multiplier %.2f",
                  recovered.pressure_sensor_calibration_multiplier);
    ESP_LOGCONFIG(TAG, "restored pressure_sensor_test_flag %", recovered.pressure_sensor_test_flag);
  } else {
    ESP_LOGCONFIG(TAG, "unable to restore state. setting to defaults");
    recovered.pressure_sensor_calibration_multiplier = PRESSURE_SENSOR_DEFAULT_VALUE;
    recovered.pressure_sensor_test_flag = false;
  }
  // set the component properties using the restored state
  this->pressure_sensor_calibration_->publish_state(recovered.pressure_sensor_calibration_multiplier);
  this->pressure_sensor_test_->publish_state(recovered.pressure_sensor_test_flag);
}

void FSWM100::save_state_() {
  ESP_LOGD(TAG, "Saving state...");
  State state{};
  // initialize as zero to prevent random data on stack triggering erase
  memset(&state, 0, sizeof(State));
  // set the state usign the component properties
  state.pressure_sensor_calibration_multiplier = this->pressure_sensor_calibration_->state;
  state.pressure_sensor_test_flag = this->pressure_sensor_test_->state;
  this->pref_.save(&state);
  ESP_LOGD(TAG, "Saved state:");
  ESP_LOGD(TAG, " - pressure_sensor_calibration_multiplier %.2f", state.pressure_sensor_calibration_multiplier);
  ESP_LOGD(TAG, " - pressure_sensor_test_flag %", state.pressure_sensor_test_flag);
}

void FSWM100::save_state() {
  this->last_save_state_call_ = millis();
  this->save_state_debounced_ = true;
}

void FSWM100::save_state_pending_check_() {
  if (this->save_state_debounced_ && (millis() - this->last_save_state_call_) > SAVE_STATE_DEBOUNCE_FREQUENCY) {
    this->save_state_();
    this->save_state_debounced_ = false;
  }
}

void FSWM100::setup() {
  ESP_LOGCONFIG(TAG, "Setting up FSWM100 started...");
  // check ADS component
  if (!this->ads1115_) {
    ESP_LOGE(TAG, "ADS1115 shared component was not set. Cannot proceed.");
    this->mark_failed();  // Mark component as failed if prerequisite is missing
    return;
  }

  this->load_state_();
  // TODO: use these to calculate, etc.
  // this->flow_sensor_->get_unit_of_measurement();
  // this->pressure_sensor_->get_unit_of_measurement();
  ESP_LOGCONFIG(TAG, "Setting up FSWM100 completed...");
};

// TODO: refactor to add loop to each sensor and maybe let them access parent and do their thing to publish or not
void FSWM100::loop() {
  const uint32_t now = millis();
  save_state_pending_check_();

  if (now - this->last_transmission_ >= 15000) {
    this->last_transmission_ = now;

    // flow
    float new_flow_sensor_state = this->flow_sensor_->get_state();
    if (abs(this->flow_sensor_state - new_flow_sensor_state) > 0.01) {
      this->flow_sensor_state = new_flow_sensor_state;
      this->flow_sensor_->publish_state(new_flow_sensor_state);
    }

    // if we get flactuations in the values after the first sensor read in the loop,
    // we must ensure that the ADS1115 is not in "continuous mode". otherwise,
    // we need to introduce a delay to allow the ADS1115 to stabilize,
    // after switching multiplexer channels.
  }

  this->pressure_sensor_->loop();

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

/**
 * @brief ensure I2C and ADS1115 are initialized first
 */
float FSWM100::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace fswm100
}  // namespace esphome
