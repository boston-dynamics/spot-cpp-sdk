/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/data_buffer.pb.h>
#include <bosdyn/api/data_buffer_service.grpc.pb.h>
#include <bosdyn/api/data_buffer_service.pb.h>

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/data_buffer/data_buffer_error_codes.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::RecordTextMessagesResponse> RecordTextMessagesResultType;
typedef Result<::bosdyn::api::RecordOperatorCommentsResponse> RecordOperatorCommentsResultType;
typedef Result<::bosdyn::api::RecordDataBlobsResponse> RecordDataBlobsResultType;
typedef Result<::bosdyn::api::RecordSignalTicksResponse> RecordSignalTicksResultType;
typedef Result<::bosdyn::api::RecordEventsResponse> RecordEventsResultType;
typedef Result<::bosdyn::api::RegisterSignalSchemaResponse> RegisterSignalSchemaResultType;

class DataBufferClient : public ServiceClient {
 public:
    DataBufferClient() = default;

    ~DataBufferClient() = default;

    // Asynchronous method to record text messages.
    std::shared_future<RecordTextMessagesResultType> RecordTextMessagesAsync(
        ::bosdyn::api::RecordTextMessagesRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to record text messages.
    RecordTextMessagesResultType RecordTextMessages(
        ::bosdyn::api::RecordTextMessagesRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record text messages.
    std::shared_future<RecordTextMessagesResultType> RecordTextMessagesAsync(
        const std::vector<::bosdyn::api::TextMessage>& text_messages,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record text messages.
    RecordTextMessagesResultType RecordTextMessages(
        const std::vector<::bosdyn::api::TextMessage>& text_messages,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record a single text message.
    std::shared_future<RecordTextMessagesResultType> RecordTextMessageAsync(
        const ::bosdyn::api::TextMessage& text_messages,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record a single text message.
    RecordTextMessagesResultType RecordTextMessage(
        const ::bosdyn::api::TextMessage& text_messages,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to record operator comments.
    std::shared_future<RecordOperatorCommentsResultType> RecordOperatorCommentsAsync(
        ::bosdyn::api::RecordOperatorCommentsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to record operator comments.
    RecordOperatorCommentsResultType RecordOperatorComments(
        ::bosdyn::api::RecordOperatorCommentsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record operator comments.
    std::shared_future<RecordOperatorCommentsResultType> RecordOperatorCommentsAsync(
        const std::vector<::bosdyn::api::OperatorComment>& comments,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record operator comments.
    RecordOperatorCommentsResultType RecordOperatorComments(
        const std::vector<::bosdyn::api::OperatorComment>& comments,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record a single operator comment.
    std::shared_future<RecordOperatorCommentsResultType> RecordOperatorCommentAsync(
        const ::bosdyn::api::OperatorComment& comment,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record a single operator comment.
    RecordOperatorCommentsResultType RecordOperatorComment(
        const ::bosdyn::api::OperatorComment& comment,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to record data blobs.
    std::shared_future<RecordDataBlobsResultType> RecordDataBlobsAsync(
        ::bosdyn::api::RecordDataBlobsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to record data blobs.
    RecordDataBlobsResultType RecordDataBlobs(
        ::bosdyn::api::RecordDataBlobsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record data blobs.
    std::shared_future<RecordDataBlobsResultType> RecordDataBlobsAsync(
        const std::vector<::bosdyn::api::DataBlob>& blobs,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record data blobs.
    RecordDataBlobsResultType RecordDataBlobs(
        const std::vector<::bosdyn::api::DataBlob>& blobs,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record a single data blob.
    std::shared_future<RecordDataBlobsResultType> RecordDataBlobAsync(
        const ::bosdyn::api::DataBlob& blob,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record a single data blob.
    RecordDataBlobsResultType RecordDataBlob(
        const ::bosdyn::api::DataBlob& blob,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to record signal ticks.
    std::shared_future<RecordSignalTicksResultType> RecordSignalTicksAsync(
        ::bosdyn::api::RecordSignalTicksRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to record signal ticks
    RecordSignalTicksResultType RecordSignalTicks(
        ::bosdyn::api::RecordSignalTicksRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record signal ticks.
    std::shared_future<RecordSignalTicksResultType> RecordSignalTicksAsync(
        const std::vector<::bosdyn::api::SignalTick>& ticks,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record signal ticks.
    RecordSignalTicksResultType RecordSignalTicks(
        const std::vector<::bosdyn::api::SignalTick>& ticks,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record a single signal tick.
    std::shared_future<RecordSignalTicksResultType> RecordSignalTickAsync(
        const ::bosdyn::api::SignalTick& tick,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record a single signal tick.
    RecordSignalTicksResultType RecordSignalTick(
        const ::bosdyn::api::SignalTick& tick,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to record events.
    std::shared_future<RecordEventsResultType> RecordEventsAsync(
        ::bosdyn::api::RecordEventsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to record events.
    RecordEventsResultType RecordEvents(
        ::bosdyn::api::RecordEventsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record events.
    std::shared_future<RecordEventsResultType> RecordEventsAsync(
        const std::vector<::bosdyn::api::Event>& events,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record events.
    RecordEventsResultType RecordEvents(
        const std::vector<::bosdyn::api::Event>& events,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to record a single event.
    std::shared_future<RecordEventsResultType> RecordEventAsync(
        const ::bosdyn::api::Event& event, const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to record a single event.
    RecordEventsResultType RecordEvent(
        const ::bosdyn::api::Event& event, const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to register a signal schema.
    std::shared_future<RegisterSignalSchemaResultType> RegisterSignalSchemaAsync(
        ::bosdyn::api::RegisterSignalSchemaRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to register a signal schema.
    RegisterSignalSchemaResultType RegisterSignalSchema(
        ::bosdyn::api::RegisterSignalSchemaRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to register a signal schema.
    std::shared_future<RegisterSignalSchemaResultType> RegisterSignalSchemaAsync(
        const ::bosdyn::api::SignalSchema& schema,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to register a signal schema.
    RegisterSignalSchemaResultType RegisterSignalSchema(
        const ::bosdyn::api::SignalSchema& schema,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the DataBuffer service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the DataBuffer service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to record text messages.
    void OnRecordTextMessagesComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RecordTextMessagesRequest& request,
        ::bosdyn::api::RecordTextMessagesResponse&& response, const grpc::Status& status,
        std::promise<RecordTextMessagesResultType> promise);

    // Callback function registered for the asynchronous calls to record operator comments.
    void OnRecordOperatorCommentsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RecordOperatorCommentsRequest& request,
        ::bosdyn::api::RecordOperatorCommentsResponse&& response, const grpc::Status& status,
        std::promise<RecordOperatorCommentsResultType> promise);

    // Callback function registered for the asynchronous calls to record data blobs.
    void OnRecordDataBlobsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RecordDataBlobsRequest& request,
        ::bosdyn::api::RecordDataBlobsResponse&& response, const grpc::Status& status,
        std::promise<RecordDataBlobsResultType> promise);

    // Callback function registered for the asynchronous calls to record signal ticks.
    void OnRecordSignalTicksComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RecordSignalTicksRequest& request,
        ::bosdyn::api::RecordSignalTicksResponse&& response, const grpc::Status& status,
        std::promise<RecordSignalTicksResultType> promise);

    // Callback function registered for the asynchronous calls to record events.
    void OnRecordEventsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RecordEventsRequest& request,
        ::bosdyn::api::RecordEventsResponse&& response, const grpc::Status& status,
        std::promise<RecordEventsResultType> promise);

    // Callback function registered for the asynchronous calls to register a signal schema.
    void OnRegisterSignalSchemaComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RegisterSignalSchemaRequest& request,
        ::bosdyn::api::RegisterSignalSchemaResponse&& response, const grpc::Status& status,
        std::promise<RegisterSignalSchemaResultType> promise);

    std::unique_ptr<::bosdyn::api::DataBufferService::Stub> m_stub;

    // Default service name for the DataBuffer service.
    static const char* s_default_service_name;

    // Default service type for the DataBuffer service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
