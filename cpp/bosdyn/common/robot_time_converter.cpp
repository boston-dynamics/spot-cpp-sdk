/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "robot_time_converter.h"

#include <chrono>
#include <string>

#include "bosdyn/common/time.h"

namespace bosdyn {

namespace common {

RobotTimeConverter::RobotTimeConverter(::bosdyn::common::Duration robot_clock_skew) {
    m_clock_skew = robot_clock_skew;
}

::google::protobuf::Timestamp RobotTimeConverter::RobotTimestampFromLocal(
    ::bosdyn::common::TimePoint local_time) {
    ::google::protobuf::Timestamp timestamp;
    SetTimestamp(local_time.time_since_epoch() + m_clock_skew, &timestamp);
    return timestamp;
}

::google::protobuf::Timestamp RobotTimeConverter::RobotTimestampFromLocalTimestamp(
    ::google::protobuf::Timestamp local_timestamp) {
    ::bosdyn::common::TimePoint tp(::bosdyn::common::Duration(TimestampToNsec(local_timestamp)));
    return RobotTimestampFromLocal(tp);
}

}  // namespace common

}  // namespace bosdyn
