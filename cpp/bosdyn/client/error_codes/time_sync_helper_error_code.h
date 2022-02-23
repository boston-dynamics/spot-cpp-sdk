/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <system_error>

namespace bosdyn {

namespace client {

enum class TimeSyncHelperErrorCode {
    ClockIdentifierUnset = 1,
};

enum class EstablishTimeSyncErrorCode {
    UnableToEstablishTimeSync = 1,
};

enum class RobotTimeSyncErrorCode {
    PreviousTimeSyncUnavailableYet = 1,
};

std::error_code make_error_code(TimeSyncHelperErrorCode);
std::error_code make_error_code(EstablishTimeSyncErrorCode);
std::error_code make_error_code(RobotTimeSyncErrorCode);

}  // client namespace
}  // bosdyn namespace

namespace std {
template <>
struct is_error_code_enum<bosdyn::client::TimeSyncHelperErrorCode> : true_type {};
template <>
struct is_error_code_enum<bosdyn::client::EstablishTimeSyncErrorCode> : true_type {};
template <>
struct is_error_code_enum<bosdyn::client::RobotTimeSyncErrorCode> : true_type {};
}  // namespace std
