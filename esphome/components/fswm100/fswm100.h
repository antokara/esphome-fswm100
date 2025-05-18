#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/entity_base.h"
#include "esphome/components/ads1115/ads1115.h"
#include "pulse_sensor.h"
#include "pressure_sensor.h"
#include "pressure_sensor_calibration.h"
#include "flow_sensor.h"

/**
 * @brief frequency in milliseconds, to debounce the pulses.
 * in case the pulse switch toggles too fast for some reason within the defined
 * period, it will be ignored.
 */
#define PULSE_DEBOUNCE_FREQUENCY 250

/**
 * @brief the state that can be saved/loaded from Flash Memory
 * which perists device restarts
 */
struct State {
  float testValue;
} __attribute__((packed));

namespace esphome {
namespace fswm100 {

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
   * @brief Set the pressure sensor calibration object
   *  @see __init__.py
   *
   * @param pressure_sensor_calibration
   */
  void set_pressure_sensor_calibration(PressureSensorCalibration *pressure_sensor_calibration);

  /**
   * @brief returns the pressure sensor calibration multiplier
   */
  float get_pressure_sensor_calibration_multiplier();

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

 private:
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
   * @brief the pressure sensor calibration number
   */
  PressureSensorCalibration *pressure_sensor_calibration_{nullptr};

  /**
   * @brief if the pulse sensor is considered active (after debounce)
   *
   */
  bool pulse_sensor_active;

  /**
   * @brief if true, then at least one transmission
   * of the pulse sensor, has taken place
   *
   */
  bool pulse_sensor_first_transmission;

  /**
   * @brief the time (millis)
   * when the "pulse_sensor_value" was last toggled
   *
   */
  uint32_t pulse_sensor_active_time_{0};

  /**
   * the pressure sensor state value we last sent...
   */
  float pressure_sensor_state = -1.0f;

  /**
   * the flow sensor state value we last sent...
   */
  float flow_sensor_state = -1.0f;

  // TODO: remove
  uint32_t last_transmission_{0};

  // TODO: expose public save state and set state props for the children to use...
  //       it must allow for queing though... debounce

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
  void load_state_();
  void save_state_();
};

}  // namespace fswm100
}  // namespace esphome
