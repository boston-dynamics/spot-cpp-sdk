/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "robot_command_client.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/lease/lease_resources.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* RobotCommandClient::s_default_service_name = "robot-command";

const char* RobotCommandClient::s_service_type = "bosdyn.api.RobotCommandService";

std::shared_future<RobotCommandResultType> RobotCommandClient::RobotCommandAsync(
    ::bosdyn::api::RobotCommandRequest& request,
    const RPCParameters& parameters) {
    std::promise<RobotCommandResultType> response;
    std::shared_future<RobotCommandResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ::bosdyn::client::ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                                  ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RobotCommandRequest, ::bosdyn::api::RobotCommandResponse,
                          ::bosdyn::api::RobotCommandResponse>(
            request,
            std::bind(&::bosdyn::api::RobotCommandService::Stub::AsyncRobotCommand, m_stub.get(), _1, _2, _3),
            std::bind(&RobotCommandClient::OnRobotCommandComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

std::shared_future<RobotCommandResultType> RobotCommandClient::RobotCommandAsync(
    ::bosdyn::api::RobotCommand& command, Lease* lease, TimeSyncEndpoint* time_sync_endpoint,
    ::bosdyn::common::TimePoint end_time, const RPCParameters& parameters) {
    std::promise<RobotCommandResultType> response;
    std::shared_future<RobotCommandResultType> future = response.get_future();

    ::bosdyn::api::RobotCommandRequest request;
    request.mutable_command()->CopyFrom(command);
    // Note, if a lease wasn't provided, the processors should apply one automatically using the
    // lease wallet. Otherwise, an error will be thrown.
    if (lease) {
        request.mutable_lease()->CopyFrom(lease->GetProto());
    }

    if (time_sync_endpoint) {
        if (end_time != ::bosdyn::common::TimePoint(::bosdyn::common::Duration(0))) {
            // Update the end time to robot time and apply it to the command.
            ::bosdyn::common::RobotTimeConverter time_converter = time_sync_endpoint->GetRobotTimeConverter();
            auto cmd_timestamp = time_converter.RobotTimestampFromLocal(end_time);
            MutateEndTime(request.mutable_command(), cmd_timestamp);
        }
        // Set the clock identifier for the timesync endpoint.
        auto clock_string = time_sync_endpoint->GetClockIdentifier();
        if (!clock_string.status) {
            response.set_value({clock_string.status, {}});
            return future;
        }
        request.set_clock_identifier(*clock_string.response);
    } else if (m_time_sync_endpoint) {
        if (end_time != ::bosdyn::common::TimePoint(::bosdyn::common::Duration(0))) {
            // Update the end time to robot time and apply it to the command.
            ::bosdyn::common::RobotTimeConverter time_converter = m_time_sync_endpoint->GetRobotTimeConverter();
            auto cmd_timestamp = time_converter.RobotTimestampFromLocal(end_time);
            MutateEndTime(request.mutable_command(), cmd_timestamp);
        }
        // Set the clock identifier to the robot object's timesync endpoint.
        auto clock_string = m_time_sync_endpoint->GetClockIdentifier();
        if (!clock_string.status) {
            response.set_value({clock_string.status, {}});
            return future;
        }
        request.set_clock_identifier(*clock_string.response);
    } else {
        // Unable to set the clock identifier for the robot command.
        response.set_value({::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                   "Timesync endpoint is unset for the RobotCommand Client."), {}});
        return future;
    }

    // Call the base robot command RPC.
    return RobotCommandAsync(request, parameters);
}

void RobotCommandClient::MutateEndTime(
    ::bosdyn::api::MobilityCommand::Request* mobility_command,
    const google::protobuf::Timestamp& end_time) {
    if (mobility_command->has_se2_velocity_request()) {
        // Velocity request has an endtime.
        mobility_command->mutable_se2_velocity_request()->mutable_end_time()->CopyFrom(end_time);
    }
    if (mobility_command->has_se2_trajectory_request()) {
        // Trajectory request has an endtime.
        mobility_command->mutable_se2_trajectory_request()->mutable_end_time()->CopyFrom(end_time);
    }
}

void RobotCommandClient::MutateEndTime(
    ::bosdyn::api::RobotCommand* robot_command, const google::protobuf::Timestamp& end_time) {
    if (robot_command->has_synchronized_command()) {
        if (robot_command->synchronized_command().has_mobility_command()) {
            MutateEndTime(robot_command->mutable_synchronized_command()->mutable_mobility_command(),
                          end_time);
        }
    } else if (robot_command->has_mobility_command()) {
        MutateEndTime(robot_command->mutable_mobility_command(), end_time);
    }
}

void RobotCommandClient::OnRobotCommandComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RobotCommandRequest& request,
    ::bosdyn::api::RobotCommandResponse&& response, const grpc::Status& status,
    std::promise<RobotCommandResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::RobotCommandResponse>(
        status, response, response.status(), m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

RobotCommandResultType RobotCommandClient::RobotCommand(
    ::bosdyn::api::RobotCommandRequest& request, const RPCParameters& parameters) {
    return RobotCommandAsync(request, parameters).get();
}

RobotCommandResultType RobotCommandClient::RobotCommand(
    ::bosdyn::api::RobotCommand& command, Lease* lease, TimeSyncEndpoint* time_sync_endpoint,
    ::bosdyn::common::TimePoint end_time, const RPCParameters& parameters) {
    return RobotCommandAsync(command, lease, time_sync_endpoint, end_time, parameters).get();
}

std::shared_future<RobotCommandFeedbackResultType> RobotCommandClient::RobotCommandFeedbackAsync(
    ::bosdyn::api::RobotCommandFeedbackRequest& request,
    const RPCParameters& parameters) {
    std::promise<RobotCommandFeedbackResultType> response;
    std::shared_future<RobotCommandFeedbackResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RobotCommandFeedbackRequest, ::bosdyn::api::RobotCommandFeedbackResponse,
                          ::bosdyn::api::RobotCommandFeedbackResponse>(
            request,
            std::bind(&::bosdyn::api::RobotCommandService::Stub::AsyncRobotCommandFeedback, m_stub.get(), _1,
                      _2, _3),
            std::bind(&RobotCommandClient::OnRobotCommandFeedbackComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

void RobotCommandClient::OnRobotCommandFeedbackComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RobotCommandFeedbackRequest& request,
    ::bosdyn::api::RobotCommandFeedbackResponse&& response, const grpc::Status& status,
    std::promise<RobotCommandFeedbackResultType> promise) {
    // Feedback indicates the command conditions and does not represent an error code. As
    // such, the response.status will always be a success, regardless of the feedback status value.
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RobotCommandFeedbackResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

RobotCommandFeedbackResultType RobotCommandClient::RobotCommandFeedback(
    ::bosdyn::api::RobotCommandFeedbackRequest& request,
    const RPCParameters& parameters) {
    return RobotCommandFeedbackAsync(request, parameters).get();
}

std::shared_future<ClearBehaviorFaultResultType> RobotCommandClient::ClearBehaviorFaultAsync(
    ::bosdyn::api::ClearBehaviorFaultRequest& request,
    const RPCParameters& parameters) {
    std::promise<ClearBehaviorFaultResultType> response;
    std::shared_future<ClearBehaviorFaultResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = bosdyn::client::ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ClearBehaviorFaultRequest, ::bosdyn::api::ClearBehaviorFaultResponse,
                          ::bosdyn::api::ClearBehaviorFaultResponse>(
            request,
            std::bind(&::bosdyn::api::RobotCommandService::Stub::AsyncClearBehaviorFault, m_stub.get(), _1,
                      _2, _3),
            std::bind(&RobotCommandClient::OnClearBehaviorFaultComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void RobotCommandClient::OnClearBehaviorFaultComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ClearBehaviorFaultRequest& request,
    ::bosdyn::api::ClearBehaviorFaultResponse&& response, const grpc::Status& status,
    std::promise<ClearBehaviorFaultResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::ClearBehaviorFaultResponse>(
        status, response, response.status(), m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

ClearBehaviorFaultResultType RobotCommandClient::ClearBehaviorFault(
    ::bosdyn::api::ClearBehaviorFaultRequest& request,
    const RPCParameters& parameters) {
    return ClearBehaviorFaultAsync(request, parameters).get();
}

ServiceClient::QualityOfService RobotCommandClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void RobotCommandClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::RobotCommandService::Stub(channel));
}

void RobotCommandClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                           ResponseProcessorChain& response_processor_chain,
                                           const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}

}  // namespace client

}  // namespace bosdyn
