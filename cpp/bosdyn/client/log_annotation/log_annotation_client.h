/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <google/protobuf/util/time_util.h>

#include <bosdyn/api/log_annotation.pb.h>
#include <bosdyn/api/log_annotation_service.grpc.pb.h>
#include <bosdyn/api/log_annotation_service.pb.h>

#include <future>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::AddLogAnnotationResponse> AddLogAnnotationResultType;

class LogAnnotationClient : public ServiceClient {
 public:
    LogAnnotationClient() = default;

    ~LogAnnotationClient() = default;

    // Asynchronous main RPC method to add log annotations.
    std::shared_future<AddLogAnnotationResultType> AddLogAnnotationAsync(
        ::bosdyn::api::AddLogAnnotationRequest& request, const RPCParameters& parameters = RPCParameters());

    // Synchronous main RPC method to add log annotations.
    AddLogAnnotationResultType AddLogAnnotation(
        ::bosdyn::api::AddLogAnnotationRequest& request, const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to add log annotations with text messages.
    std::shared_future<AddLogAnnotationResultType> AddTextMessagesAsync(
        const std::vector<::bosdyn::api::LogAnnotationTextMessage>& text_messages,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to add log annotations with text messages.
    AddLogAnnotationResultType AddTextMessages(
        const std::vector<::bosdyn::api::LogAnnotationTextMessage>& text_messages,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to add log annotations with operator comments.
    std::shared_future<AddLogAnnotationResultType> AddOperatorCommentAsync(
        const std::string& comment,
        const ::google::protobuf::Timestamp& robot_timestamp =
        ::google::protobuf::util::TimeUtil::NanosecondsToTimestamp(0),
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to add log annotations with operator comments.
    AddLogAnnotationResultType AddOperatorComment(
        const std::string& comment,
        const ::google::protobuf::Timestamp& robot_timestamp =
        ::google::protobuf::util::TimeUtil::NanosecondsToTimestamp(0),
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to add log annotations with log blobs.
    // blob is the data to be logged.
    // type_id specifies the type of data that is being stored.
    // channel is optional GRPC channel where the data came from. If it's not passed, the client
    //   sets it to the value of type_id parameter.
    // robot_timestamp is optional time associated with the data. If it's not passed, the client
    //   sets it to the current machine time when the function is called.
    std::shared_future<AddLogAnnotationResultType> AddLogBlobAsync(
        const std::string& blob, const std::string& type_id, const std::string& channel = "",
        const ::google::protobuf::Timestamp& robot_timestamp =
        ::google::protobuf::util::TimeUtil::NanosecondsToTimestamp(0),
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to add log annotations with log blobs.
    // blob is the data to be logged.
    // type_id specifies the type of data that is being stored.
    // channel is optional GRPC channel where the data came from. If it's not passed, the client
    //   sets it to the value of type_id parameter.
    // robot_timestamp is optional time associated with the data. If it's not passed, the client
    //   sets it to the current machine time when the function is called.
    AddLogAnnotationResultType AddLogBlob(
        const std::string& blob, const std::string& type_id, const std::string& channel = "",
        const ::google::protobuf::Timestamp& robot_timestamp =
        ::google::protobuf::util::TimeUtil::NanosecondsToTimestamp(0),
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous helper method to add log annotations with full proto messages.
    // prop is the protobuf message to be logged.
    // robot_timestamp is optional time associated with the data. If it's not passed, the client
    //   sets it to the current machine time when the function is called.
    std::shared_future<AddLogAnnotationResultType> AddLogProtobufAsync(
        const ::google::protobuf::Message& proto,
        const ::google::protobuf::Timestamp& robot_timestamp =
        ::google::protobuf::util::TimeUtil::NanosecondsToTimestamp(0),
        const RPCParameters& parameters = RPCParameters());

    // Synchronous helper method to add log annotations with full proto messages.
    // prop is the protobuf message to be logged.
    // robot_timestamp is optional time associated with the data. If it's not passed, the client
    //   sets it to the current machine time when the function is called.
    AddLogAnnotationResultType AddLogProtobuf(
        const ::google::protobuf::Message& proto,
        const ::google::protobuf::Timestamp& robot_timestamp =
        ::google::protobuf::util::TimeUtil::NanosecondsToTimestamp(0),
        const RPCParameters& parameters = RPCParameters());


    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the LogAnnotation service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the LogAnnotation service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    void OnAddLogAnnotationComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::AddLogAnnotationRequest& request,
        ::bosdyn::api::AddLogAnnotationResponse&& response, const grpc::Status& status,
        std::promise<AddLogAnnotationResultType> promise);

    std::unique_ptr<::bosdyn::api::LogAnnotationService::Stub> m_stub;

    // Default service name for the LogAnnotation service.
    static const char* s_default_service_name;

    // Default service type for the LogAnnotation service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
