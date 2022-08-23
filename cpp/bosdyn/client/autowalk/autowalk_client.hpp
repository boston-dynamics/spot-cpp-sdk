/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/autowalk/autowalk.pb.h>
#include <bosdyn/api/autowalk/autowalk_service.grpc.pb.h>
#include <bosdyn/api/autowalk/autowalk_service.pb.h>

#include "bosdyn/client/lease/lease_resources.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/autowalk/autowalk_error_codes.h"

namespace bosdyn {

namespace client {

namespace autowalk {

typedef Result<::bosdyn::api::autowalk::CompileAutowalkResponse> CompileAutowalkResultType;
typedef Result<::bosdyn::api::autowalk::LoadAutowalkResponse> LoadAutowalkResultType;

class AutowalkClient : public ServiceClient {
 public:
    AutowalkClient() = default;

    ~AutowalkClient() = default;

    // Compiles a walk to a mission.
    CompileAutowalkResultType CompileAutowalk(
        ::bosdyn::api::autowalk::CompileAutowalkRequest& request,
        const RPCParameters& parameters = RPCParameters());
    std::shared_future<CompileAutowalkResultType> CompileAutowalkAsync(
        ::bosdyn::api::autowalk::CompileAutowalkRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Compiles a walk to a mission then uploads the mission to the robot.
    // Note: the LoadAutowalk RPC requires a lease. By default, we use a body lease, however other
    // resources (or no resources) can be provided to change which leases are attached to the
    LoadAutowalkResultType LoadAutowalk(::bosdyn::api::autowalk::LoadAutowalkRequest& request,
                                        const RPCParameters& parameters = RPCParameters(),
                                        const std::vector<std::string>& desired_lease_resources = {
                                            ::bosdyn::client::kBodyResource});
    std::shared_future<LoadAutowalkResultType> LoadAutowalkAsync(
        ::bosdyn::api::autowalk::LoadAutowalkRequest& request,
        const RPCParameters& parameters = RPCParameters(),
        const std::vector<std::string>& desired_lease_resources = {
            ::bosdyn::client::kBodyResource});

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the mission service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the mission service that will be registered in the
    // directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    void OnCompileAutowalkComplete(MessagePumpCallBase* call,
                                   const std::vector<::bosdyn::api::DataChunk>&& requests,
                                   std::vector<::bosdyn::api::DataChunk>&& responses,
                                   const grpc::Status& status,
                                   std::promise<CompileAutowalkResultType> promise);

    void OnLoadAutowalkComplete(MessagePumpCallBase* call,
                                const std::vector<::bosdyn::api::DataChunk>&& requests,
                                std::vector<::bosdyn::api::DataChunk>&& responses,
                                const grpc::Status& status,
                                std::promise<LoadAutowalkResultType> promise);

    std::unique_ptr<::bosdyn::api::autowalk::AutowalkService::Stub> m_stub;

    // Default service name for the mission service.
    static const char* s_default_service_name;

    // Default service type for the mission service.
    static const char* s_service_type;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;
};

}  // namespace autowalk

}  // namespace client

}  // namespace bosdyn