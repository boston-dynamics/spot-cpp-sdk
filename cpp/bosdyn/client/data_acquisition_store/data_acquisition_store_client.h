/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/data_acquisition_store_service.grpc.pb.h>
#include <bosdyn/api/data_acquisition_store_service.pb.h>
#include "bosdyn/client/service_client/service_client.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::ListCaptureActionsResponse> DataAcquisitionStoreListCaptureActionsResultType;
typedef Result<::bosdyn::api::ListStoredDataResponse> DataAcquisitionStoreListStoredDataResultType;
typedef Result<::bosdyn::api::StoreDataResponse> DataAcquisitionStoreStoreDataResultType;
typedef Result<::bosdyn::api::ListStoredImagesResponse> DataAcquisitionStoreListStoredImagesResultType;
typedef Result<::bosdyn::api::StoreImageResponse> DataAcquisitionStoreStoreImageResultType;
typedef Result<::bosdyn::api::ListStoredMetadataResponse> DataAcquisitionStoreListStoredMetadataResultType;
typedef Result<::bosdyn::api::StoreMetadataResponse> DataAcquisitionStoreStoreMetadataResultType;
typedef Result<::bosdyn::api::ListStoredAlertDataResponse> DataAcquisitionStoreListStoredAlertDataResultType;
typedef Result<::bosdyn::api::StoreAlertDataResponse> DataAcquisitionStoreStoreAlertDataResultType;

class DataAcquisitionStoreClient : public ServiceClient {
 public:
    DataAcquisitionStoreClient() = default;

    ~DataAcquisitionStoreClient() = default;

