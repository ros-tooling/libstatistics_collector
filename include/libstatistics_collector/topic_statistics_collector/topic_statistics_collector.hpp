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


#ifndef LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR_HPP_
#define LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR_HPP_

#include <chrono>
#include <string>

#include "rcl/time.h"

#include "rmw/types.h"

#include "libstatistics_collector/collector/collector.hpp"

namespace libstatistics_collector
{
namespace topic_statistics_collector
{

/**
 * Primary specialization class template until deprecated templated class is phased out
 * @warning Don't use templated version of the TopicStatisticsCollector, use
 * libstatistics_collector::TopicStatisticsCollector alias with rmw_message_info_t parameter in
 * the OnMessageReceived callback
 */
template<typename T = rmw_message_info_t, typename Enable = void>
class TopicStatisticsCollector : public collector::Collector
{};

/**
 * Interface to collect and perform measurements for ROS2 topic statistics.
 *
 * @tparam T the ROS2 message type to collect
 */
// *INDENT-OFF*
template<typename T>
class
  [[deprecated("Don't use templated version of the TopicStatisticsCollector, use"
  "libstatistics_collector::TopicStatisticsCollector alias instead")]]
  TopicStatisticsCollector<T, std::enable_if_t<!std::is_same<T, rmw_message_info_t>::value>>
  : public collector::Collector
{
public:
  TopicStatisticsCollector() = default;

  virtual ~TopicStatisticsCollector() = default;

  /**
   * Handle receiving a single message of type T.
   *
   * @param received_message T the ROS2 message type to collect
   * @param now_nanoseconds nanoseconds the time the message was received. Any metrics using this time assumes the
   * following 1). the time provided is strictly monotonic 2). the time provided uses the same source
   * as time obtained from the message header.
   */
  virtual void OnMessageReceived(
    const T & received_message,
    const rcl_time_point_value_t now_nanoseconds) = 0;
};
// *INDENT-ON*

/**
 * Interface to collect and perform measurements for ROS2 topic statistics.
 */
using message_info_same = std::is_same<rmw_message_info_t, rmw_message_info_t>;
template<>
class TopicStatisticsCollector<rmw_message_info_t, std::enable_if_t<message_info_same::value>>
  : public collector::Collector
{
public:
  TopicStatisticsCollector() = default;

  virtual ~TopicStatisticsCollector() = default;

  /**
   * Handle receiving a single message of type rmw_message_info_t.
   *
   * @param received_message rmw_message_info_t the ROS2 message type to collect
   * @param now_nanoseconds nanoseconds the time the message was received.
   * Any metrics using this time assumes the following:
   * 1). the time provided is strictly monotonic
   * 2). the time provided uses the same source as time obtained from the message header.
   */
  virtual void OnMessageReceived(
    const rmw_message_info_t & received_message,
    const rcl_time_point_value_t now_nanoseconds) = 0;
};

}  // namespace topic_statistics_collector

using TopicStatisticsCollector = topic_statistics_collector::TopicStatisticsCollector<>;

}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR_HPP_
