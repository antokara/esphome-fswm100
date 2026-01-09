#include "esphome/core/log.h"
#include "fswm100.h"
#include "flow_sensor.h"

namespace esphome {
namespace fswm100 {

FlowSensor::FlowSensor(FSWM100 *fswm100) { fswm100_ = fswm100; };

void FlowSensor::setup(const std::function<std::vector<sensor::Filter *>()> &filters_factory, float min_volume,
                       float max_volume, float rate_time, float flow_rate_time_between_pulses_multiplier,
                       float fault_time_since_activity_multiplier, int inactivity_fault_counter_threshold) {
  ESP_LOGCONFIG(TAG, "FlowSensor setup start.");
  this->filters_factory_ = filters_factory;
  this->min_volume_ = min_volume;
  this->max_volume_ = max_volume;
  this->rate_time_ = rate_time;
  this->flow_rate_time_between_pulses_multiplier_ = flow_rate_time_between_pulses_multiplier;
  this->fault_time_since_activity_multiplier_ = fault_time_since_activity_multiplier;
  this->inactivity_fault_counter_threshold_ = inactivity_fault_counter_threshold;
  // initialize values...
  this->publish(0, true);
  ESP_LOGCONFIG(TAG, "FlowSensor setup complete.");
}

void FlowSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "FlowSensor:");
  ESP_LOGCONFIG(TAG, "  min volume:", this->min_volume_);
  ESP_LOGCONFIG(TAG, "  max volume:", this->max_volume_);
  ESP_LOGCONFIG(TAG, "  rate time:", this->rate_time_);
  ESP_LOGCONFIG(TAG, "  flow rate time between pulses multiplier:", this->flow_rate_time_between_pulses_multiplier_);
  ESP_LOGCONFIG(TAG, "  fault time since activity multiplier:", this->fault_time_since_activity_multiplier_);
  ESP_LOGCONFIG(TAG, "  inactivity fault counter threshold:", this->inactivity_fault_counter_threshold_);
}

bool FlowSensor::has_fault() { return this->has_fault_; }
float FlowSensor::get_min_volume() { return this->min_volume_; }
float FlowSensor::get_max_volume() { return this->max_volume_; }
float FlowSensor::get_rate_time() { return this->rate_time_; }
uint32_t FlowSensor::get_last_switched_to_active_time() { return this->last_switched_to_active_time_; }
uint32_t FlowSensor::get_last_switched_to_inactive_time() { return this->last_switched_to_inactive_time_; }

void FlowSensor::active() {
  // update the last active time (not switched to active)
  this->last_active_time_ = millis();
  // set the last switched to active time (if we were inactive before)
  if (this->state == 0) {
    this->last_switched_to_active_time_ = this->last_active_time_;
  }
  this->publish(this->calculate_active_flow());
  // for diagnostics, mark that a flow/pulse correlation check is pending
  this->flow_pulse_correlation_pending_ = true;
}

float FlowSensor::calculate_active_flow() {
  // start with the minimum flow volume
  float flow = this->min_volume_;

  // don't attempt to calculate flow if we don't have a pulse sensor active time
  if (this->oldest_pulse_sensor_active_time_ > 0) {
    const uint32_t time_between_pulses =
        abs(long(this->newest_pulse_sensor_active_time_ - this->oldest_pulse_sensor_active_time_));
    const uint32_t time_since_newest_pulse = abs(long(millis() - this->newest_pulse_sensor_active_time_));
    const float calculated_flow = (this->rate_time_ * 1000) / (millis() - this->oldest_pulse_sensor_active_time_) *
                                  this->fswm100_->get_pulse_rate_volume();
    if (time_since_newest_pulse > long(time_between_pulses * this->flow_rate_time_between_pulses_multiplier_) ||
        (calculated_flow > this->state)) {
      // if the the current flow is greater than the last published one or
      // if time since the last pulse is greater than the time between pulses,
      // we use the newly calculated flow rate.
      flow = calculated_flow;
    } else if (this->state > 0) {
      // when not enough time has passed to calculate the new flow rate but there is an active flow,
      // use the last published flow rate
      flow = this->state;
    }
  }

  // ensure the flow is not below the minimum volume
  if (flow < this->min_volume_) {
    flow = this->min_volume_;
  }

  return flow;
}

