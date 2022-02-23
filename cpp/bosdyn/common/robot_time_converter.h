/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <google/protobuf/timestamp.pb.h>

#include <chrono>

/// These are utilities for use in writing services.
#include "bosdyn/client/service_client/result.h"
#include "bosdyn/common/time.h"

namespace bosdyn {

namespace common {

class RobotTimeConverter {
 public:
    explicit RobotTimeConverter(::bosdyn::common::Duration robot_clock_skew);

    ::google::protobuf::Timestamp RobotTimestampFromLocal(::bosdyn::common::TimePoint local_time);

    ::google::protobuf::Timestamp RobotTimestampFromLocalTimestamp(
        ::google::protobuf::Timestamp local_timestamp);

    ::bosdyn::common::Duration m_clock_skew;
};

typedef ::bosdyn::client::Result<std::shared_ptr<RobotTimeConverter>> RobotTimeConverterResultType;

}  // namespace common

}  // namespace bosdyn
