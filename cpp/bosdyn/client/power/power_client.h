/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/power_service.grpc.pb.h>
#include <bosdyn/api/power_service.pb.h>

#include <future>
#include <string>

#include "bosdyn/client/power/power_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

// Return type for the PowerCommand method.
typedef Result<::bosdyn::api::PowerCommandResponse> PowerCommandResultType;
// Return type for the PowerCommandFeedback method.
typedef Result<::bosdyn::api::PowerCommandFeedbackResponse> PowerCommandFeedbackResultType;
// Return type for the FanPowerCommand method
typedef Result<::bosdyn::api::FanPowerCommandResponse> FanPowerCommandResultType;
// Return type for the FanPowerCommandFeedback method
typedef Result<::bosdyn::api::FanPowerCommandFeedbackResponse> FanPowerCommandFeedbackResultType;

class PowerClient : public ServiceClient {
 public:
    // Constructor for the Power client, which can be used to make RPC requests for power commands.
    PowerClient() = default;

    // Destructor for the Power client.
    ~PowerClient() = default;

    // Power Commands

    // Asynchronous method to execute a power command.
    std::shared_future<PowerCommandResultType> PowerCommandAsync(
        ::bosdyn::api::PowerCommandRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a power command.
    PowerCommandResultType PowerCommand(::bosdyn::api::PowerCommandRequest& request,
                                        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request power command feedback. The status field in the return object
    // does not incorporate the value of the status field in the protobuf response object because
    // the status of a feedback method is not considered an error.
    std::shared_future<PowerCommandFeedbackResultType> PowerCommandFeedbackAsync(
        ::bosdyn::api::PowerCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    PowerCommandFeedbackResultType PowerCommandFeedback(
        ::bosdyn::api::PowerCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<PowerCommandFeedbackResultType> PowerCommandFeedbackAsync(
        unsigned int id, const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request power command feedback. The status field in the return object
    // does not incorporate the value of the status field in the protobuf response object because
    // the status of a feedback method is not considered an error.
    PowerCommandFeedbackResultType PowerCommandFeedback(
        unsigned int id, const RPCParameters& parameters = RPCParameters());

    // Fan Power Commands

    // Asynchronous method to execute a fan power command.
    std::shared_future<FanPowerCommandResultType> FanPowerCommandAsync(
        ::bosdyn::api::FanPowerCommandRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a fan power command.
    FanPowerCommandResultType FanPowerCommand(::bosdyn::api::FanPowerCommandRequest& request,
                                              const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request fan power command feedback. The status field in the return
    // object does not incorporate the value of the status field in the protobuf response object
    // because the status of a feedback method is not considered an error.
    std::shared_future<FanPowerCommandFeedbackResultType> FanPowerCommandFeedbackAsync(
        ::bosdyn::api::FanPowerCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<FanPowerCommandFeedbackResultType> FanPowerCommandFeedbackAsync(
        unsigned int id, const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request fan power command feedback. The status field in the return
    // object does not incorporate the value of the status field in the protobuf response object
    // because the status of a feedback method is not considered an error.
    FanPowerCommandFeedbackResultType FanPowerCommandFeedback(
        ::bosdyn::api::FanPowerCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    FanPowerCommandFeedbackResultType FanPowerCommandFeedback(
        unsigned int id, const RPCParameters& parameters = RPCParameters());

    // Power Client Commands

    // Sets the QualityOfService enum for the power client to be used for network selection
    // optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the power service, including the stub and processors
    // for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;

    // Get the default service name the Power service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the Power service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // The stub for the power client to communicate it's rpc with (and pass to the robot).
    std::unique_ptr<::bosdyn::api::PowerService::Stub> m_stub;

    // Default service name for the power service.
    static const char* s_default_service_name;

    // Callback that will return the PowerCommandResponse message after PowerCommand rpc returns to
    // the client.
    void OnPowerCommandComplete(MessagePumpCallBase* call,
                                const ::bosdyn::api::PowerCommandRequest& request,
                                ::bosdyn::api::PowerCommandResponse&& response,
                                const grpc::Status& status,
                                std::promise<PowerCommandResultType> promise);

    // Callback that will return the PowerCommandResponse message after PowerCommand rpc returns to
    // the client.
    void OnPowerCommandFeedbackComplete(MessagePumpCallBase* call,
                                        const ::bosdyn::api::PowerCommandFeedbackRequest& request,
                                        ::bosdyn::api::PowerCommandFeedbackResponse&& response,
                                        const grpc::Status& status,
                                        std::promise<PowerCommandFeedbackResultType> promise);

    // Callback that will return the FanPowerCommandResponse message after FanPowerCommand rpc
    // returns to the client.
    void OnFanPowerCommandComplete(MessagePumpCallBase* call,
                                   const ::bosdyn::api::FanPowerCommandRequest& request,
                                   ::bosdyn::api::FanPowerCommandResponse&& response,
                                   const grpc::Status& status,
                                   std::promise<FanPowerCommandResultType> promise);

    // Callback that will return the FanPowerCommandFeedbackResponse message after the
    // FanPowerCommandFeedback rpc returns to the client.
    void OnFanPowerCommandFeedbackComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::FanPowerCommandFeedbackRequest& request,
        ::bosdyn::api::FanPowerCommandFeedbackResponse&& response, const grpc::Status& status,
        std::promise<FanPowerCommandFeedbackResultType> promise);

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Default service type for the Power service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