void FlowSensor::publish(float flow, bool immediate) {
  if (immediate || this->state == 0 && flow > 0 || this->state > 0 && flow == 0) {
    this->reset_filters();
  }
  this->publish_state(flow);
}

void FlowSensor::loop() {
  bool new_flow_ir_sensor_state = this->fswm100_->get_flow_ir_sensor_state();
  bool pulse_sensor_state = this->fswm100_->get_pulse_sensor();
  if (pulse_sensor_state != this->last_pulse_sensor_state_ && pulse_sensor_state) {
    // active due to new pulse
    ESP_LOGD(TAG, "Flow: active due to new pulse");
    this->active();
    this->last_pulse_sensor_state_ = pulse_sensor_state;
    this->oldest_pulse_sensor_active_time_ = this->newest_pulse_sensor_active_time_;
    this->newest_pulse_sensor_active_time_ = millis();

  } else if (new_flow_ir_sensor_state != this->last_ir_sensor_state_ && new_flow_ir_sensor_state) {
    // active due to IR movement
    ESP_LOGV(TAG, "Flow: active due to Flow IR");
    this->active();
    this->last_ir_sensor_state_ = new_flow_ir_sensor_state;
    this->last_ir_activity_time_ = millis();
  } else if (millis() - this->last_active_time_ > this->fswm100_->get_flow_sensor_min_duration()) {
    // inactive. no pulse or IR and timed out
    if (this->state > 0) {
      // just switched to inactive
      ESP_LOGD(TAG, "Flow: switched to inactive");
      this->publish(0);
      this->last_switched_to_inactive_time_ = millis();
      this->flow_pulse_correlation_pending_ = false;
    }

  } else if (this->state > 0) {
    // active but not yet timed out
    this->publish(this->calculate_active_flow());

    // diagnostics: if we have gotten a pulse lately, within the min duration but not too soon that
    //              maybe the IR hasn't picked it up yet (it's possible)
    uint32_t time_since_newest_pulse = abs(long(millis() - this->newest_pulse_sensor_active_time_));
    if (!this->has_fault_ && this->flow_pulse_correlation_pending_ &&
        time_since_newest_pulse < this->fswm100_->get_flow_sensor_min_duration() &&
        time_since_newest_pulse > (this->fswm100_->get_flow_sensor_min_duration() / 2)) {
      // but there has been no IR activity within the min duration x the multiplier, so we may have a problem
      if (millis() - this->last_ir_activity_time_ >
          this->fswm100_->get_flow_sensor_min_duration() * this->fault_time_since_activity_multiplier_) {
        // increase the inactivity fault counter and log a warning
        this->inactivity_fault_counter_++;
        ESP_LOGW(TAG, "'%s': inactivity fault counter: %d", this->get_name().c_str(), this->inactivity_fault_counter_);

        // if we have reached the threshold, we have a fault
        if (this->inactivity_fault_counter_ >= this->inactivity_fault_counter_threshold_) {
          ESP_LOGW(
              TAG,
              "'%s': No IR activity has been detected, while the Pulse appears to have been toggled. "
              "Either the IR is having problems (false negative), or the pulse sensor is not working properly (false "
              "positive).",
              this->get_name().c_str());
          this->fswm100_->add_fault(
              "No IR activity has been detected, while the Pulse appears to have been toggled. "
              "Either the IR is having problems (false negative), or the pulse sensor is not working properly (false "
              "positive).");
          this->has_fault_ = true;
        }
      } else {
        // reset the fault counter, as we have had IR activity within the allowed time
        this->inactivity_fault_counter_ = 0;
      }
      // either way, reset the pending flag (the correlation check has succeeded or failed)
      this->flow_pulse_correlation_pending_ = false;
    }
  }

  // update if it just switched (to false)
  if (!pulse_sensor_state && pulse_sensor_state != this->last_pulse_sensor_state_) {
    this->last_pulse_sensor_state_ = pulse_sensor_state;
  }
}

void FlowSensor::reset_filters() {
  if (this->filters_factory_) {
    ESP_LOGD(TAG, "FlowSensor Resetting filters by creating new filter instances...");
    this->clear_filters();
    for (auto *filter : this->filters_factory_()) {
      this->add_filter(filter);
    }
  } else {
    ESP_LOGW(TAG, "FlowSensor No filter factory set, cannot reset filters.");
  }
}

}  // namespace fswm100
}  // namespace esphome
