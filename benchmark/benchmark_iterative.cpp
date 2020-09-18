// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include <string>

#include "libstatistics_collector/collector/collector.hpp"
#include "libstatistics_collector/moving_average_statistics/moving_average.hpp"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;
using libstatistics_collector::moving_average_statistics::MovingAverageStatistics;
using libstatistics_collector::collector::Collector;

namespace
{
constexpr const uint64_t kSize = 4096;
constexpr const char kTestMetricName[] = "test_metric_name";
constexpr const char kTestMetricUnit[] = "test_metric_unit";
}

/**
 * Simple extension to test basic functionality
 */
class TestCollector : public libstatistics_collector::collector::Collector
{
public:
  TestCollector() = default;
  ~TestCollector() override = default;

  /**
   * Overrides to make public for testing.
   * @return
   */
  bool Start() override
  {
    return Collector::Start();
  }
  /**
   * Overrides to make public for testing.
   * @return
   */
  bool Stop() override
  {
    return Collector::Stop();
  }

  bool SetupStart() override
  {
    return true;
  }
  bool SetupStop() override
  {
    this->ClearCurrentMeasurements();
    return true;
  }

  std::string GetMetricName() const override
  {
    return kTestMetricName;
  }

  std::string GetMetricUnit() const override
  {
    return kTestMetricUnit;
  }
};

// cppcheck-suppress unknownMacro
BENCHMARK_DEFINE_F(PerformanceTest, collector_benchmark)(benchmark::State & st)
{
  TestCollector collector;
  const size_t len = kSize;

  for (auto _ : st) {
    for (unsigned int i = 0; i < len; i++) {
      collector.AcceptData(i);
    }
  }
  st.SetComplexityN(len);

  collector.ClearCurrentMeasurements();
}
// cppcheck-suppress unknownMacro
BENCHMARK_REGISTER_F(PerformanceTest, collector_benchmark);

// cppcheck-suppress unknownMacro
BENCHMARK_DEFINE_F(PerformanceTest, moving_average_statistics_benchmark)(benchmark::State & st)
{
  MovingAverageStatistics moving_average_statistics;
  const size_t len = kSize;

  for (auto _ : st) {
    for (unsigned int i = 0; i < len; i++) {
      moving_average_statistics.AddMeasurement(i);
    }
  }
  st.SetComplexityN(len);

  moving_average_statistics.Reset();
}
// cppcheck-suppress unknownMacro
BENCHMARK_REGISTER_F(PerformanceTest, moving_average_statistics_benchmark);
