#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/entity_base.h"
#include "esphome/components/ads1115/ads1115.h"
#include "pulse_sensor.h"
#include "pressure_sensor.h"

// #include "esphome/core/hal.h"
// #include "esphome/components/binary_sensor/binary_sensor.h"

// #include <vector>

// number of pulses per gallon (Pulse/Gallon)
// #define PULSE_RATE 1.0

/**
 * @brief frequency in milliseconds, to debounce the pulses.
 * in case the pulse switch toggles too fast for some reason within the defined
 * period, it will be ignored.
 */
#define PULSE_DEBOUNCE_FREQUENCY 250

struct State {
  float testValue;
} __attribute__((packed));

namespace esphome {
namespace fswm100 {

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
   * @see sensor.py:to_code(config)
   *
   * @param flow_sensor
   */
  void set_flow_sensor(sensor::Sensor *flow_sensor);

  /**
   * @brief Set the pulse sensor object
   * @see sensor.py:to_code(config)
   *
   * @param pulse_sensor
   */
  void set_pulse_sensor(PulseSensor *pulse_sensor);

  /**
   * @brief Set the pressure sensor object
   * @see sensor.py:to_code(config)
   *
   * @param pressure_sensor
   */
  void set_pressure_sensor(PressureSensor *pressure_sensor);

  void setup() override;
  void loop() override;
  void dump_config() override;

  /**
   * @brief Get the setup priority object. It affects the order/priority,
   *        that this component, should get initialized by ESPHome.
   *
   * @return float
   */
  float get_setup_priority() const override;

 protected:
  /**
   * @brief Pointer to the shared ADS1115 component
   */
  ads1115::ADS1115Component *ads1115_{nullptr};

  /**
   * @brief the flow sensor
   *
   */
  sensor::Sensor *flow_sensor_{nullptr};

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
  float pressure_sensor_state;

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
