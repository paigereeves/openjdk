/*
 * Copyright (c) 2002, 2015, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#include "precompiled.hpp"
#include "gc/shared/gcUtil.hpp"
#include <stdlib.h>
#include <string.h>

// Catch-all file for utility classes

float AdaptiveWeightedAverage::compute_adaptive_average(float new_sample,
                                                        float average) {
  // We smooth the samples by not using weight() directly until we've
  // had enough data to make it meaningful. We'd like the first weight
  // used to be 1, the second to be 1/2, etc until we have
  // OLD_THRESHOLD/weight samples.
  unsigned count_weight = 0;

  // Avoid division by zero if the counter wraps (7158457)
  if (!is_old()) {
    count_weight = OLD_THRESHOLD/count();
  }

  unsigned adaptive_weight = (MAX2(weight(), count_weight));

  float new_avg = exp_avg(average, new_sample, adaptive_weight);

  return new_avg;
}

void AdaptiveWeightedAverage::sample(float new_sample) {
  increment_count();

  // Compute the new weighted average
  float new_avg = compute_adaptive_average(new_sample, average());
  set_average(new_avg);
  _last_sample = new_sample;
}

void AdaptiveWeightedAverage::print() const {
  print_on(tty);
}

void AdaptiveWeightedAverage::print_on(outputStream* st) const {
  guarantee(false, "NYI");
}

void AdaptivePaddedAverage::print() const {
  print_on(tty);
}

void AdaptivePaddedAverage::print_on(outputStream* st) const {
  guarantee(false, "NYI");
}

void AdaptivePaddedNoZeroDevAverage::print() const {
  print_on(tty);
}

void AdaptivePaddedNoZeroDevAverage::print_on(outputStream* st) const {
  guarantee(false, "NYI");
}

void AdaptivePaddedAverage::sample(float new_sample) {
  // Compute new adaptive weighted average based on new sample.
  AdaptiveWeightedAverage::sample(new_sample);

  // Now update the deviation and the padded average.
  float new_avg = average();
  float new_dev = compute_adaptive_average(fabsd(new_sample - new_avg),
                                           deviation());
  set_deviation(new_dev);
  set_padded_average(new_avg + padding() * new_dev);
  _last_sample = new_sample;
}

void AdaptivePaddedNoZeroDevAverage::sample(float new_sample) {
  // Compute our parent classes sample information
  AdaptiveWeightedAverage::sample(new_sample);

  float new_avg = average();
  if (new_sample != 0) {
    // We only create a new deviation if the sample is non-zero
    float new_dev = compute_adaptive_average(fabsd(new_sample - new_avg),
                                             deviation());

    set_deviation(new_dev);
  }
  set_padded_average(new_avg + padding() * deviation());
  _last_sample = new_sample;
}

LinearLeastSquareFit::LinearLeastSquareFit(unsigned weight) :
  _sum_x(0), _sum_x_squared(0), _sum_y(0), _sum_xy(0),
  _intercept(0), _slope(0), _mean_x(weight), _mean_y(weight) {}

void LinearLeastSquareFit::update(double x, double y) {
  _sum_x = _sum_x + x;
  _sum_x_squared = _sum_x_squared + x * x;
  _sum_y = _sum_y + y;
  _sum_xy = _sum_xy + x * y;
  _mean_x.sample(x);
  _mean_y.sample(y);
  assert(_mean_x.count() == _mean_y.count(), "Incorrect count");
  if ( _mean_x.count() > 1 ) {
    double slope_denominator;
    slope_denominator = (_mean_x.count() * _sum_x_squared - _sum_x * _sum_x);
    // Some tolerance should be injected here.  A denominator that is
    // nearly 0 should be avoided.

    if (slope_denominator != 0.0) {
      double slope_numerator;
      slope_numerator = (_mean_x.count() * _sum_xy - _sum_x * _sum_y);
      _slope = slope_numerator / slope_denominator;

      // The _mean_y and _mean_x are decaying averages and can
      // be used to discount earlier data.  If they are used,
      // first consider whether all the quantities should be
      // kept as decaying averages.
      // _intercept = _mean_y.average() - _slope * _mean_x.average();
      _intercept = (_sum_y - _slope * _sum_x) / ((double) _mean_x.count());
    }
  }
}

double LinearLeastSquareFit::y(double x) {
  double new_y;

  if ( _mean_x.count() > 1 ) {
    new_y = (_intercept + _slope * x);
    return new_y;
  } else {
    return _mean_y.average();
  }
}

// Both decrement_will_decrease() and increment_will_decrease() return
// true for a slope of 0.  That is because a change is necessary before
// a slope can be calculated and a 0 slope will, in general, indicate
// that no calculation of the slope has yet been done.  Returning true
// for a slope equal to 0 reflects the intuitive expectation of the
// dependence on the slope.  Don't use the complement of these functions
// since that intuitive expectation is not built into the complement.
bool LinearLeastSquareFit::decrement_will_decrease() {
  return (_slope >= 0.00);
}

bool LinearLeastSquareFit::increment_will_decrease() {
  return (_slope <= 0.00);
}

int read_perf_fd_env(const char *envName) {
  const char *fd_str = std::getenv(envName);
  if (fd_str == nullptr) {
    return -1;
  }
  int fd = atoi(fd_str);
  if (fd < 0) {
    return -1;
  }
  return fd;
}

bool perf_ctrl_send_command(const char *command) {
  log_info(gc, marking)("perf_ctrl_send_command %s", command);
  static int ctrl_fd_opt = read_perf_fd_env("PERF_CTL_FD");
  static int ctrl_ack_fd_opt = read_perf_fd_env("PERF_CTL_ACK_FD");
  log_info(gc, marking)("ctrl_fd_opt = %d, ctrl_ack_fd_opt = %d", ctrl_fd_opt, ctrl_ack_fd_opt);
  if (ctrl_fd_opt == -1) {
    return false;
  }
  int ctrl_fd = ctrl_fd_opt;
  ssize_t bytes_written = write(ctrl_fd, command, strlen(command));

  if (bytes_written == -1) {
    fprintf(stderr, "perf_ctrl: Failed to write command '%s' to FIFO (FD %d)\n",
            command, ctrl_fd);
    return false;
  } else if (static_cast<size_t>(bytes_written) != strlen(command)) {
    fprintf(stderr,
            "perf_ctrl: Partial write of command '%s' to FIFO (FD %d). Wrote "
            "%zd of %zd bytes.\n",
            command, ctrl_fd, bytes_written, strlen(command));
    return false; // Treat partial write as failure for simplicity
  }
  if (ctrl_ack_fd_opt != -1) {
    int ctrl_ack_fd = ctrl_ack_fd_opt;
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = read(ctrl_ack_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read != 5) {
      fprintf(
          stderr,
          "perf_ctrl: Failed to read exactly 4 bytes, from ACK FIFO (FD %d): '%s'\n",
          ctrl_ack_fd, buffer);
      return false;
    }
    if (strcmp(buffer, "ack\n") == 0) {
      return true;
    } else {
      fprintf(stderr,
              "perf_ctrl: Unexpected response from ACK FIFO (FD %d): '%s'\n",
              ctrl_ack_fd, buffer);
      return false;
    }
  }
  return true;
}

bool in_harness = false;

void perf_ctrl_enable() {
  if (in_harness) {
    perf_ctrl_send_command("enable\n");
  }
}

void perf_ctrl_disable() {
  if (in_harness) {
    perf_ctrl_send_command("disable\n");
  }
}

extern "C" void inform_harness_begin_openjdk() {
  in_harness = true;
}

extern "C" void inform_harness_end_openjdk() {
  in_harness = false;
}