/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <system_error>

// Can these be more general than docking errors?
enum class DockingHelperErrorCode {
    // The task could not complete within the given number of attempts.
    RetriesExceeded = 1,
    // Something else cancelled the process.
    Cancelled = 2,
    // A single docking command failed.
    CommandFailed = 3,
};

namespace std {
template <>
struct is_error_code_enum<DockingHelperErrorCode> : true_type {};
}  // namespace std

std::error_code make_error_code(DockingHelperErrorCode);
