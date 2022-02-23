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

enum class SDKErrorCode {
    Success = 0,
    GenericSDKError = 3,
};

std::error_code make_error_code(SDKErrorCode);

}  // namespace client

}  // namespace bosdyn

namespace std {
template <>
struct is_error_code_enum<::bosdyn::client::SDKErrorCode> : true_type {};
}  // namespace std

