/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

namespace {  // anonymous namespace

struct RPCErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool RPCErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success)
        return (valcode == static_cast<int>(::bosdyn::client::RPCErrorCode::Success));
    if (cond == ErrorTypeCondition::RPCError) return true;
    if (cond == RetryableRPCCondition::Retryable) {
        switch (static_cast<::bosdyn::client::RPCErrorCode>(valcode)) {
            case ::bosdyn::client::RPCErrorCode::ProxyConnectionError:
            case ::bosdyn::client::RPCErrorCode::ResponseTooLargeError:
            case ::bosdyn::client::RPCErrorCode::ServiceUnavailableError:
            case ::bosdyn::client::RPCErrorCode::ServiceFailedDuringExecutionError:
            case ::bosdyn::client::RPCErrorCode::TimedOutError:
            case ::bosdyn::client::RPCErrorCode::UnableToConnectToRobotError:
            case ::bosdyn::client::RPCErrorCode::TransientFailureError:
            case ::bosdyn::client::RPCErrorCode::TooManyRequestsError:
            case ::bosdyn::client::RPCErrorCode::RetryableUnavailableError:
                return true;
            default:
                return false;
        }
    }
    if (cond == RetryableRPCCondition::Persistent) {
        switch (static_cast<::bosdyn::client::RPCErrorCode>(valcode)) {
            case ::bosdyn::client::RPCErrorCode::ClientCancelledOperationError:
            case ::bosdyn::client::RPCErrorCode::InvalidAppTokenError:
            case ::bosdyn::client::RPCErrorCode::InvalidClientCertificateError:
            case ::bosdyn::client::RPCErrorCode::NonexistentAuthorityError:
            case ::bosdyn::client::RPCErrorCode::PermissionDeniedError:
            case ::bosdyn::client::RPCErrorCode::UnauthenticatedError:
            case ::bosdyn::client::RPCErrorCode::UnknownDnsNameError:
            case ::bosdyn::client::RPCErrorCode::UnimplementedError:
            case ::bosdyn::client::RPCErrorCode::NotFoundError:
                return true;
            default:
                return false;
        }
    }

    return false;
}

const char* RPCErrorCodeCategory::name() const noexcept { return "RPCErrorCode"; }

std::string RPCErrorCodeCategory::message(int value) const {
    switch (static_cast<::bosdyn::client::RPCErrorCode>(value)) {
        case ::bosdyn::client::RPCErrorCode::Success:
            return "Success";
        case ::bosdyn::client::RPCErrorCode::ClientCancelledOperationError:
            return "ClientCancelledOperationError";
        case ::bosdyn::client::RPCErrorCode::InvalidAppTokenError:
            return "InvalidAppTokenError";
        case ::bosdyn::client::RPCErrorCode::InvalidClientCertificateError:
            return "InvalidClientCertificateError";
        case ::bosdyn::client::RPCErrorCode::NonexistentAuthorityError:
            return "NonexistentAuthorityError";
        case ::bosdyn::client::RPCErrorCode::NotFoundError:
            return "NotFoundError";
        case ::bosdyn::client::RPCErrorCode::PermissionDeniedError:
            return "PermissionDeniedError";
        case ::bosdyn::client::RPCErrorCode::ProxyConnectionError:
            return "ProxyConnectionError";
        case ::bosdyn::client::RPCErrorCode::ResponseTooLargeError:
            return "ResponseTooLargeError";
        case ::bosdyn::client::RPCErrorCode::ServiceUnavailableError:
            return "ServiceUnavailableError";
        case ::bosdyn::client::RPCErrorCode::ServiceFailedDuringExecutionError:
            return "ServiceFailedDuringExecutionError";
        case ::bosdyn::client::RPCErrorCode::TimedOutError:
            return "TimedOutError";
        case ::bosdyn::client::RPCErrorCode::UnableToConnectToRobotError:
            return "UnableToConnectToRobotError";
        case ::bosdyn::client::RPCErrorCode::UnauthenticatedError:
            return "UnauthenticatedError";
        case ::bosdyn::client::RPCErrorCode::UnknownDnsNameError:
            return "UnknownDnsNameError";
        case ::bosdyn::client::RPCErrorCode::UnimplementedError:
            return "UnimplementedError";
        case ::bosdyn::client::RPCErrorCode::TransientFailureError:
            return "TransientFailureError";
        case ::bosdyn::client::RPCErrorCode::TooManyRequestsError:
            return "TooManyRequestsError";
        case ::bosdyn::client::RPCErrorCode::RetryableUnavailableError:
            return "RetryableUnavailableError";
    }
    return "(RPCErrorCode: unrecognized error)";
}

const RPCErrorCodeCategory RPCErrorCodeCategory_category{};

struct RetryableRPCConditionCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
};

const char* RetryableRPCConditionCategory::name() const noexcept { return "RetryableRPCCondition"; }

std::string RetryableRPCConditionCategory::message(int value) const {
    switch (static_cast<RetryableRPCCondition>(value)) {
        case RetryableRPCCondition::Retryable:
            return "Retryable";
        case RetryableRPCCondition::Persistent:
            return "Persistent";
    }
    return "(RetryableRPCCondition: unrecognized error)";
}
const RetryableRPCConditionCategory RetryableRPCConditionCategory_category{};

}  // anonymous namespace

std::error_condition make_error_condition(RetryableRPCCondition value) {
    return {static_cast<int>(value), RetryableRPCConditionCategory_category};
}

