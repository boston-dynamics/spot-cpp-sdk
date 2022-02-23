/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <cstdint>

namespace bosdyn {

namespace common {

constexpr int64_t kThousand = 1000;
constexpr int64_t kMillion = kThousand * kThousand;
constexpr int64_t kBillion = kMillion * kThousand;
constexpr int64_t kTrillion = kBillion * kThousand;

constexpr int64_t kBytesPerKB = 1024;
constexpr int64_t kBytesPerMB = 1024 * kBytesPerKB;
constexpr int64_t kBytesPerGB = 1024 * kBytesPerMB;

constexpr int kSecondsPerMinute = 60;
constexpr int kMinutesPerHour = 60;
constexpr int kHoursPerDay = 24;
constexpr int kDaysPerWeek = 7;

constexpr int64_t kMsecInSecond = kThousand;
constexpr int64_t kUsecInSecond = kMillion;
constexpr int64_t kNsecInSecond = kBillion;

constexpr int kSecondsPerHour = kSecondsPerMinute * kMinutesPerHour;
constexpr int kSecondsPerDay = kSecondsPerHour * kHoursPerDay;
constexpr int kSecondsPerWeek = kSecondsPerDay * kDaysPerWeek;

}  // namespace common

}  // namespace bosdyn
