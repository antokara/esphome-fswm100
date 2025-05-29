#pragma once

#include "esphome/core/hal.h"

class LowPassFilter {
 public:
  // Constructor
  // time_constant_samples: The time constant of the filter, in number of samples.
  //                        If 0 or negative, it might effectively default to behavior
  //                        driven by window_size for 'k'.
  // window_size: The number of initial samples to process before the filter output
  //              is considered valid/ready. Must be >= 1.
  LowPassFilter(double time_constant_samples, int window_size)
      : time_constant_samples_(time_constant_samples),
        window_size_(window_size),
        current_filtered_value_(0.0),  // Will be set by the first sample
        samples_processed_(0),
        initialized_(false) {
    // window_size must be at least 1 for the filter's readiness logic.
    if (window_size_ < 1) {
      // In a real application, you might throw an error or handle this case.
      // For this example, we'll proceed, but behavior might be unexpected if window_size < 1.
      // Home Assistant configurations typically have window_size >= 1.
      // Let's ensure it's at least 1 for k_w calculation.
      if (window_size_ < 1)
        window_size_ = 1;  // Basic safeguard
    }

    // Determine the effective 'k' factor for the filter.
    // k = max(k_w, k_t)
    // k_w = window_size (as float)
    // k_t = time_constant_samples (assuming time_step = 1 sample period)
    double k_w = static_cast<double>(window_size_);
    double k_t = (time_constant_samples_ > 0) ? time_constant_samples_ : 0.0;

    if (k_t > 0) {  // If time_constant is specified and positive
      k_effective_ = std::max(k_w, k_t);
    } else {  // If time_constant is not specified or zero, k_w (from window_size) is used.
      k_effective_ = k_w;
    }

    // Ensure k_effective is at least 1.0.
    // If window_size_ >= 1, k_w will be >= 1.
    // If time_constant_samples_ is also non-negative, k_effective_ should be >= 1.
    // This safeguard is mostly for hypothetical cases where k_effective_ might become < 1.
    if (k_effective_ < 1.0) {
      k_effective_ = 1.0;
    }
  }

  // Adds a new sample to the filter.
  // Returns the filtered value if the filter is ready (window full), otherwise std::nullopt.
  float add_sample(double new_value) {
    samples_processed_++;

    if (!initialized_) {
      current_filtered_value_ = new_value;  // First value initializes the state
      initialized_ = true;
    } else {
      // Apply the low-pass filter formula:
      // y[n] = y[n-1] + (x[n] - y[n-1]) / k_effective
      // This is equivalent to an Exponential Moving Average (EMA):
      // y[n] = y[n-1] * (1 - alpha) + x[n] * alpha, where alpha = 1/k_effective
      if (k_effective_ > 0) {  // Should always be true due to constructor logic (k_effective_ >= 1.0)
        current_filtered_value_ = current_filtered_value_ + (new_value - current_filtered_value_) / k_effective_;
      } else {
        // Fallback, though k_effective_ should be >= 1.0
        current_filtered_value_ = new_value;
      }
    }

    // The filter output is considered valid once 'window_size_' samples have been processed.
    if (is_ready()) {
      return current_filtered_value_;
    } else {
      return 0;  // Not ready yet
    }
  }

  // Returns true if the filter has processed enough samples (i.e., window_size samples)
  // and is ready to output filtered values.
  bool is_ready() const { return samples_processed_ >= window_size_; }

  // Gets the current internal state of the filter.
  // This value is updated with each sample. It represents the latest filtered value,
  // but should typically be used in conjunction with is_ready().
  // Returns 0.0 if not yet initialized by any sample.
  double get_current_internal_state() const { return current_filtered_value_; }

  // Gets the effective k factor used by the filter. Useful for debugging/verification.
  double get_k_effective() const { return k_effective_; }

 private:
  double time_constant_samples_;  // Stores the configured time constant
  int window_size_;               // Stores the configured window size
  double k_effective_;            // The calculated effective k for the filter formula

  double current_filtered_value_;  // Stores the current filtered state (y[n])
  int samples_processed_;          // Counter for the number of samples processed
  bool initialized_;               // Flag to check if the filter state has been initialized by the first sample
};
