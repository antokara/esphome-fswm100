#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/entity_base.h"
#include "esphome/components/ads1115/ads1115.h"
#include "pulse_sensor.h"
#include "pressure_sensor.h"
#include "pressure_test_sensor.h"
#include "pressure_sensor_calibration.h"
#include "pressure_sensor_test.h"
#include "flow_sensor.h"
#include "flow_ir_sensor.h"
#include "flow_sensor_min_duration.h"
#include "flow_sensor_problem_sensor.h"
#include "flow_ir_sensor_problem_sensor.h"
#include "pulse_sensor_problem_sensor.h"
#include "pressure_sensor_problem_sensor.h"

/**
 * @brief the milliseconds that the component will wait
 * before actually saving the state, after each call to save_state().
 */
#define SAVE_STATE_DEBOUNCE_FREQUENCY 15000

/**
 * @brief the frequency in milliseconds to log faults,
 * if any, to avoid spamming the logs.
 */
#define LOG_FAULTS_FREQUENCY 60000

/**
 * @brief the state that can be saved/loaded from Flash Memory
 * which perists device restarts
 */
struct State {
  float pressure_sensor_calibration_multiplier;
  float flow_sensor_min_duration;
} __attribute__((packed));

namespace esphome {
namespace fswm100 {

enum class StatusLEDColor { RED, GREEN, BLUE, OFF, WHITE, YELLOW, CYAN, MAGENTA };

/**
 * @brief the log tag for this component
 */
static const char *TAG = "fswm100";

class FSWM100 : public Component, public EntityBase {
 public:
  /**
   * @brief Setter for the shared ADS1115 component
   */
  void set_ads1115(ads1115::ADS1115Component *ads1115);

  /**
   * @brief getter for the shared ADS1115 component
   */
  ads1115::ADS1115Component *get_ads1115();

  /**
   * @brief Sets the flow sensor object.
   * @see __init__.py
   *
   * @param flow_sensor
   */
  void set_flow_sensor(FlowSensor *flow_sensor);

  /**
   * @brief Sets the flow ir sensor object.
   * @see __init__.py
   *
   * @param flow_ir_sensor
   */
  void set_flow_ir_sensor(FlowIrSensor *flow_ir_sensor);

  /**
   * @brief Set the pulse sensor object
   * @see __init__.py
   *
   * @param pulse_sensor
   */
  void set_pulse_sensor(PulseSensor *pulse_sensor);

  /**
   * @brief Set the pressure sensor object
   * @see __init__.py
   *
   * @param pressure_sensor
   */
  void set_pressure_sensor(PressureSensor *pressure_sensor);

  /**
   * @brief Set the pressure test sensor object
   * @see __init__.py
   *
   * @param pressure_test_sensor
   */
  void set_pressure_test_sensor(PressureTestSensor *pressure_test_sensor);

  /**
   * @brief Set the pressure sensor calibration object
   *  @see __init__.py
   *
   * @param pressure_sensor_calibration
   */
  void set_pressure_sensor_calibration(PressureSensorCalibration *pressure_sensor_calibration);

  /**
   * @brief Set the flow sensor minimum duration object
   *  @see __init__.py
   *
   * @param flow_sensor_min_duration
   */
  void set_flow_sensor_min_duration(FlowSensorMinDuration *flow_sensor_min_duration);

  /**
   * @brief Set the pressure sensor calibration object
   *  @see __init__.py
   *
   * @param pressure_sensor_test
   */
  void set_pressure_sensor_test(PressureSensorTest *pressure_sensor_test);

  /**
   * @brief sets the flow sensor problem sensor object
   *  @see __init__.py
   *
   * @param flow_sensor_problem_sensor
   */
  void set_flow_problem_sensor(FlowSensorProblemSensor *flow_sensor_problem_sensor);

  /**
   * @brief sets the flow IR sensor problem sensor object
   *  @see __init__.py
   *
   * @param flow_ir_sensor_problem_sensor
   */
  void set_flow_ir_problem_sensor(FlowIrSensorProblemSensor *flow_ir_sensor_problem_sensor);

