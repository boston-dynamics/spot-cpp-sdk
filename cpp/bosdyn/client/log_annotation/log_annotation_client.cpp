/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/log_annotation/log_annotation_client.h"
#include "bosdyn/common/time.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* LogAnnotationClient::s_default_service_name = "log-annotation";

const char* LogAnnotationClient::s_service_type = "bosdyn.api.LogAnnotationService";

std::shared_future<AddLogAnnotationResultType> LogAnnotationClient::AddLogAnnotationAsync(
    ::bosdyn::api::AddLogAnnotationRequest& request, const RPCParameters& parameters) {
    std::promise<AddLogAnnotationResultType> response;
    std::shared_future<AddLogAnnotationResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::AddLogAnnotationRequest, ::bosdyn::api::AddLogAnnotationResponse,
                          ::bosdyn::api::AddLogAnnotationResponse>(
            request,
            std::bind(&::bosdyn::api::LogAnnotationService::Stub::AsyncAddLogAnnotation, m_stub.get(), _1, _2,
                      _3),
            std::bind(&LogAnnotationClient::OnAddLogAnnotationComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

AddLogAnnotationResultType LogAnnotationClient::AddLogAnnotation(
    ::bosdyn::api::AddLogAnnotationRequest& request, const RPCParameters& parameters) {
    return AddLogAnnotationAsync(request, parameters).get();
}

void LogAnnotationClient::OnAddLogAnnotationComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::AddLogAnnotationRequest& request,
    ::bosdyn::api::AddLogAnnotationResponse&& response, const grpc::Status& status,
    std::promise<AddLogAnnotationResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::AddLogAnnotationResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<AddLogAnnotationResultType> LogAnnotationClient::AddTextMessagesAsync(
    const std::vector<::bosdyn::api::LogAnnotationTextMessage>& text_messages,
    const RPCParameters& parameters) {
    // Perform error checking first.
    std::promise<AddLogAnnotationResultType> error_response;
    std::shared_future<AddLogAnnotationResultType> error_future = error_response.get_future();
    ::bosdyn::api::AddLogAnnotationRequest request;
    *request.mutable_annotations()->mutable_text_messages() = {text_messages.begin(),
                                                               text_messages.end()};
    return AddLogAnnotationAsync(request, parameters);
}

AddLogAnnotationResultType LogAnnotationClient::AddTextMessages(
    const std::vector<::bosdyn::api::LogAnnotationTextMessage>& text_messages,
    const RPCParameters& parameters) {
    return AddTextMessagesAsync(text_messages, parameters).get();
}

std::shared_future<AddLogAnnotationResultType> LogAnnotationClient::AddOperatorCommentAsync(
    const std::string& comment, const google::protobuf::Timestamp& robot_timestamp,
    const RPCParameters& parameters) {
    // Perform error checking first.
    std::promise<AddLogAnnotationResultType> error_response;
    std::shared_future<AddLogAnnotationResultType> error_future = error_response.get_future();
    ::bosdyn::api::AddLogAnnotationRequest request;
    ::bosdyn::api::LogAnnotationOperatorMessage* op_message =
        request.mutable_annotations()->add_operator_messages();
    if (::google::protobuf::util::TimeUtil::TimestampToNanoseconds(robot_timestamp) > 0) {
        ::google::protobuf::Timestamp ts_copy(robot_timestamp);
        op_message->set_allocated_timestamp(&ts_copy);
    }
    op_message->set_message(comment);
    return AddLogAnnotationAsync(request, parameters);
}

AddLogAnnotationResultType LogAnnotationClient::AddOperatorComment(
    const std::string& comment, const google::protobuf::Timestamp& robot_timestamp,
    const RPCParameters& parameters) {
    return AddOperatorCommentAsync(comment, robot_timestamp, parameters).get();
}

std::shared_future<AddLogAnnotationResultType> LogAnnotationClient::AddLogBlobAsync(
    const std::string& blob, const std::string& type_id, const std::string& channel,
    const google::protobuf::Timestamp& robot_timestamp, const RPCParameters& parameters) {
    // Perform error checking first.
    std::promise<AddLogAnnotationResultType> error_response;
    std::shared_future<AddLogAnnotationResultType> error_future = error_response.get_future();
    ::bosdyn::api::AddLogAnnotationRequest request;
    ::bosdyn::api::LogAnnotationLogBlob* blob_data = request.mutable_annotations()->add_blob_data();
    if (::google::protobuf::util::TimeUtil::TimestampToNanoseconds(robot_timestamp) > 0) {
        blob_data->mutable_timestamp()->CopyFrom(robot_timestamp);
    } else {
        *blob_data->mutable_timestamp() = ::bosdyn::common::NowTimestamp();
    }

    if (channel.empty()) {
        blob_data->set_channel(type_id);
    } else {
        blob_data->set_channel(channel);
    }
    blob_data->set_type_id(type_id);
    blob_data->set_data(blob);
    return AddLogAnnotationAsync(request, parameters);
}

AddLogAnnotationResultType LogAnnotationClient::AddLogBlob(
    const std::string& blob, const std::string& type_id, const std::string& channel,
    const google::protobuf::Timestamp& robot_timestamp, const RPCParameters& parameters) {
    return AddLogBlobAsync(blob, type_id, channel, robot_timestamp, parameters).get();
}

std::shared_future<AddLogAnnotationResultType> LogAnnotationClient::AddLogProtobufAsync(
    const ::google::protobuf::Message& proto, const google::protobuf::Timestamp& robot_timestamp,
    const RPCParameters& parameters) {
    std::string blob;
    if (!proto.SerializeToString(&blob)) {
        std::promise<AddLogAnnotationResultType> error_response;
        std::shared_future<AddLogAnnotationResultType> error_future = error_response.get_future();
        error_response.set_value({::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                         "SerializeAsString could not serialize proto message"),
                                  {}});
        return error_future;
    }
    std::string type = proto.GetTypeName();
    return AddLogBlobAsync(blob, type, type, robot_timestamp, parameters);
}

AddLogAnnotationResultType LogAnnotationClient::AddLogProtobuf(
    const ::google::protobuf::Message& proto, const google::protobuf::Timestamp& robot_timestamp,
    const RPCParameters& parameters) {
    return AddLogProtobufAsync(proto, robot_timestamp, parameters).get();
}


ServiceClient::QualityOfService LogAnnotationClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void LogAnnotationClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::LogAnnotationService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
