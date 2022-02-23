/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <future>

#include <bosdyn/api/auto_return/auto_return_service.grpc.pb.h>
#include <bosdyn/api/auto_return/auto_return_service.pb.h>

#include "bosdyn/client/auto_return/auto_return_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/lease/lease_client.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::auto_return::ConfigureResponse> ConfigureResultType;
typedef Result<::bosdyn::api::auto_return::GetConfigurationResponse> GetConfigurationResultType;
typedef Result<::bosdyn::api::auto_return::StartResponse> StartResultType;

// AutoReturnClient is the GRPC client for the Auto Return service defined in
// auto_return_service.proto.
class AutoReturnClient : public ServiceClient {
 public:
    AutoReturnClient() = default;

    ~AutoReturnClient() = default;

    // Asynchronous method to configure auto return.
    std::shared_future<ConfigureResultType> ConfigureAsync(
        ::bosdyn::api::auto_return::ConfigureRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to configure auto return.
    ConfigureResultType Configure(
        ::bosdyn::api::auto_return::ConfigureRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetConfigurationResultType> GetConfigurationAsync(
        ::bosdyn::api::auto_return::GetConfigurationRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request the auto return configuration.
    GetConfigurationResultType GetConfiguration(
        ::bosdyn::api::auto_return::GetConfigurationRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<StartResultType> StartAsync(
        ::bosdyn::api::auto_return::StartRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to start auto return.
    StartResultType Start(
        ::bosdyn::api::auto_return::StartRequest& request,
        const RPCParameters& parameters = RPCParameters());

    /**
     * Initialize auto return
     * new_lease - a Lease, e.g. the lease.response field of GetOwnedLease().
     * Post conditions -
     *   1) AutoReturn is configured with a version of new_lease
     *   2) If not nullptr, lease_wallet has a version of new_lease older than the version given to
     *      AutoReturn. , new_lease added to it,
     *   or
     *   1) AutoReturn is not configured and the lease_wallet is not changed.
     * Returns bosdyn::ConfigureResultType status if configuration failed,
     * SDKErrorCode::Success otherwise.
     */
    ::bosdyn::common::Status Init(const Lease& new_lease, const ::bosdyn::api::auto_return::Params& params,
                const std::string& client_name, std::shared_ptr<LeaseWallet> lease_wallet);

    // Recover after auto return has taken control.
    // Call this upon returning from comms loss and taking back control of the robot.
    ::bosdyn::common::Status Recover(std::shared_ptr<LeaseClient> lease_client, const std::string& client_name);

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(
        RequestProcessorChain& request_processor_chain,
        ResponseProcessorChain& response_processor_chain,
        const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the Auto Return service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the AutoReturn service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback that will return the ConfigureResponse message after Configure rpc
    // returns to the client.
    void OnConfigureComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::auto_return::ConfigureRequest& request,
        ::bosdyn::api::auto_return::ConfigureResponse&& response, const grpc::Status& status,
        std::promise<ConfigureResultType> promise);

    // Callback that will return the GetConfigurationResponse message after GetConfiguration rpc
    // returns to the client.
    void OnGetConfigurationComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::auto_return::GetConfigurationRequest& request,
        ::bosdyn::api::auto_return::GetConfigurationResponse&& response, const grpc::Status& status,
        std::promise<GetConfigurationResultType> promise);

    // Callback that will return the StartResponse message after Start rpc returns
    // to the client.
    void OnStartComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::auto_return::StartRequest& request,
        ::bosdyn::api::auto_return::StartResponse&& response, const grpc::Status& status,
        std::promise<StartResultType> promise);

    std::unique_ptr<::bosdyn::api::auto_return::AutoReturnService::Stub> m_stub;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Default service name for the Auto Return service.
    static const char* s_default_service_name;

    // Default service type for the AutoReturn service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
