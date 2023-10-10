// Copyright 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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


#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "libstatistics_collector/moving_average_statistics/types.hpp"
#include "libstatistics_collector/topic_statistics_collector/received_message_period.hpp"
#include "rmw/types.h"

namespace
{
using ReceivedMessagePeriodCollector = libstatistics_collector::ReceivedMessagePeriodCollector;

constexpr const std::chrono::seconds kDefaultDurationSeconds{1};
constexpr const double kExpectedAverageMilliseconds{1000.0};
constexpr const double kExpectedMinMilliseconds{1000.0};
constexpr const double kExpectedMaxMilliseconds{1000.0};
constexpr const double kExpectedStandardDeviation{0.0};
}  // namespace

TEST(ReceivedMessagePeriodTest, TestPeriodMeasurement) {
  ReceivedMessagePeriodCollector test{};
  libstatistics_collector::moving_average_statistics::StatisticData stats;

  rmw_message_info_t msg_info = rmw_get_zero_initialized_message_info();

  EXPECT_FALSE(test.IsStarted()) << "Expected to be not started after constructed";

  EXPECT_TRUE(test.Start()) << "Expected Start() to be successful";
  EXPECT_TRUE(test.IsStarted()) << "Expected to be started";

  rcl_time_point_value_t fake_now_nanos_{1};

  for (int i = 0; i < 4; i++) {
    test.OnMessageReceived(msg_info, fake_now_nanos_);
    stats = test.GetStatisticsResults();
    EXPECT_EQ(i, stats.sample_count) << "Expected " << i << " samples to be collected";

    fake_now_nanos_ +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(kDefaultDurationSeconds).count();
  }

  EXPECT_EQ(kExpectedAverageMilliseconds, stats.average);
  EXPECT_EQ(kExpectedMinMilliseconds, stats.min);
  EXPECT_EQ(kExpectedMaxMilliseconds, stats.max);
  EXPECT_EQ(kExpectedStandardDeviation, stats.standard_deviation);
}

TEST(ReceivedMessagePeriodTest, TestGetStatNameAndUnit) {
  ReceivedMessagePeriodCollector test{};

  EXPECT_FALSE(test.GetMetricName().empty());
  EXPECT_FALSE(test.GetMetricUnit().empty());
}