    // Asynchronous RPC to trigger data acquisition store to list capture actions.
    std::shared_future<DataAcquisitionStoreListCaptureActionsResultType> ListCaptureActionsAsync(
        ::bosdyn::api::ListCaptureActionsRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to list capture actions.
    DataAcquisitionStoreListCaptureActionsResultType ListCaptureActions(
        ::bosdyn::api::ListCaptureActionsRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to list stored data.
    std::shared_future<DataAcquisitionStoreListStoredDataResultType> ListStoredDataAsync(
        ::bosdyn::api::ListStoredDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to list stored data.
    DataAcquisitionStoreListStoredDataResultType ListStoredData(
        ::bosdyn::api::ListStoredDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to store data.
    std::shared_future<DataAcquisitionStoreStoreDataResultType> StoreDataAsync(
        ::bosdyn::api::StoreDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to store data.
    DataAcquisitionStoreStoreDataResultType StoreData(
        ::bosdyn::api::StoreDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to list stored images.
    std::shared_future<DataAcquisitionStoreListStoredImagesResultType> ListStoredImagesAsync(
        ::bosdyn::api::ListStoredImagesRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to list stored images.
    DataAcquisitionStoreListStoredImagesResultType ListStoredImages(
        ::bosdyn::api::ListStoredImagesRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to store image.
    std::shared_future<DataAcquisitionStoreStoreImageResultType> StoreImageAsync(
        ::bosdyn::api::StoreImageRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to store image.
    DataAcquisitionStoreStoreImageResultType StoreImage(
        ::bosdyn::api::StoreImageRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to list stored metadata.
    std::shared_future<DataAcquisitionStoreListStoredMetadataResultType> ListStoredMetadataAsync(
        ::bosdyn::api::ListStoredMetadataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to list stored metadata.
    DataAcquisitionStoreListStoredMetadataResultType ListStoredMetadata(
        ::bosdyn::api::ListStoredMetadataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to store metadata.
    std::shared_future<DataAcquisitionStoreStoreMetadataResultType> StoreMetadataAsync(
        ::bosdyn::api::StoreMetadataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to store metadata.
    DataAcquisitionStoreStoreMetadataResultType StoreMetadata(
        ::bosdyn::api::StoreMetadataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to list stored AlertData.
    std::shared_future<DataAcquisitionStoreListStoredAlertDataResultType> ListStoredAlertDataAsync(
        ::bosdyn::api::ListStoredAlertDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to list stored AlertData.
    DataAcquisitionStoreListStoredAlertDataResultType ListStoredAlertData(
        ::bosdyn::api::ListStoredAlertDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC to trigger data acquisition store to store AlertData.
    std::shared_future<DataAcquisitionStoreStoreAlertDataResultType> StoreAlertDataAsync(
        ::bosdyn::api::StoreAlertDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC to trigger data acquisition store to store AlertData.
    DataAcquisitionStoreStoreAlertDataResultType StoreAlertData(
        ::bosdyn::api::StoreAlertDataRequest& request, const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the image service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to list capture actions.
    void OnListCaptureActionsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListCaptureActionsRequest& request,
        ::bosdyn::api::ListCaptureActionsResponse&& response, const grpc::Status& status,
        std::promise<DataAcquisitionStoreListCaptureActionsResultType> promise);

    // Callback function registered for the asynchronous calls to list stored data.
    void OnListStoredDataComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListStoredDataRequest& request,
        ::bosdyn::api::ListStoredDataResponse&& response, const grpc::Status& status,
        std::promise<DataAcquisitionStoreListStoredDataResultType> promise);

    // Callback function registered for the asynchronous calls to store data.
    void OnStoreDataComplete(MessagePumpCallBase* call, const ::bosdyn::api::StoreDataRequest& request,
                             ::bosdyn::api::StoreDataResponse&& response, const grpc::Status& status,
                             std::promise<DataAcquisitionStoreStoreDataResultType> promise);

    // Callback function registered for the asynchronous calls to list stored images.
    void OnListStoredImagesComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListStoredImagesRequest& request,
        ::bosdyn::api::ListStoredImagesResponse&& response, const grpc::Status& status,
        std::promise<DataAcquisitionStoreListStoredImagesResultType> promise);

    // Callback function registered for the asynchronous calls to store image.
    void OnStoreImageComplete(MessagePumpCallBase* call, const ::bosdyn::api::StoreImageRequest& request,
                              ::bosdyn::api::StoreImageResponse&& response, const grpc::Status& status,
                              std::promise<DataAcquisitionStoreStoreImageResultType> promise);

    // Callback function registered for the asynchronous calls to list stored metadata.
    void OnListStoredMetadataComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListStoredMetadataRequest& request,
        ::bosdyn::api::ListStoredMetadataResponse&& response, const grpc::Status& status,
        std::promise<DataAcquisitionStoreListStoredMetadataResultType> promise);

    // Callback function registered for the asynchronous calls to store metadata.
    void OnStoreMetadataComplete(MessagePumpCallBase* call,
                                 const ::bosdyn::api::StoreMetadataRequest& request,
                                 ::bosdyn::api::StoreMetadataResponse&& response, const grpc::Status& status,
                                 std::promise<DataAcquisitionStoreStoreMetadataResultType> promise);

    // Callback function registered for the asynchronous calls to list stored AlertData.
    void OnListStoredAlertDataComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListStoredAlertDataRequest& request,
        ::bosdyn::api::ListStoredAlertDataResponse&& response, const grpc::Status& status,
        std::promise<DataAcquisitionStoreListStoredAlertDataResultType> promise);

    // Callback function registered for the asynchronous calls to store AlertData.
    void OnStoreAlertDataComplete(MessagePumpCallBase* call,
                                 const ::bosdyn::api::StoreAlertDataRequest& request,
                                 ::bosdyn::api::StoreAlertDataResponse&& response, const grpc::Status& status,
                                 std::promise<DataAcquisitionStoreStoreAlertDataResultType> promise);

    std::unique_ptr<::bosdyn::api::DataAcquisitionStoreService::Stub> m_stub;

    // Default service name for the Data Acquisition service.
    static const char* s_default_service_name;

    // Default service type for the data acquisition service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
