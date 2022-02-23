/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/data_acquisition_store/data_acquisition_store_client.h"

using namespace std::placeholders;

namespace {

auto constexpr kGenericStubError = "Stub is not set in Data Acquisition Store Client";

}

namespace bosdyn {

namespace client {

const char* DataAcquisitionStoreClient::s_default_service_name = "data-acquisition-store";

const char* DataAcquisitionStoreClient::s_service_type = "bosdyn.api.DataAcquisitionStoreService";

// RPC to trigger data acquisition store to list capture actions.
std::shared_future<DataAcquisitionStoreListCaptureActionsResultType>
DataAcquisitionStoreClient::ListCaptureActionsAsync(::bosdyn::api::ListCaptureActionsRequest& request,
                                                    const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreListCaptureActionsResultType> response;
    std::shared_future<DataAcquisitionStoreListCaptureActionsResultType> future =
        response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListCaptureActionsRequest, ::bosdyn::api::ListCaptureActionsResponse,
                          ::bosdyn::api::ListCaptureActionsResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncListCaptureActions,
                      m_stub.get(), _1, _2, _3),
            std::bind(&DataAcquisitionStoreClient::OnListCaptureActionsComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionStoreListCaptureActionsResultType DataAcquisitionStoreClient::ListCaptureActions(
    ::bosdyn::api::ListCaptureActionsRequest& request, const RPCParameters& parameters) {
    return ListCaptureActionsAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnListCaptureActionsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListCaptureActionsRequest& request,
    ::bosdyn::api::ListCaptureActionsResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreListCaptureActionsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListCaptureActionsResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to trigger data acquisition store to list stored data.
std::shared_future<DataAcquisitionStoreListStoredDataResultType>
DataAcquisitionStoreClient::ListStoredDataAsync(::bosdyn::api::ListStoredDataRequest& request,
                                                const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreListStoredDataResultType> response;
    std::shared_future<DataAcquisitionStoreListStoredDataResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<
        ::bosdyn::api::ListStoredDataRequest, ::bosdyn::api::ListStoredDataResponse, ::bosdyn::api::ListStoredDataResponse>(
        request,
        std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncListStoredData, m_stub.get(), _1,
                  _2, _3),
        std::bind(&DataAcquisitionStoreClient::OnListStoredDataComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

DataAcquisitionStoreListStoredDataResultType DataAcquisitionStoreClient::ListStoredData(
    ::bosdyn::api::ListStoredDataRequest& request, const RPCParameters& parameters) {
    return ListStoredDataAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnListStoredDataComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListStoredDataRequest& request,
    ::bosdyn::api::ListStoredDataResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreListStoredDataResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListStoredDataResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to trigger data acquisition store to store data.
std::shared_future<DataAcquisitionStoreStoreDataResultType>
DataAcquisitionStoreClient::StoreDataAsync(::bosdyn::api::StoreDataRequest& request,
                                           const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreStoreDataResultType> response;
    std::shared_future<DataAcquisitionStoreStoreDataResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::StoreDataRequest, ::bosdyn::api::StoreDataResponse, ::bosdyn::api::StoreDataResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncStoreData, m_stub.get(), _1, _2,
                      _3),
            std::bind(&DataAcquisitionStoreClient::OnStoreDataComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionStoreStoreDataResultType DataAcquisitionStoreClient::StoreData(
    ::bosdyn::api::StoreDataRequest& request, const RPCParameters& parameters) {
    return StoreDataAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnStoreDataComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::StoreDataRequest& request,
    ::bosdyn::api::StoreDataResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreStoreDataResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::StoreDataResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to trigger data acquisition store to list stored images.
std::shared_future<DataAcquisitionStoreListStoredImagesResultType>
DataAcquisitionStoreClient::ListStoredImagesAsync(::bosdyn::api::ListStoredImagesRequest& request,
                                                  const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreListStoredImagesResultType> response;
    std::shared_future<DataAcquisitionStoreListStoredImagesResultType> future =
        response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListStoredImagesRequest, ::bosdyn::api::ListStoredImagesResponse,
                          ::bosdyn::api::ListStoredImagesResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncListStoredImages, m_stub.get(),
                      _1, _2, _3),
            std::bind(&DataAcquisitionStoreClient::OnListStoredImagesComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionStoreListStoredImagesResultType DataAcquisitionStoreClient::ListStoredImages(
    ::bosdyn::api::ListStoredImagesRequest& request, const RPCParameters& parameters) {
    return ListStoredImagesAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnListStoredImagesComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListStoredImagesRequest& request,
    ::bosdyn::api::ListStoredImagesResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreListStoredImagesResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListStoredImagesResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to trigger data acquisition store to store image.
std::shared_future<DataAcquisitionStoreStoreImageResultType>
DataAcquisitionStoreClient::StoreImageAsync(::bosdyn::api::StoreImageRequest& request,
                                            const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreStoreImageResultType> response;
    std::shared_future<DataAcquisitionStoreStoreImageResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::StoreImageRequest, ::bosdyn::api::StoreImageResponse, ::bosdyn::api::StoreImageResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncStoreImage, m_stub.get(), _1,
                      _2, _3),
            std::bind(&DataAcquisitionStoreClient::OnStoreImageComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionStoreStoreImageResultType DataAcquisitionStoreClient::StoreImage(
    ::bosdyn::api::StoreImageRequest& request, const RPCParameters& parameters) {
    return StoreImageAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnStoreImageComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::StoreImageRequest& request,
    ::bosdyn::api::StoreImageResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreStoreImageResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::StoreImageResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to trigger data acquisition store to list stored metadata.
std::shared_future<DataAcquisitionStoreListStoredMetadataResultType>
DataAcquisitionStoreClient::ListStoredMetadataAsync(::bosdyn::api::ListStoredMetadataRequest& request,
                                                    const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreListStoredMetadataResultType> response;
    std::shared_future<DataAcquisitionStoreListStoredMetadataResultType> future =
        response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListStoredMetadataRequest, ::bosdyn::api::ListStoredMetadataResponse,
                          ::bosdyn::api::ListStoredMetadataResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncListStoredMetadata,
                      m_stub.get(), _1, _2, _3),
            std::bind(&DataAcquisitionStoreClient::OnListStoredMetadataComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionStoreListStoredMetadataResultType DataAcquisitionStoreClient::ListStoredMetadata(
    ::bosdyn::api::ListStoredMetadataRequest& request, const RPCParameters& parameters) {
    return ListStoredMetadataAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnListStoredMetadataComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListStoredMetadataRequest& request,
    ::bosdyn::api::ListStoredMetadataResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreListStoredMetadataResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListStoredMetadataResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to trigger data acquisition store to store metadata.
std::shared_future<DataAcquisitionStoreStoreMetadataResultType>
DataAcquisitionStoreClient::StoreMetadataAsync(::bosdyn::api::StoreMetadataRequest& request,
                                               const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreStoreMetadataResultType> response;
    std::shared_future<DataAcquisitionStoreStoreMetadataResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<
        ::bosdyn::api::StoreMetadataRequest, ::bosdyn::api::StoreMetadataResponse, ::bosdyn::api::StoreMetadataResponse>(
        request,
        std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncStoreMetadata, m_stub.get(), _1, _2,
                  _3),
        std::bind(&DataAcquisitionStoreClient::OnStoreMetadataComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

DataAcquisitionStoreStoreMetadataResultType DataAcquisitionStoreClient::StoreMetadata(
    ::bosdyn::api::StoreMetadataRequest& request, const RPCParameters& parameters) {
    return StoreMetadataAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnStoreMetadataComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::StoreMetadataRequest& request,
    ::bosdyn::api::StoreMetadataResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreStoreMetadataResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::StoreMetadataResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}


// RPC to trigger data acquisition store to list stored AlertData.
std::shared_future<DataAcquisitionStoreListStoredAlertDataResultType>
DataAcquisitionStoreClient::ListStoredAlertDataAsync(::bosdyn::api::ListStoredAlertDataRequest& request,
                                                    const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreListStoredAlertDataResultType> response;
    std::shared_future<DataAcquisitionStoreListStoredAlertDataResultType> future =
        response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListStoredAlertDataRequest, ::bosdyn::api::ListStoredAlertDataResponse,
                          ::bosdyn::api::ListStoredAlertDataResponse>(
            request,
            std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncListStoredAlertData,
                      m_stub.get(), _1, _2, _3),
            std::bind(&DataAcquisitionStoreClient::OnListStoredAlertDataComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

DataAcquisitionStoreListStoredAlertDataResultType DataAcquisitionStoreClient::ListStoredAlertData(
    ::bosdyn::api::ListStoredAlertDataRequest& request, const RPCParameters& parameters) {
    return ListStoredAlertDataAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnListStoredAlertDataComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListStoredAlertDataRequest& request,
    ::bosdyn::api::ListStoredAlertDataResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreListStoredAlertDataResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListStoredAlertDataResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// RPC to trigger data acquisition store to store AlertData.
std::shared_future<DataAcquisitionStoreStoreAlertDataResultType>
DataAcquisitionStoreClient::StoreAlertDataAsync(::bosdyn::api::StoreAlertDataRequest& request,
                                               const RPCParameters& parameters) {
    std::promise<DataAcquisitionStoreStoreAlertDataResultType> response;
    std::shared_future<DataAcquisitionStoreStoreAlertDataResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<
        ::bosdyn::api::StoreAlertDataRequest, ::bosdyn::api::StoreAlertDataResponse, ::bosdyn::api::StoreAlertDataResponse>(
        request,
        std::bind(&::bosdyn::api::DataAcquisitionStoreService::Stub::AsyncStoreAlertData, m_stub.get(), _1, _2,
                  _3),
        std::bind(&DataAcquisitionStoreClient::OnStoreAlertDataComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

DataAcquisitionStoreStoreAlertDataResultType DataAcquisitionStoreClient::StoreAlertData(
    ::bosdyn::api::StoreAlertDataRequest& request, const RPCParameters& parameters) {
    return StoreAlertDataAsync(request, parameters).get();
}

void DataAcquisitionStoreClient::OnStoreAlertDataComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::StoreAlertDataRequest& request,
    ::bosdyn::api::StoreAlertDataResponse&& response, const grpc::Status& status,
    std::promise<DataAcquisitionStoreStoreAlertDataResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::StoreAlertDataResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService DataAcquisitionStoreClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void DataAcquisitionStoreClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::DataAcquisitionStoreService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
