# libstatistics_collector

![License](https://img.shields.io/github/license/ros-tooling/libstatistics_collector)
[![Test libstatistics_collector](https://github.com/ros-tooling/libstatistics_collector/workflows/Test%20libstatistics_collector/badge.svg)]
## Description

This C++ library provides the following:

- A `Collector` interface for implementing classes that collect observed data
 and generate statistics for them
- A `TopicStatisticsCollector` interface for implementing classes that
 collect and perform measurements for ROS2 topic statistics.
 Classes for calculating ROS 2 message age and message period statistics are
 also implemented.
- A `MovingAverageStatistics` class for calculating moving average statistics
