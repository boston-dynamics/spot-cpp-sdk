/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "string_util.h"

#include <string>

namespace bosdyn {

namespace common {

bool EndsWith(const std::string& str, const std::string& end) {
    const int str_len = str.length();
    const int end_len = end.length();
    return end_len <= str_len && str.substr(str_len - end_len) == end;
}

bool StartsWith(const std::string& str, const std::string& start) {
    const int str_len = str.length();
    const int start_len = start.length();
    return start_len <= str_len && str.substr(0, start_len) == start;
}

/// If str ends with suffix, return the part preceding, otherwise the original value of str.
std::string RemoveSuffix(const std::string& str, const std::string& suffix) {
    return EndsWith(str, suffix) ? str.substr(0, str.length() - suffix.length()) : str;
}

}  // namespace common

}  // namespace bosdyn
