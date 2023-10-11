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
#include <string>

#include "libstatistics_collector/topic_statistics_collector/received_message_age.hpp"
#include "rmw/types.h"

namespace
{
using ReceivedMessageAgeCollector = libstatistics_collector::ReceivedMessageAgeCollector;

constexpr const std::chrono::seconds kDefaultDurationSeconds{1};
constexpr const double kExpectedAverageMilliseconds{2000.0};
constexpr const double kExpectedMinMilliseconds{1000.0};
constexpr const double kExpectedMaxMilliseconds{3000.0};
constexpr const double kExpectedStandardDeviation{816.49658092772597};
constexpr const int kDefaultTimesToTest{10};
constexpr const int64_t kDefaultTimeMessageReceived{1000};
constexpr const rcl_time_point_value_t kStartTime{123456789};
}  // namespace

TEST(ReceivedMessageAgeTest, TestMessagesWithDifferentDefaultTimes) {
  libstatistics_collector::moving_average_statistics::StatisticData stats;
  {
    // Initialize message_info source_timestamp at 0
    ReceivedMessageAgeCollector msg_collector{};
    rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
    message_info.source_timestamp = 0;

    for (int i = 0; i < kDefaultTimesToTest; ++i) {
      msg_collector.OnMessageReceived(message_info, kDefaultTimeMessageReceived);
      stats = msg_collector.GetStatisticsResults();
      EXPECT_EQ(0, stats.sample_count) << "Expect 0 samples to be collected";
    }
  }
  {
    // Initialize message_info source_timestamp at random time
    ReceivedMessageAgeCollector msg_collector{};
    rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
    message_info.source_timestamp = kDefaultTimeMessageReceived;
    auto fake_now_nanos_ = kDefaultTimeMessageReceived;

    for (int i = 0; i < kDefaultTimesToTest; ++i) {
      msg_collector.OnMessageReceived(message_info, fake_now_nanos_);
      stats = msg_collector.GetStatisticsResults();
      EXPECT_EQ(i + 1, stats.sample_count) << "Expect " << i + 1 << " samples to be collected";
      fake_now_nanos_++;
    }
  }
  {
    // Initialize message_info source_timestamp at random time and fake_now_nanos_ at 0
    ReceivedMessageAgeCollector msg_collector{};
    rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
    message_info.source_timestamp = kDefaultTimeMessageReceived;
    auto fake_now_nanos_ = 0;

    for (int i = 0; i < kDefaultTimesToTest; ++i) {
      msg_collector.OnMessageReceived(message_info, fake_now_nanos_);
      stats = msg_collector.GetStatisticsResults();
      EXPECT_EQ(0, stats.sample_count) << "Expect 0 samples to be collected";
    }
  }
}

TEST(ReceivedMessageAgeTest, TestAgeMeasurement) {
  ReceivedMessageAgeCollector test_collector{};
  libstatistics_collector::moving_average_statistics::StatisticData stats;

  EXPECT_FALSE(test_collector.IsStarted()) << "Expect to be not started after constructed";

  EXPECT_TRUE(test_collector.Start()) << "Expect Start() to be successful";
  EXPECT_TRUE(test_collector.IsStarted()) << "Expect to be started";

  rcl_time_point_value_t fake_now_nanos_{kStartTime};

  rmw_message_info_t msg = rmw_get_zero_initialized_message_info();
  msg.source_timestamp = fake_now_nanos_;

  for (int i = 1; i < 4; i++) {
    fake_now_nanos_ +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(kDefaultDurationSeconds).count();

    test_collector.OnMessageReceived(msg, fake_now_nanos_);
    stats = test_collector.GetStatisticsResults();
    EXPECT_EQ(i, stats.sample_count);
  }

  stats = test_collector.GetStatisticsResults();
  EXPECT_EQ(kExpectedAverageMilliseconds, stats.average);
  EXPECT_EQ(kExpectedMinMilliseconds, stats.min);
  EXPECT_EQ(kExpectedMaxMilliseconds, stats.max);
  EXPECT_DOUBLE_EQ(kExpectedStandardDeviation, stats.standard_deviation);
}

TEST(ReceivedMessageAgeTest, TestGetStatNameAndUnit) {
  ReceivedMessageAgeCollector test_collector{};

  EXPECT_FALSE(test_collector.GetMetricName().empty());
  EXPECT_FALSE(test_collector.GetMetricUnit().empty());
}