  /**
   * @brief sets the pulse sensor problem sensor object
   *  @see __init__.py
   *
   * @param pulse_sensor_problem_sensor
   */
  void set_pulse_problem_sensor(PulseSensorProblemSensor *pulse_sensor_problem_sensor);

  /**
   * @brief sets the pressure sensor problem sensor object
   *  @see __init__.py
   *
   * @param pressure_sensor_problem_sensor
   */
  void set_pressure_problem_sensor(PressureSensorProblemSensor *pressure_sensor_problem_sensor);

  /**
   * @brief returns the pressure sensor calibration multiplier
   */
  float get_pressure_sensor_calibration_multiplier();

  /**
   * @brief returns the flow sensor minimum duration in milliseconds
   */
  float get_flow_sensor_min_duration();

  /**
   * @brief returns the flow sensor state (e.g. flow rate in gpm)
   */
  float get_flow_sensor_state();

  /**
   * @brief returns the min flow volume the meter is supposed to detect.
   */
  float get_flow_sensor_min_volume();

  /**
   * @brief returns the max flow volume the meter is supposed to detect.
   */
  float get_flow_sensor_max_volume();

  /**
   * @brief returns the flow sensor rate time in seconds
   */
  float get_flow_sensor_rate_time();

  /**
   * @brief returns the flow ir sensor raw state (e.g. IR active/inactive unaffected by filters)
   */
  bool get_flow_ir_sensor_raw_state();

  /**
   * @brief returns the last time (in milliseconds since boot)
   *        the flow sensor switched to active from inactive.
   *        This is useful for diagnostics.
   */
  uint32_t get_flow_sensor_last_switched_to_active_time();

  /**
   * @brief returns the last time (in milliseconds since boot)
   *        the flow sensor switched to inactive from active.
   *        This is useful for diagnostics.
   */
  uint32_t get_flow_sensor_last_switched_to_inactive_time();

  /**
   * @brief returns the pulse rate volume
   */
  float get_pulse_rate_volume();

  /**
   * @brief returns the pressure sensor test flag
   */
  bool get_pressure_sensor_test_flag();

  /**
   * @brief returns the pulse sensor state
   *
   * @return true if the pulse sensor is active
   * @return false if the pulse sensor is inactive
   */
  bool get_pulse_sensor();

  /**
   * @brief processes the pressure with the test sensor value
   */
  void pressure_test_sensor_process(float pressure);

  /**
   * @brief sets the status (RGB) LED GPIO pins
   */
  void set_status_led(GPIOPin *status_led_red_gpio_pin, GPIOPin *status_led_green_gpio_pin,
                      GPIOPin *status_led_blue_gpio_pin);

  void setup() override;

  /**
   * @brief loop method
   *
   * This method is called repeatedly.
   * Analogous to Arduino's loop(). setup() is guaranteed to be called before this.
   * Important: this must not take more than 30ms to execute.
   */
  void loop() override;
  void dump_config() override;

  /**
   * @brief Get the setup priority object. It affects the order/priority,
   *        that this component, should get initialized by ESPHome.
   *
   * @return float
   */
  float get_setup_priority() const override;

  /**
   * @brief saves the state of the component using a debouncer
   * to avoid too frequent writes to the flash memory.
   * it will only save once, at the end of the debounce period.
   */
  void save_state();

  /**
   * @brief sets the status LED to the given RGB color
   *
   * @param color the color to set the status LED to
   */
  void set_status_led(StatusLEDColor color);

  /**
   * @brief adds a fault to the faults list, if not already present.
   */
  void add_fault(const std::string &fault);

 private:
  /**
   * @brief if true, the component has detected a fault and the status LED/log should get updated.
   *        those faults could be detected by the sensors themselves (in isolation) or by the component
   *        based on the sensors' readings.
   *        if false, the component as far as it knows, is working fine.
   *
   *        Important: once the flag becomes true, it will not change back to false,
   *        until the component is restarted (the device is rebooted).
   *        that's to ensure that the user is aware of the fault and
   *        action has been taken to investigate it.
   *
   *        This is not using status_set_error()/status_has_error()/mark_failed()
   *        to ensure the component itself is not marked as failed/unrecoverable,
   *        since the sensor fault may be a false positive or a temporary issue and
   *        the component should nto be blocked/etc.
   */
  bool has_fault_{false};