namespace bosdyn {

namespace client {

std::error_code make_error_code(RPCErrorCode value) {
    return {static_cast<int>(value), RPCErrorCodeCategory_category};
}

const std::error_category& RPCErrorCategory() { return RPCErrorCodeCategory_category; }

::bosdyn::common::Status ConvertGRPCStatus(const grpc::Status& status) {
    std::error_code err_code;
    std::string err_message;
    if (status.ok()) {
        err_code = RPCErrorCode::Success;
    } else {
        grpc::StatusCode code = status.error_code();
        grpc::string grpc_message = status.error_message();
        grpc::string details = status.error_details();

        if (code == grpc::StatusCode::CANCELLED) {
            if (details.find("401") != std::string::npos) {
                err_code = RPCErrorCode::UnauthenticatedError;
                err_message = "The user needs to authenticate to get a user token.";
            } else if (details.find("403") != std::string::npos) {
                err_code = RPCErrorCode::InvalidAppTokenError;
                err_message = "The provided app token is invalid.";
            } else if (details.find("404") != std::string::npos) {
                err_code = RPCErrorCode::NotFoundError;
                err_message = "The backend system could not be found.";
            } else if (details.find("429") != std::string::npos) {
                err_code = RPCErrorCode::TooManyRequestsError;
                err_message = "The server is not ready to handle the request due to rate limiting.";
            } else if (details.find("502") != std::string::npos) {
                err_code = RPCErrorCode::ServiceUnavailableError;
                err_message = "The proxy could not find the (possibly unregistered) service.";
            } else if (details.find("504") != std::string::npos) {
                err_code = RPCErrorCode::TimedOutError;
                err_message =
                    "The remote procedure call did not terminate within the allotted time.";
            } else {
                err_code = RPCErrorCode::ClientCancelledOperationError;
                err_message = "The user cancelled the rpc request.";
            }
        } else if (code == grpc::StatusCode::DEADLINE_EXCEEDED) {
            err_code = RPCErrorCode::TimedOutError;
            err_message = "The remote procedure call did not terminate within the allotted time.";
        } else if (code == grpc::StatusCode::UNIMPLEMENTED) {
            err_code = RPCErrorCode::UnimplementedError;
            err_message =
                "The API does not recognize the request and is unable to complete the request.";
        } else if (code == grpc::StatusCode::PERMISSION_DENIED) {
            err_code = RPCErrorCode::PermissionDeniedError;
            err_message = "The rpc request was denied access.";
        } else if (code == grpc::StatusCode::RESOURCE_EXHAUSTED &&
                   grpc_message.find("Received message larger than max")) {
            err_code = RPCErrorCode::ResponseTooLargeError;
            err_message = "The rpc response was larger than allowed max size.";
        } else if (code == grpc::StatusCode::UNAUTHENTICATED) {
            err_code = RPCErrorCode::UnauthenticatedError;
            err_message = "The user needs to authenticate to get a user token.";
        } else if (!grpc_message.empty()) {
            // The following checks are performed on debug_error_string() in Python, which does not
            // exist in C++
            if (grpc_message.find("is not in peer certificate") != std::string::npos) {
                err_code = RPCErrorCode::NonexistentAuthorityError;
                err_message = "The app token's authority field names a nonexistent service.";
            } else if (grpc_message.find("Failed to connect to remote host") != std::string::npos ||
                       grpc_message.find("Failed to create subchannel") != std::string::npos) {
                err_code = RPCErrorCode::ProxyConnectionError;
                err_message = "The proxy on the robot could not be reached.";
            } else if (grpc_message.find("Exception calling application") != std::string::npos) {
                err_code = RPCErrorCode::ServiceFailedDuringExecutionError;
                err_message = "The service encountered an unexpected failure.";
            } else if (grpc_message.find("Handshake failed") != std::string::npos) {
                err_code = RPCErrorCode::InvalidClientCertificateError;
                err_message = "The provided client certificate is invalid.";
            } else if (grpc_message.find("Name resolution failure") != std::string::npos) {
                err_code = RPCErrorCode::UnknownDnsNameError;
                err_message = "The system is unable to translate the domain name.";
            } else if (grpc_message.find("channel is in state TRANSIENT_FAILURE") !=
                       std::string::npos) {
                err_code = RPCErrorCode::TransientFailureError;
                err_message =
                    "The channel is in state TRANSIENT_FAILURE, often caused by a connection "
                    "failure.";
            } else if (grpc_message.find("Connect Failed") != std::string::npos) {
                err_code = RPCErrorCode::UnableToConnectToRobotError;
                err_message = "The robot may be offline, connect failed.";
            } else {
                // This is an unknown error case
                err_code = RPCErrorCode::UnimplementedError;
                err_message = std::to_string(code) + "|" + grpc_message + "|";
            }
        } else if (code == grpc::StatusCode::UNAVAILABLE) {
            // The following check is performed on debug_error_string() in Python, which does not
            // exist in C++.
            if (grpc_message.find("Socket closed") != std::string::npos ||
                grpc_message.find("Connection reset by peer") != std::string::npos) {
                err_code = RPCErrorCode::RetryableUnavailableError;
                err_message = "Socket closed or connection reset by peer.";
            } else if (details.find("502") != std::string::npos) {
                err_code = RPCErrorCode::ServiceUnavailableError;
                err_message = "Service is unavailable.";
            } else if (details.find("429") != std::string::npos) {
                err_code = RPCErrorCode::TooManyRequestsError;
                err_message = "The server is not ready to handle the request due to rate limiting.";
            } else {
                err_code = RPCErrorCode::UnableToConnectToRobotError;
                err_message = "The robot may be offline or unavailable";
            }
        } else {
            // This is an unknown error case
            err_code = RPCErrorCode::UnimplementedError;
            err_message = std::to_string(code) + "|" + grpc_message + "|";
        }
    }
    return ::bosdyn::common::Status(err_code, err_message);
}

}  // namespace client

}  // namespace bosdyn
