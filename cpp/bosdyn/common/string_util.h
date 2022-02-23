/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <string>

namespace bosdyn {

namespace common {

/// Return true if the end of str matches end.
bool EndsWith(const std::string& str, const std::string& end);

/// Return true if the start of str matches start.
bool StartsWith(const std::string& str, const std::string& start);

/// If str ends with suffix, return the part preceding, otherwise the original value of str.
std::string RemoveSuffix(const std::string& str, const std::string& suffix);

}  // namespace common

}  // namespace bosdyn