  /**
   * @brief a list of faults detected by the component.
   *        it is used for diagnostics and reporting.
   */
  std::vector<std::string> faults = {};

  /**
   * @brief the last time (in milliseconds since boot)
   *        the faults list was logged, to avoid flooding the logs.
   */
  uint32_t last_fault_log_time_{0};

  /**
   * @brief logs the faults to the ESPHome log, if any.
   *        it will only log once every LOG_FAULTS_FREQUENCY milliseconds,
   *        to avoid flooding the logs.
   */
  void log_faults();

  /**
   * @brief Pointer to the shared ADS1115 component
   */
  ads1115::ADS1115Component *ads1115_{nullptr};

  /**
   * @brief the flow sensor
   *
   */
  FlowSensor *flow_sensor_{nullptr};

  /**
   * @brief the flow ir sensor
   *
   */
  FlowIrSensor *flow_ir_sensor_{nullptr};

  /**
   * @brief the pulse sensor
   *
   */
  PulseSensor *pulse_sensor_{nullptr};

  /**
   * @brief the pressure sensor
   *
   */
  PressureSensor *pressure_sensor_{nullptr};

  /**
   * @brief the pressure test sensor
   *
   */
  PressureTestSensor *pressure_test_sensor_{nullptr};

  /**
   * @brief the pressure sensor calibration number
   */
  PressureSensorCalibration *pressure_sensor_calibration_{nullptr};

  /**
   * @brief the flow sensor active duration number
   */
  FlowSensorMinDuration *flow_sensor_min_duration_{nullptr};

  /**
   * @brief the pressure sensor test flag
   */
  PressureSensorTest *pressure_sensor_test_{nullptr};

  /**
   * @brief the flow sensor problem sensor
   */
  FlowSensorProblemSensor *flow_sensor_problem_sensor_{nullptr};

  /**
   * @brief the flow IR sensor problem sensor
   */
  FlowIrSensorProblemSensor *flow_ir_sensor_problem_sensor_{nullptr};

  /**
   * @brief the pulse sensor problem sensor
   */
  PulseSensorProblemSensor *pulse_sensor_problem_sensor_{nullptr};

  /**
   * @brief the pressure sensor problem sensor
   */
  PressureSensorProblemSensor *pressure_sensor_problem_sensor_{nullptr};

  /**
   * @brief the time (millis) when the last save_state() was called
   */
  uint32_t last_save_state_call_{0};

  /**
   * @brief if true, there's a pending save_state() call
   *        that should be executed after the debounce period
   */
  bool save_state_debounced_{false};

  /**
   * @brief preferences object.
   *
   * It manages perstistent state storage
   * across device reboots, etc.
   *
   * @see esphome/components/climate/climate.cpp::Climate::restore_state_()
   *
   */
  ESPPreferenceObject pref_;

  /**
   * @brief loads the state from the flash memory and
   * sets the appropriate values to the component with it.
   */
  void load_state_();

  /**
   * @brief saves the state to the flash memory using
   * the appropriate component values.
   */
  void save_state_();

  /**
   * @brief checks if there's a pending save_state() call
   *        and executes it if the debounce period has passed.
   *
   * @see loop()
   */
  void save_state_pending_check_();

  /**
   * @brief the GPIO pin for the red status (RGB) LED
   */
  GPIOPin *status_led_red_gpio_pin_{nullptr};
  /**
   * @brief the GPIO pin for the green status (RGB) LED
   */
  GPIOPin *status_led_green_gpio_pin_{nullptr};
  /**
   * @brief the GPIO pin for the blue status (RGB) LED
   */
  GPIOPin *status_led_blue_gpio_pin_{nullptr};

  /**
   * @brief the current RGB color of the status LED
   */
  StatusLEDColor status_led_color_{StatusLEDColor::OFF};
};

}  // namespace fswm100
}  // namespace esphome
