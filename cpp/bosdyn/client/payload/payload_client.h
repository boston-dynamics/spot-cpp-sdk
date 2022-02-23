/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/payload_service.pb.h>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

#include <bosdyn/api/payload.pb.h>
#include <bosdyn/api/payload_service.grpc.pb.h>
#include <future>

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::ListPayloadsResponse> ListPayloadsResultType;

class PayloadClient : public ServiceClient {
 public:
    PayloadClient() = default;

    ~PayloadClient() = default;

    // Asynchronous method to list payloads.
    std::shared_future<ListPayloadsResultType> ListPayloadsAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to list payloads.
    ListPayloadsResultType ListPayloads(
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the Payload service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the Payload service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    void OnListPayloadsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListPayloadsRequest& request,
        ::bosdyn::api::ListPayloadsResponse&& response, const grpc::Status& status,
        std::promise<ListPayloadsResultType> promise);

    std::unique_ptr<::bosdyn::api::PayloadService::Stub> m_stub;

    // Default service name for the Payload service.
    static const char* s_default_service_name;

    // Default service type for the Payload service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
