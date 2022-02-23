/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/data_acquisition_service.grpc.pb.h>
#include <bosdyn/api/data_acquisition_service.pb.h>
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/data_acquisition/data_acquisition_error_codes.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::AcquireDataResponse> DataAcquisitionAcquireDataResultType;
typedef Result<::bosdyn::api::GetStatusResponse> DataAcquisitionGetStatusResultType;
typedef Result<::bosdyn::api::GetServiceInfoResponse> DataAcquisitionServiceInfoResultType;
typedef Result<::bosdyn::api::CancelAcquisitionResponse> DataAcquisitionCancelAcquisitionResultType;

class DataAcquisitionClient : public ServiceClient {
 public:
    DataAcquisitionClient() = default;

    ~DataAcquisitionClient() = default;

    // Asynchronous RPC to trigger data acquisition to save data and metadata to the data buffer.
    std::shared_future<DataAcquisitionAcquireDataResultType> AcquireDataAsync(
        ::bosdyn::api::AcquireDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition to save data and metadata to the data buffer.
    DataAcquisitionAcquireDataResultType AcquireData(
        ::bosdyn::api::AcquireDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to query the status of a data acquisition.
    std::shared_future<DataAcquisitionGetStatusResultType> GetStatusAsync(
        ::bosdyn::api::GetStatusRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to query the status of a data acquisition.
    DataAcquisitionGetStatusResultType GetStatus(
        ::bosdyn::api::GetStatusRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to get service info from the data acquisition service.
    std::shared_future<DataAcquisitionServiceInfoResultType> GetServiceInfoAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to get service info from the data acquisition service.
    DataAcquisitionServiceInfoResultType GetServiceInfo(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to cancel an in-progress data acquisition.
    std::shared_future<DataAcquisitionCancelAcquisitionResultType> CancelAcquisitionAsync(
        ::bosdyn::api::CancelAcquisitionRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to cancel an in-progress data acquisition.
    DataAcquisitionCancelAcquisitionResultType CancelAcquisition(
        ::bosdyn::api::CancelAcquisitionRequest& request, const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the image service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

private:
    // Callback function registered for the asynchronous calls to acquire data.
    void OnAcquireDataComplete(MessagePumpCallBase* call, const ::bosdyn::api::AcquireDataRequest& request,
                               ::bosdyn::api::AcquireDataResponse&& response, const grpc::Status& status,
                               std::promise<DataAcquisitionAcquireDataResultType> promise);

    // Callback function registered for the asynchronous calls to get status.
    void OnGetStatusComplete(MessagePumpCallBase* call, const ::bosdyn::api::GetStatusRequest& request,
                             ::bosdyn::api::GetStatusResponse&& response, const grpc::Status& status,
                             std::promise<DataAcquisitionGetStatusResultType> promise);

    // Callback function registered for the asynchronous calls to get service info.
    void OnGetServiceInfoComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::GetServiceInfoRequest& request,
                                  ::bosdyn::api::GetServiceInfoResponse&& response, const grpc::Status& status,
                                  std::promise<DataAcquisitionServiceInfoResultType> promise);

    // Callback function registered for the asynchronous calls to cancel acquisition.
    void OnCancelAcquisitionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::CancelAcquisitionRequest& request,
        ::bosdyn::api::CancelAcquisitionResponse&& response, const grpc::Status& status,
        std::promise<DataAcquisitionCancelAcquisitionResultType> promise);

    std::unique_ptr<::bosdyn::api::DataAcquisitionService::Stub> m_stub;

    // Default service name for the Data Acquisition service.
    static const char* s_default_service_name;

    // Default service type for the data acquisition service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
