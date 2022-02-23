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

enum class LeaseWalletErrorCode {
    Success = 0,
    ResourceNotInWalletError = 1,
    ResourceNotOwnedError = 2,
    LeaseInvalidError = 3,
    GenericLeaseError = 4
};

const std::error_category& LeaseWalletErrorCategory();
std::error_code make_error_code(LeaseWalletErrorCode);

}  // namespace client

}  // namespace bosdyn

namespace std {
template <>
struct is_error_code_enum<bosdyn::client::LeaseWalletErrorCode> : true_type {};
}  // namespace std

