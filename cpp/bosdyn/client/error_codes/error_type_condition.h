/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <system_error>

enum class ErrorTypeCondition {
    // no 0
    ResponseError = 1,
    RPCError = 2,
    SDKError = 3,
};

namespace std {
template <>
struct is_error_condition_enum<ErrorTypeCondition> : true_type {};
}  // namespace std

std::error_condition make_error_condition(ErrorTypeCondition);
