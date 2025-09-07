#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "fswm100.h"

namespace esphome {
namespace fswm100 {

// Random 32bit value; If this changes existing restore preferences are invalidated
static const uint32_t RESTORE_STATE_VERSION = 0x113EA6ADUL;

// setters
void FSWM100::set_ads1115(ads1115::ADS1115Component *ads1115) { this->ads1115_ = ads1115; }
ads1115::ADS1115Component *FSWM100::get_ads1115() { return this->ads1115_; }
void FSWM100::set_flow_sensor(FlowSensor *flow_sensor) { this->flow_sensor_ = flow_sensor; }
void FSWM100::set_pulse_sensor(PulseSensor *pulse_sensor) { this->pulse_sensor_ = pulse_sensor; }
void FSWM100::set_pressure_sensor(PressureSensor *pressure_sensor) { this->pressure_sensor_ = pressure_sensor; }
void FSWM100::set_pressure_test_sensor(PressureTestSensor *pressure_test_sensor) {
  this->pressure_test_sensor_ = pressure_test_sensor;
}
void FSWM100::set_pressure_sensor_calibration(PressureSensorCalibration *pressure_sensor_calibration) {
  this->pressure_sensor_calibration_ = pressure_sensor_calibration;
}
void FSWM100::set_flow_sensor_min_duration(FlowSensorMinDuration *flow_sensor_min_duration) {
  this->flow_sensor_min_duration_ = flow_sensor_min_duration;
}
void FSWM100::set_pressure_sensor_test(PressureSensorTest *pressure_sensor_test) {
  this->pressure_sensor_test_ = pressure_sensor_test;
}
void FSWM100::set_flow_problem_sensor(FlowSensorProblemSensor *flow_sensor_problem_sensor) {
  this->flow_sensor_problem_sensor_ = flow_sensor_problem_sensor;
}
void FSWM100::set_pulse_problem_sensor(PulseSensorProblemSensor *pulse_sensor_problem_sensor) {
  this->pulse_sensor_problem_sensor_ = pulse_sensor_problem_sensor;
}
void FSWM100::set_pressure_problem_sensor(PressureSensorProblemSensor *pressure_sensor_problem_sensor) {
  this->pressure_sensor_problem_sensor_ = pressure_sensor_problem_sensor;
}
void FSWM100::pressure_test_sensor_process(float pressure) { this->pressure_test_sensor_->process(pressure); }
void FSWM100::set_status_led(GPIOPin *status_led_red_gpio_pin, GPIOPin *status_led_green_gpio_pin,
                             GPIOPin *status_led_blue_gpio_pin) {
  this->status_led_red_gpio_pin_ = status_led_red_gpio_pin;
  this->status_led_green_gpio_pin_ = status_led_green_gpio_pin;
  this->status_led_blue_gpio_pin_ = status_led_blue_gpio_pin;
  this->status_led_red_gpio_pin_->pin_mode(gpio::Flags::FLAG_OUTPUT);
  this->status_led_green_gpio_pin_->pin_mode(gpio::Flags::FLAG_OUTPUT);
  this->status_led_blue_gpio_pin_->pin_mode(gpio::Flags::FLAG_OUTPUT);
  this->set_status_led(StatusLEDColor::WHITE);
}
void FSWM100::set_status_led(StatusLEDColor color) {
  if (this->status_led_color_ == color) {
    return;
  }

  this->status_led_color_ = color;
  switch (color) {
    case StatusLEDColor::RED:
      this->status_led_red_gpio_pin_->digital_write(true);
      this->status_led_green_gpio_pin_->digital_write(false);
      this->status_led_blue_gpio_pin_->digital_write(false);
      break;
    case StatusLEDColor::GREEN:
      this->status_led_red_gpio_pin_->digital_write(false);
      this->status_led_green_gpio_pin_->digital_write(true);
      this->status_led_blue_gpio_pin_->digital_write(false);
      break;
    case StatusLEDColor::BLUE:
      this->status_led_red_gpio_pin_->digital_write(false);
      this->status_led_green_gpio_pin_->digital_write(false);
      this->status_led_blue_gpio_pin_->digital_write(true);
      break;
    case StatusLEDColor::OFF:
      this->status_led_red_gpio_pin_->digital_write(false);
      this->status_led_green_gpio_pin_->digital_write(false);
      this->status_led_blue_gpio_pin_->digital_write(false);
      break;
    case StatusLEDColor::WHITE:
      this->status_led_red_gpio_pin_->digital_write(true);
      this->status_led_green_gpio_pin_->digital_write(true);
      this->status_led_blue_gpio_pin_->digital_write(true);
      break;
    case StatusLEDColor::YELLOW:
      this->status_led_red_gpio_pin_->digital_write(true);
      this->status_led_green_gpio_pin_->digital_write(true);
      this->status_led_blue_gpio_pin_->digital_write(false);
      break;
    case StatusLEDColor::CYAN:
      this->status_led_red_gpio_pin_->digital_write(false);
      this->status_led_green_gpio_pin_->digital_write(true);
      this->status_led_blue_gpio_pin_->digital_write(true);
      break;
    case StatusLEDColor::MAGENTA:
      this->status_led_red_gpio_pin_->digital_write(true);
      this->status_led_green_gpio_pin_->digital_write(false);
      this->status_led_blue_gpio_pin_->digital_write(true);
      break;
  }
}

// getters
float FSWM100::get_pressure_sensor_calibration_multiplier() { return this->pressure_sensor_calibration_->state; }
float FSWM100::get_flow_sensor_min_duration() { return this->flow_sensor_min_duration_->state * 1000; }
float FSWM100::get_flow_sensor_state() { return this->flow_sensor_->state; }
float FSWM100::get_flow_sensor_min_volume() { return this->flow_sensor_->get_min_volume(); }
float FSWM100::get_flow_sensor_max_volume() { return this->flow_sensor_->get_max_volume(); }
float FSWM100::get_flow_sensor_rate_time() { return this->flow_sensor_->get_rate_time(); }
uint32_t FSWM100::get_flow_sensor_last_switched_to_active_time() {
  return this->flow_sensor_->get_last_switched_to_active_time();
}
uint32_t FSWM100::get_flow_sensor_last_switched_to_inactive_time() {
  return this->flow_sensor_->get_last_switched_to_inactive_time();
}
bool FSWM100::get_pressure_sensor_test_flag() { return this->pressure_sensor_test_->state; }
bool FSWM100::get_pulse_sensor() { return this->pulse_sensor_->state; }
float FSWM100::get_pulse_rate_volume() { return this->pulse_sensor_->get_rate_volume(); }

void FSWM100::load_state_() {
  this->pref_ = global_preferences->make_preference<State>(this->get_object_id_hash() ^ RESTORE_STATE_VERSION);
  State recovered{};
  if (this->pref_.load(&recovered)) {
    ESP_LOGCONFIG(TAG, "restored pressure_sensor_calibration_multiplier %.2f",
                  recovered.pressure_sensor_calibration_multiplier);
    ESP_LOGCONFIG(TAG, "restored flow_sensor_min_duration %.2f", recovered.flow_sensor_min_duration);
  } else {
    ESP_LOGCONFIG(TAG, "unable to restore state. setting to defaults");
    recovered.pressure_sensor_calibration_multiplier = PRESSURE_SENSOR_CALIBRATION_DEFAULT_VALUE;
    recovered.flow_sensor_min_duration = FLOW_SENSOR_MIN_DURATION_DEFAULT_VALUE;
  }
  // set the component properties using the restored state
  this->pressure_sensor_calibration_->publish_state(recovered.pressure_sensor_calibration_multiplier);
  this->flow_sensor_min_duration_->publish_state(recovered.flow_sensor_min_duration);
}

void FSWM100::save_state_() {
  ESP_LOGD(TAG, "Saving state...");
  State state{};
  // initialize as zero to prevent random data on stack triggering erase
  memset(&state, 0, sizeof(State));
  // set the state usign the component properties
  state.pressure_sensor_calibration_multiplier = this->pressure_sensor_calibration_->state;
  state.flow_sensor_min_duration = this->flow_sensor_min_duration_->state;
  this->pref_.save(&state);
  ESP_LOGD(TAG, "Saved state:");
  ESP_LOGD(TAG, " - pressure_sensor_calibration_multiplier %.2f", state.pressure_sensor_calibration_multiplier);
  ESP_LOGD(TAG, " - flow_sensor_min_duration %.2f", state.flow_sensor_min_duration);
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

void FSWM100::log_faults() {
  if (millis() - this->last_fault_log_time_ > LOG_FAULTS_FREQUENCY && this->faults.size() > 0) {
    this->last_fault_log_time_ = millis();
    for (const std::string &fault : this->faults) {
      ESP_LOGE(TAG, fault.c_str());
    }
  }
}

void FSWM100::add_fault(const std::string &fault) {
  if (std::find(this->faults.begin(), this->faults.end(), fault) == this->faults.end()) {
    this->faults.push_back(fault);
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
  ESP_LOGCONFIG(TAG, "Setting up FSWM100 completed...");
};

// TODO: refactor to add loop to each sensor and maybe let them access parent and do their thing to publish or not
void FSWM100::loop() {
  save_state_pending_check_();

  this->pulse_sensor_->loop();
  this->flow_sensor_->loop();
  this->pressure_sensor_->loop();
  this->flow_sensor_problem_sensor_->loop();

  /**
   * diagnostic: checks for faults.
   *
   * we allow for multiple faults to be detected and reported combined.
   * this is true because we may not be able to isolate the fault to a single sensor and/or
   * multiple sensors may be affected by the same issue.
   */
  if (this->pulse_sensor_->has_fault() && this->pulse_sensor_problem_sensor_->state != true) {
    ESP_LOGE(TAG, "Pulse Sensor has an internal fault!");
    this->pulse_sensor_problem_sensor_->publish_state(true);
    this->has_fault_ = true;
  }
  if (this->flow_sensor_->has_fault() && this->flow_sensor_problem_sensor_->state != true) {
    ESP_LOGE(TAG, "Flow Sensor has an internal fault!");
    this->flow_sensor_problem_sensor_->publish_state(true);
    this->has_fault_ = true;
  }
  if (this->pressure_sensor_->has_fault() && this->pressure_sensor_problem_sensor_->state != true) {
    ESP_LOGE(TAG, "Pressure Sensor has an internal fault!");
    this->pressure_sensor_problem_sensor_->publish_state(true);
    this->has_fault_ = true;
  }

  /**
   * diagnostics: check to set the status LED states/colors.
   * the order here is important, as it determines which state/color
   * takes precedence over the others.
   *
   * e.g. if the flow sensor is active, it will override the pressure sensor test state,
   * and the status LED will be set to blue no matter what else follows. But if there
   * is a fault, if will override any other state and the status LED will be set to red.
   */
  if (this->has_fault_) {
    // fault detected
    this->set_status_led(StatusLEDColor::RED);
  } else if (this->flow_sensor_->state > 0) {
    // active flow
    this->set_status_led(StatusLEDColor::BLUE);
  } else if (this->pressure_sensor_test_->state == true) {
    // active pressure test
    this->set_status_led(StatusLEDColor::MAGENTA);
  } else {
    // idle and without issues
    this->set_status_led(StatusLEDColor::GREEN);
  }

  this->log_faults();

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

/**
 * @brief ensure I2C and ADS1115 are initialized first
 */
float FSWM100::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace fswm100
}  // namespace esphome
