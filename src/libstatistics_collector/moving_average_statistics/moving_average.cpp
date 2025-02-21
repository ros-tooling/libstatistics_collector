// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <cmath>

#include <algorithm>
#include <limits>
#include <mutex>
#include <numeric>
#include <type_traits>

#include "libstatistics_collector/moving_average_statistics/moving_average.hpp"
#include "libstatistics_collector/moving_average_statistics/types.hpp"

namespace libstatistics_collector
{
namespace moving_average_statistics
{

MovingAverageStatistics::MovingAverageStatistics(const std::size_t & window_size)
: window_size_{window_size} {}

double MovingAverageStatistics::Average() const
{
  return GetStatistics().average;
}

double MovingAverageStatistics::Max() const
{
  return GetStatistics().max;
}

double MovingAverageStatistics::Min() const
{
  return GetStatistics().min;
}

double MovingAverageStatistics::StandardDeviation() const
{
  return GetStatistics().standard_deviation;
}

StatisticData MovingAverageStatistics::GetStatistics() const
{
  std::lock_guard<std::mutex> guard{mutex_};
  StatisticData to_return;

  if (count_ == 0) {
    return to_return;  // already initialized
  }

  // update based on current observations
  to_return.sample_count = count_;
  to_return.average = average_;
  to_return.min = min_;
  to_return.max = max_;
  to_return.standard_deviation = std::sqrt(sum_of_square_diff_from_mean_ / count_);

  return to_return;
}

void MovingAverageStatistics::Reset()
{
  std::lock_guard<std::mutex> guard{mutex_};
  average_ = 0;
  min_ = std::numeric_limits<double>::max();
  max_ = std::numeric_limits<double>::lowest();
  sum_of_square_diff_from_mean_ = 0;
  count_ = 0;
  window_buffer_.clear();
}

void MovingAverageStatistics::AddMeasurement(const double item)
{
  std::lock_guard<std::mutex> guard{mutex_};

  if (!std::isnan(item)) {
    const double previous_average = average_;
    if (window_size_ == 0 || count_ < window_size_) {
      count_++;
      average_ = previous_average + (item - previous_average) / count_;
      if (window_size_ != 0) {
        window_buffer_.push_back(item);
      }
    } else if (window_size_ > 0) {
      const double old_item = window_buffer_.front();
      window_buffer_.pop_front();
      average_ = previous_average + (item - old_item) / window_size_;
      sum_of_square_diff_from_mean_ = sum_of_square_diff_from_mean_ - (old_item - average_) *
        (old_item - previous_average);
      if (std::abs(old_item - min_) < std::numeric_limits<double>::epsilon()) {
        min_ = *std::min_element(window_buffer_.begin(), window_buffer_.end());
      }
      if (std::abs(old_item - max_) < std::numeric_limits<double>::epsilon()) {
        max_ = *std::max_element(window_buffer_.begin(), window_buffer_.end());
      }
      window_buffer_.push_back(item);
    }
    min_ = std::min(min_, item);
    max_ = std::max(max_, item);
    sum_of_square_diff_from_mean_ = sum_of_square_diff_from_mean_ + (item - previous_average) *
      (item - average_);
  }
}

uint64_t MovingAverageStatistics::GetCount() const
{
  std::lock_guard<std::mutex> guard{mutex_};
  return count_;
}

}  // namespace moving_average_statistics
}  // namespace libstatistics_collector
