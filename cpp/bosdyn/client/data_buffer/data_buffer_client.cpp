/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/data_buffer/data_buffer_client.h"

#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* DataBufferClient::s_default_service_name = "data-buffer";

const char* DataBufferClient::s_service_type = "bosdyn.api.DataBufferService";

std::shared_future<RecordTextMessagesResultType> DataBufferClient::RecordTextMessagesAsync(
    ::bosdyn::api::RecordTextMessagesRequest& request,
    const RPCParameters& parameters) {
    std::promise<RecordTextMessagesResultType> response;
    std::shared_future<RecordTextMessagesResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RecordTextMessagesRequest, ::bosdyn::api::RecordTextMessagesResponse,
                          ::bosdyn::api::RecordTextMessagesResponse>(
            request,
            std::bind(&::bosdyn::api::DataBufferService::Stub::AsyncRecordTextMessages, m_stub.get(), _1, _2,
                      _3),
            std::bind(&DataBufferClient::OnRecordTextMessagesComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

RecordTextMessagesResultType DataBufferClient::RecordTextMessages(
    ::bosdyn::api::RecordTextMessagesRequest& request,
    const RPCParameters& parameters) {
    return RecordTextMessagesAsync(request, parameters).get();
}

std::shared_future<RecordTextMessagesResultType> DataBufferClient::RecordTextMessagesAsync(
    const std::vector<::bosdyn::api::TextMessage>& text_messages,
    const RPCParameters& parameters) {
    ::bosdyn::api::RecordTextMessagesRequest request;
    *request.mutable_text_messages() = {text_messages.begin(), text_messages.end()};
    return RecordTextMessagesAsync(request, parameters);
}

RecordTextMessagesResultType DataBufferClient::RecordTextMessages(
    const std::vector<::bosdyn::api::TextMessage>& text_messages,
    const RPCParameters& parameters) {
    return RecordTextMessagesAsync(text_messages, parameters).get();
}

std::shared_future<RecordTextMessagesResultType> DataBufferClient::RecordTextMessageAsync(
    const ::bosdyn::api::TextMessage& text_message, const RPCParameters& parameters) {
    ::bosdyn::api::RecordTextMessagesRequest request;
    *request.add_text_messages() = text_message;
    return RecordTextMessagesAsync(request, parameters);
}

RecordTextMessagesResultType DataBufferClient::RecordTextMessage(
    const ::bosdyn::api::TextMessage& text_message, const RPCParameters& parameters) {
    return RecordTextMessageAsync(text_message, parameters).get();
}

std::shared_future<RecordOperatorCommentsResultType> DataBufferClient::RecordOperatorCommentsAsync(
    ::bosdyn::api::RecordOperatorCommentsRequest& request,
    const RPCParameters& parameters) {
    std::promise<RecordOperatorCommentsResultType> response;
    std::shared_future<RecordOperatorCommentsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RecordOperatorCommentsRequest, ::bosdyn::api::RecordOperatorCommentsResponse,
                          ::bosdyn::api::RecordOperatorCommentsResponse>(
            request,
            std::bind(&::bosdyn::api::DataBufferService::Stub::AsyncRecordOperatorComments, m_stub.get(), _1,
                      _2, _3),
            std::bind(&DataBufferClient::OnRecordOperatorCommentsComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);
    return future;
}

RecordOperatorCommentsResultType DataBufferClient::RecordOperatorComments(
    ::bosdyn::api::RecordOperatorCommentsRequest& request,
    const RPCParameters& parameters) {
    return RecordOperatorCommentsAsync(request, parameters).get();
}

std::shared_future<RecordOperatorCommentsResultType> DataBufferClient::RecordOperatorCommentsAsync(
    const std::vector<::bosdyn::api::OperatorComment>& comments,
    const RPCParameters& parameters) {
    ::bosdyn::api::RecordOperatorCommentsRequest request;
    *request.mutable_operator_comments() = {comments.begin(), comments.end()};
    return RecordOperatorCommentsAsync(request, parameters);
}

RecordOperatorCommentsResultType DataBufferClient::RecordOperatorComments(
    const std::vector<::bosdyn::api::OperatorComment>& comments,
    const RPCParameters& parameters) {
    return RecordOperatorCommentsAsync(comments, parameters).get();
}

std::shared_future<RecordOperatorCommentsResultType> DataBufferClient::RecordOperatorCommentAsync(
    const ::bosdyn::api::OperatorComment& comment, const RPCParameters& parameters) {
    ::bosdyn::api::RecordOperatorCommentsRequest request;
    *request.add_operator_comments() = comment;
    return RecordOperatorCommentsAsync(request, parameters);
}

RecordOperatorCommentsResultType DataBufferClient::RecordOperatorComment(
    const ::bosdyn::api::OperatorComment& comment, const RPCParameters& parameters) {
    return RecordOperatorCommentAsync(comment, parameters).get();
}

std::shared_future<RecordDataBlobsResultType> DataBufferClient::RecordDataBlobsAsync(
    ::bosdyn::api::RecordDataBlobsRequest& request, const RPCParameters& parameters) {
    std::promise<RecordDataBlobsResultType> response;
    std::shared_future<RecordDataBlobsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RecordDataBlobsRequest, ::bosdyn::api::RecordDataBlobsResponse,
                          ::bosdyn::api::RecordDataBlobsResponse>(
            request,
            std::bind(&::bosdyn::api::DataBufferService::Stub::AsyncRecordDataBlobs, m_stub.get(), _1, _2,
                      _3),
            std::bind(&DataBufferClient::OnRecordDataBlobsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

RecordDataBlobsResultType DataBufferClient::RecordDataBlobs(
    ::bosdyn::api::RecordDataBlobsRequest& request, const RPCParameters& parameters) {
    return RecordDataBlobsAsync(request, parameters).get();
}

std::shared_future<RecordDataBlobsResultType> DataBufferClient::RecordDataBlobsAsync(
    const std::vector<::bosdyn::api::DataBlob>& blobs,
    const RPCParameters& parameters) {
    ::bosdyn::api::RecordDataBlobsRequest request;
    *request.mutable_blob_data() = {blobs.begin(), blobs.end()};
    return RecordDataBlobsAsync(request, parameters);
}

RecordDataBlobsResultType DataBufferClient::RecordDataBlobs(
    const std::vector<::bosdyn::api::DataBlob>& blobs,
    const RPCParameters& parameters) {
    return RecordDataBlobsAsync(blobs, parameters).get();
}

std::shared_future<RecordDataBlobsResultType> DataBufferClient::RecordDataBlobAsync(
    const ::bosdyn::api::DataBlob& blob, const RPCParameters& parameters) {
    ::bosdyn::api::RecordDataBlobsRequest request;
    *request.add_blob_data() = blob;
    return RecordDataBlobsAsync(request, parameters);
}

RecordDataBlobsResultType DataBufferClient::RecordDataBlob(
    const ::bosdyn::api::DataBlob& blob, const RPCParameters& parameters) {
    return RecordDataBlobAsync(blob, parameters).get();
}

std::shared_future<RecordSignalTicksResultType> DataBufferClient::RecordSignalTicksAsync(
    ::bosdyn::api::RecordSignalTicksRequest& request,
    const RPCParameters& parameters) {
    std::promise<RecordSignalTicksResultType> response;
    std::shared_future<RecordSignalTicksResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RecordSignalTicksRequest, ::bosdyn::api::RecordSignalTicksResponse,
                          ::bosdyn::api::RecordSignalTicksResponse>(
            request,
            std::bind(&::bosdyn::api::DataBufferService::Stub::AsyncRecordSignalTicks, m_stub.get(), _1, _2,
                      _3),
            std::bind(&DataBufferClient::OnRecordSignalTicksComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

RecordSignalTicksResultType DataBufferClient::RecordSignalTicks(
    ::bosdyn::api::RecordSignalTicksRequest& request,
    const RPCParameters& parameters) {
    return RecordSignalTicksAsync(request, parameters).get();
}

std::shared_future<RecordSignalTicksResultType> DataBufferClient::RecordSignalTicksAsync(
    const std::vector<::bosdyn::api::SignalTick>& ticks,
    const RPCParameters& parameters) {
    ::bosdyn::api::RecordSignalTicksRequest request;
    *request.mutable_tick_data() = {ticks.begin(), ticks.end()};
    return RecordSignalTicksAsync(request, parameters);
}

RecordSignalTicksResultType DataBufferClient::RecordSignalTicks(
    const std::vector<::bosdyn::api::SignalTick>& ticks,
    const RPCParameters& parameters) {
    return RecordSignalTicksAsync(ticks, parameters).get();
}

std::shared_future<RecordSignalTicksResultType> DataBufferClient::RecordSignalTickAsync(
    const ::bosdyn::api::SignalTick& tick, const RPCParameters& parameters) {
    ::bosdyn::api::RecordSignalTicksRequest request;
    *request.add_tick_data() = tick;
    return RecordSignalTicksAsync(request, parameters);
}

RecordSignalTicksResultType DataBufferClient::RecordSignalTick(
    const ::bosdyn::api::SignalTick& tick, const RPCParameters& parameters) {
    return RecordSignalTickAsync(tick, parameters).get();
}

std::shared_future<RecordEventsResultType> DataBufferClient::RecordEventsAsync(
    ::bosdyn::api::RecordEventsRequest& request, const RPCParameters& parameters) {
    std::promise<RecordEventsResultType> response;
    std::shared_future<RecordEventsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RecordEventsRequest, ::bosdyn::api::RecordEventsResponse,
                          ::bosdyn::api::RecordEventsResponse>(
            request,
            std::bind(&::bosdyn::api::DataBufferService::Stub::AsyncRecordEvents, m_stub.get(), _1, _2, _3),
            std::bind(&DataBufferClient::OnRecordEventsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

RecordEventsResultType DataBufferClient::RecordEvents(
    ::bosdyn::api::RecordEventsRequest& request, const RPCParameters& parameters) {
    return RecordEventsAsync(request, parameters).get();
}

std::shared_future<RecordEventsResultType> DataBufferClient::RecordEventsAsync(
    const std::vector<::bosdyn::api::Event>& events, const RPCParameters& parameters) {
    ::bosdyn::api::RecordEventsRequest request;
    *request.mutable_events() = {events.begin(), events.end()};
    return RecordEventsAsync(request, parameters);
}

RecordEventsResultType DataBufferClient::RecordEvents(
    const std::vector<::bosdyn::api::Event>& events, const RPCParameters& parameters) {
    return RecordEventsAsync(events, parameters).get();
}

std::shared_future<RecordEventsResultType> DataBufferClient::RecordEventAsync(
    const ::bosdyn::api::Event& event, const RPCParameters& parameters) {
    ::bosdyn::api::RecordEventsRequest request;
    *request.add_events() = event;
    return RecordEventsAsync(request, parameters);
}

RecordEventsResultType DataBufferClient::RecordEvent(
    const ::bosdyn::api::Event& event, const RPCParameters& parameters) {
    return RecordEventAsync(event, parameters).get();
}

std::shared_future<RegisterSignalSchemaResultType> DataBufferClient::RegisterSignalSchemaAsync(
    ::bosdyn::api::RegisterSignalSchemaRequest& request,
    const RPCParameters& parameters) {
    std::promise<RegisterSignalSchemaResultType> response;
    std::shared_future<RegisterSignalSchemaResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RegisterSignalSchemaRequest, ::bosdyn::api::RegisterSignalSchemaResponse,
                          ::bosdyn::api::RegisterSignalSchemaResponse>(
            request,
            std::bind(&::bosdyn::api::DataBufferService::Stub::AsyncRegisterSignalSchema, m_stub.get(), _1,
                      _2, _3),
            std::bind(&DataBufferClient::OnRegisterSignalSchemaComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

RegisterSignalSchemaResultType DataBufferClient::RegisterSignalSchema(
    ::bosdyn::api::RegisterSignalSchemaRequest& request,
    const RPCParameters& parameters) {
    return RegisterSignalSchemaAsync(request, parameters).get();
}

std::shared_future<RegisterSignalSchemaResultType> DataBufferClient::RegisterSignalSchemaAsync(
    const ::bosdyn::api::SignalSchema& schema, const RPCParameters& parameters) {
    ::bosdyn::api::RegisterSignalSchemaRequest request;
    *request.mutable_schema() = schema;
    return RegisterSignalSchemaAsync(request, parameters);
}

RegisterSignalSchemaResultType DataBufferClient::RegisterSignalSchema(
    const ::bosdyn::api::SignalSchema& schema, const RPCParameters& parameters) {
    return RegisterSignalSchemaAsync(schema, parameters).get();
}

void DataBufferClient::OnRecordTextMessagesComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RecordTextMessagesRequest& request,
    ::bosdyn::api::RecordTextMessagesResponse&& response, const grpc::Status& status,
    std::promise<RecordTextMessagesResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RecordTextMessagesResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    for (const auto& error : response.errors()) {
        std::error_code code = error.type();
        if (code != SuccessCondition::Success) {
            promise.set_value({::bosdyn::common::Status(code, "RecordTextMessagesResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

void DataBufferClient::OnRecordOperatorCommentsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RecordOperatorCommentsRequest& request,
    ::bosdyn::api::RecordOperatorCommentsResponse&& response, const grpc::Status& status,
    std::promise<RecordOperatorCommentsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RecordOperatorCommentsResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    for (const auto& error : response.errors()) {
        std::error_code code = error.type();
        if (code != SuccessCondition::Success) {
            promise.set_value({::bosdyn::common::Status(code, "RecordOperatorCommentsResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

void DataBufferClient::OnRecordDataBlobsComplete(MessagePumpCallBase* call,
                                                 const ::bosdyn::api::RecordDataBlobsRequest& request,
                                                 ::bosdyn::api::RecordDataBlobsResponse&& response,
                                                 const grpc::Status& status,
                                                 std::promise<RecordDataBlobsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RecordDataBlobsResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    for (const auto& error : response.errors()) {
        std::error_code code = error.type();
        if (code != SuccessCondition::Success) {
            promise.set_value({::bosdyn::common::Status(code, "RecordDataBlobsResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

void DataBufferClient::OnRecordSignalTicksComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RecordSignalTicksRequest& request,
    ::bosdyn::api::RecordSignalTicksResponse&& response, const grpc::Status& status,
    std::promise<RecordSignalTicksResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RecordSignalTicksResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    for (const auto& error : response.errors()) {
        std::error_code code = error.type();
        if (code != SuccessCondition::Success) {
            promise.set_value({::bosdyn::common::Status(code, "RecordSignalTicksResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

void DataBufferClient::OnRecordEventsComplete(MessagePumpCallBase* call,
                                              const ::bosdyn::api::RecordEventsRequest& request,
                                              ::bosdyn::api::RecordEventsResponse&& response,
                                              const grpc::Status& status,
                                              std::promise<RecordEventsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RecordEventsResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    for (const auto& error : response.errors()) {
        std::error_code code = error.type();
        if (code != SuccessCondition::Success) {
            promise.set_value({::bosdyn::common::Status(code, "RecordEventsResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

void DataBufferClient::OnRegisterSignalSchemaComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RegisterSignalSchemaRequest& request,
    ::bosdyn::api::RegisterSignalSchemaResponse&& response, const grpc::Status& status,
    std::promise<RegisterSignalSchemaResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RegisterSignalSchemaResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService DataBufferClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void DataBufferClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::DataBufferService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
