/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <grpcpp/grpcpp.h>
#include <system_error>
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

enum class RPCErrorCode {
    Success = 0,
    ClientCancelledOperationError = 1,
    InvalidAppTokenError = 2,
    InvalidClientCertificateError = 3,
    NonexistentAuthorityError = 4,
    PermissionDeniedError = 5,
    ProxyConnectionError = 6,
    ResponseTooLargeError = 7,
    ServiceUnavailableError = 8,
    ServiceFailedDuringExecutionError = 9,
    TimedOutError = 10,
    UnableToConnectToRobotError = 11,
    UnauthenticatedError = 12,
    UnknownDnsNameError = 13,
    UnimplementedError = 14,
    TransientFailureError = 15,
    TooManyRequestsError = 16,
    NotFoundError = 17,
    RetryableUnavailableError = 18,
};

const std::error_category& RPCErrorCategory();

std::error_code make_error_code(RPCErrorCode);

::bosdyn::common::Status ConvertGRPCStatus(const grpc::Status& status);

}  // namespace client
}  // namespace bosdyn


enum class RetryableRPCCondition {
    // no 0
    Retryable = 1,
    Persistent = 2,
};


namespace std {
template <>
struct is_error_code_enum<::bosdyn::client::RPCErrorCode> : true_type {};

template <>
struct is_error_condition_enum<RetryableRPCCondition> : true_type {};
}  // namespace std


std::error_condition make_error_condition(RetryableRPCCondition);
