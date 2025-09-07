#include "esphome/core/log.h"
#include "fswm100.h"
#include "pressure_sensor.h"

namespace esphome {
namespace fswm100 {

PressureSensor::PressureSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void PressureSensor::setup(float effective_noise_floor, float min_voltage, float max_voltage, float min_pressure,
                           float max_pressure, ads1115::ADS1115Multiplexer multiplexer, ads1115::ADS1115Gain gain,
                           ads1115::ADS1115Samplerate sample_rate, ads1115::ADS1115Resolution resolution) {
  ESP_LOGCONFIG(TAG, "PressureSensor setup start.");
  this->effective_noise_floor_ = effective_noise_floor;
  this->min_voltage_ = min_voltage;
  this->max_voltage_ = max_voltage;
  this->min_pressure_ = min_pressure;
  this->max_pressure_ = max_pressure;
  // calculate
  this->voltage_factor_ = (max_pressure - min_pressure) / (max_voltage - min_voltage);
  // ADS1115
  this->multiplexer_ = multiplexer;
  this->gain_ = gain;
  this->sample_rate_ = sample_rate;
  this->resolution_ = resolution;
  // initial state publish
  this->publish_state(0);
  ESP_LOGCONFIG(TAG, "PressureSensor setup complete.");
}

void PressureSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "PressureSensor:");
  ESP_LOGCONFIG(TAG, "  effective noise floor:", this->effective_noise_floor_);
  ESP_LOGCONFIG(TAG, "  min voltage:", this->min_voltage_);
  ESP_LOGCONFIG(TAG, "  max voltage:", this->max_voltage_);
  ESP_LOGCONFIG(TAG, "  min pressure:", this->min_pressure_);
  ESP_LOGCONFIG(TAG, "  max pressure:", this->max_pressure_);
  // calculated
  ESP_LOGCONFIG(TAG, "  voltage factor:", this->voltage_factor_);
  // ADS1115
  ESP_LOGCONFIG(TAG, "  multiplexer:", this->multiplexer_);
  ESP_LOGCONFIG(TAG, "  gain:", this->gain_);
  ESP_LOGCONFIG(TAG, "  sample rate:", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  resolution:", this->resolution_);
}

bool PressureSensor::has_fault() { return this->has_fault_; }

float PressureSensor::voltage_to_pressure(float voltage) {
  return (voltage - this->min_voltage_) * this->voltage_factor_ + this->min_pressure_;
}

void PressureSensor::loop() {
  /**
   * the voltage has already been adjusted for gain and resolution.
   * meaning, we get the proper min/max voltage for the range it can provide.
   *
   * @example 0-4.96 (V) for the IC L7805
   */
  float new_sensor_state = abs(this->fswm100_->get_ads1115()->request_measurement(
      this->multiplexer_, this->gain_, this->resolution_, this->sample_rate_));

  // diagnostics: check for invalid reading
  if (!this->has_fault_ && std::isnan(new_sensor_state)) {
    ESP_LOGE(TAG, "Failed to read from ADS1115 channel for '%s'. Result was NaN.", this->get_name().c_str());
    this->fswm100_->add_fault("Failed to read from ADS1115 channel for pressure sensor. Result was NaN.");
    this->has_fault_ = true;
    return;
  }

  ESP_LOGVV(TAG, "'%s': Read voltage from ADS1115 channel %d: %.4f V", this->get_name().c_str(),
            static_cast<int>(this->multiplexer_), new_sensor_state);

  // diagnostics: check if the voltage is within the expected range
  if (!this->has_fault_ && new_sensor_state < this->min_voltage_ || new_sensor_state > this->max_voltage_) {
    ESP_LOGW(TAG, "'%s': Voltage %.4f V is out of range (%.4f V - %.4f V).", this->get_name().c_str(), new_sensor_state,
             this->min_voltage_, this->max_voltage_);
    this->fswm100_->add_fault("Pressure sensor voltage out of range: " + std::to_string(new_sensor_state) +
                              " V. Expected range: " + std::to_string(this->min_voltage_) + " V - " +
                              std::to_string(this->max_voltage_) + " V.");
    this->has_fault_ = true;
  }

  // has the state changed enough to publish?
  if (abs(this->last_sensor_state_ - new_sensor_state) > this->effective_noise_floor_) {
    float pressure =
        this->voltage_to_pressure(new_sensor_state) * this->fswm100_->get_pressure_sensor_calibration_multiplier();

    ESP_LOGVV(TAG, "'%s': %.4f Voltage", this->get_name().c_str(), new_sensor_state);
    ESP_LOGVV(TAG, "'%s': Voltage Delta %.4f", this->get_name().c_str(),
              abs(this->last_sensor_state_ - new_sensor_state));
    ESP_LOGVV(TAG, "'%s': Converted to %.4f pressure", this->get_name().c_str(), pressure);

    if (pressure < this->min_pressure_) {
      pressure = this->min_pressure_;
    }
    if (pressure > this->max_pressure_) {
      pressure = this->max_pressure_;
    }

    // when the pressure has dropped considerably (e.g. -5% or more),
    // keep track of the last time this happened, for diagnostics
    if (this->state != 0.0f &&
        ((pressure - this->state) / this->state) * 100.0f <= CONSIDERABLE_PRESSURE_DROP_PERCENTAGE) {
      this->last_time_pressure_dropped_considerably_ = millis();
    }

    // attempt to publish (this uses filters)
    this->publish_state(pressure);

    // when the pressure sensor calibration multiplier changed,
    // force an immediate publish of the current pressure (without filters),
    // so the user can see the new value immediately, during the calibration process.
    if (this->last_pressure_sensor_calibration_multiplier_ !=
        this->fswm100_->get_pressure_sensor_calibration_multiplier()) {
      this->last_pressure_sensor_calibration_multiplier_ = this->fswm100_->get_pressure_sensor_calibration_multiplier();
      this->internal_send_state_to_frontend(pressure);
    }

    // when the test starts/ends, force an immediate publish of the current pressure
    // so that there's an absolute pressure value tied to those points in time, besides
    // the relative pressure value changes.
    if (this->fswm100_->get_pressure_sensor_test_flag() != this->last_pressure_sensor_test_flag_) {
      this->last_pressure_sensor_test_flag_ = this->fswm100_->get_pressure_sensor_test_flag();
      this->internal_send_state_to_frontend(pressure);
    }

    this->fswm100_->pressure_test_sensor_process(pressure);
  }

  // diagnostics: check flow/pressure correlation, when the flow sensor state changed or we have a pending check
  if (!this->has_fault_ && (this->fswm100_->get_flow_sensor_state() != this->last_flow_sensor_state_ ||
                            this->flow_pressure_correlation_pending_)) {
    // when the flow sensor just switched to active, start a pending check
    if (this->fswm100_->get_flow_sensor_state() > 0 && this->last_flow_sensor_state_ == 0.0) {
      this->flow_pressure_correlation_pending_ = true;
    }

    // when we have a pending check, after a delay so that the pressure had time to react
    // check if there's a correlation between the flow and pressure drop
    if (this->flow_pressure_correlation_pending_ &&
        millis() - this->fswm100_->get_flow_sensor_last_switched_to_active_time() >=
            this->fswm100_->get_flow_sensor_min_duration()) {
      // clear the pending flag, so that we don't check again until the next time the flow sensor switches to active
      this->flow_pressure_correlation_pending_ = false;

      /**
       * the period which we look back for activity
       * either on last flow switched to inactive or significant pressure drop
       */
      float lookback_period =
          this->fswm100_->get_flow_sensor_min_duration() * IR_SENSOR_FAULT_TIME_SINCE_ACTIVITY_MULTIPLIER;
      /**
       * 1. has enough time passed, since the last time we switched to inactive?
       * (i.e. avoid too frequent switching active/inactive, e.g. when the meter can barely detect the ultra low flow
       * since when that happens, the pressure is most likely to remain stable, since the flow does not really
       * start/stop, we just have a difficulty detecting it)
       *
       * 2. has enough time passed, since the last time the pressure dropped considerably?
       * (i.e. if the pressure dropped recently, that means that the correlation is there and everything is normal)
       */
      if (millis() - this->fswm100_->get_flow_sensor_last_switched_to_inactive_time() > lookback_period &&
          millis() - this->last_time_pressure_dropped_considerably_ > lookback_period) {
        // increase the fault counter, since we have active flow but no recent pressure drop correlation
        flow_pressure_correlation_fault_counter_++;
        // if the counter exceeds the threshold, we have a fault
        if (flow_pressure_correlation_fault_counter_ >= FLOW_PRESSURE_CORRELATION_FAULT_THRESHOLD) {
          ESP_LOGE(TAG, "'%s': Flow sensor switched to active but no pressure drop correlation detected.",
                   this->get_name().c_str());
          this->fswm100_->add_fault("Flow sensor switched to active but no pressure drop correlation detected.");
          this->has_fault_ = true;
        }
      } else {
        // reset the counter, since everything appears normal
        flow_pressure_correlation_fault_counter_ = 0;
      }
    }
    // update the last known state
    this->last_flow_sensor_state_ = this->fswm100_->get_flow_sensor_state();
  }
}

}  // namespace fswm100
}  // namespace esphome
