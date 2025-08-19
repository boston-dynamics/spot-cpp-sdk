/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <set>

#include "bosdyn/client/robot/robot.h"
#include "bosdyn/common/time.h"

namespace bosdyn::directory::util {

enum class WaitError {
    kSuccess = 0,
    kTimeout = 1,
};

struct WaitResult {
    // Normally a WaitError, but may be an RPC failure.
    bosdyn::common::Status status;
    // List of any services that were missing when a timeout was hit.
    std::set<std::string> missing_services;
};

/**
 * Block until all requested services are registered with the directory.  Useful during startup to
 * wait until the services you depend on are all running before you start creating clients.
 *
 * The status code will normally be a WaitError for success or timeout.  If it fails with a timeout,
 * the services that were not yet registered will be returned in missing_services.  If the calls to
 * list the services fail in an un-retryable manner, that error code will be returned in the status
 * instead.
 */
WaitResult WaitForAllServices(
    const std::set<std::string>& service_names, bosdyn::client::Robot& robot,
    const bosdyn::common::Duration& timeout,
    const bosdyn::common::Duration& interval = std::chrono::milliseconds(100));

std::error_code make_error_code(WaitError value);

}  // namespace bosdyn::directory::util

namespace std {
template <>
struct is_error_code_enum<bosdyn::directory::util::WaitError> : true_type {};
}  // namespace std
