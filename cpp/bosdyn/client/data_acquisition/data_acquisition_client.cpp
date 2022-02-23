/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/data_acquisition/data_acquisition_client.h"

using namespace std::placeholders;

namespace {

auto constexpr kGenericStubError = "Stub is not set in Data Acquisition Client";

}

namespace bosdyn {

namespace client {

const char* DataAcquisitionClient::s_default_service_name = "data-acquisition";

const char* DataAcquisitionClient::s_service_type = "bosdyn.api.DataAcquisitionService";

// RPC to trigger data acquisition to save data and metadata to the data buffer.
std::shared_future<DataAcquisitionAcquireDataResultType> DataAcquisitionClient::AcquireDataAsync(
    ::bosdyn::api::AcquireDataRequest& request, const RPCParameters& parameters) {
    std::promise<DataAcquisitionAcquireDataResultType> response;
    std::shared_future<DataAcquisitionAcquireDataResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::AcquireDataRequest, ::bosdyn::api::AcquireDataResponse,
                          ::bosdyn::api::AcquireDataResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionService::Stub::AsyncAcquireData, m_stub.get(), _1, _2,
                      _3),
            std::bind(&DataAcquisitionClient::OnAcquireDataComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionAcquireDataResultType DataAcquisitionClient::AcquireData(
    ::bosdyn::api::AcquireDataRequest& request, const RPCParameters& parameters) {
    return AcquireDataAsync(request, parameters).get();
}

void DataAcquisitionClient::OnAcquireDataComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::AcquireDataRequest& request,
    ::bosdyn::api::AcquireDataResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionAcquireDataResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::AcquireDataResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

// RPC to query the status of a data acquisition.
std::shared_future<DataAcquisitionGetStatusResultType> DataAcquisitionClient::GetStatusAsync(
    ::bosdyn::api::GetStatusRequest& request, const RPCParameters& parameters) {
    std::promise<DataAcquisitionGetStatusResultType> response;
    std::shared_future<DataAcquisitionGetStatusResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::GetStatusRequest, ::bosdyn::api::GetStatusResponse,
                                                      ::bosdyn::api::GetStatusResponse>(
        request,
        std::bind(&::bosdyn::api::DataAcquisitionService::Stub::AsyncGetStatus, m_stub.get(), _1, _2, _3),
        std::bind(&DataAcquisitionClient::OnGetStatusComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

DataAcquisitionGetStatusResultType DataAcquisitionClient::GetStatus(
    ::bosdyn::api::GetStatusRequest& request, const RPCParameters& parameters) {
    return GetStatusAsync(request, parameters).get();
}

void DataAcquisitionClient::OnGetStatusComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetStatusRequest& request,
    ::bosdyn::api::GetStatusResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionGetStatusResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetStatusResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to get service info from the data acquisition service.
std::shared_future<DataAcquisitionServiceInfoResultType>
DataAcquisitionClient::GetServiceInfoAsync(const RPCParameters& parameters) {
    std::promise<DataAcquisitionServiceInfoResultType> response;
    std::shared_future<DataAcquisitionServiceInfoResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetServiceInfoRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetServiceInfoRequest, ::bosdyn::api::GetServiceInfoResponse,
                          ::bosdyn::api::GetServiceInfoResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionService::Stub::AsyncGetServiceInfo, m_stub.get(), _1, _2,
                      _3),
            std::bind(&DataAcquisitionClient::OnGetServiceInfoComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionServiceInfoResultType DataAcquisitionClient::GetServiceInfo(
    const RPCParameters& parameters) {
    return GetServiceInfoAsync(parameters).get();
}

void DataAcquisitionClient::OnGetServiceInfoComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetServiceInfoRequest& request,
    ::bosdyn::api::GetServiceInfoResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionServiceInfoResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetServiceInfoResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to cancel an in-progress data acquisition.
std::shared_future<DataAcquisitionCancelAcquisitionResultType>
DataAcquisitionClient::CancelAcquisitionAsync(::bosdyn::api::CancelAcquisitionRequest& request,
                                              const RPCParameters& parameters) {
    std::promise<DataAcquisitionCancelAcquisitionResultType> response;
    std::shared_future<DataAcquisitionCancelAcquisitionResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::CancelAcquisitionRequest, ::bosdyn::api::CancelAcquisitionResponse,
                          ::bosdyn::api::CancelAcquisitionResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionService::Stub::AsyncCancelAcquisition, m_stub.get(), _1,
                      _2, _3),
            std::bind(&DataAcquisitionClient::OnCancelAcquisitionComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionCancelAcquisitionResultType DataAcquisitionClient::CancelAcquisition(
    ::bosdyn::api::CancelAcquisitionRequest& request, const RPCParameters& parameters) {
    return CancelAcquisitionAsync(request, parameters).get();
}

void DataAcquisitionClient::OnCancelAcquisitionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::CancelAcquisitionRequest& request,
    ::bosdyn::api::CancelAcquisitionResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionCancelAcquisitionResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::CancelAcquisitionResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService DataAcquisitionClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void DataAcquisitionClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::DataAcquisitionService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
