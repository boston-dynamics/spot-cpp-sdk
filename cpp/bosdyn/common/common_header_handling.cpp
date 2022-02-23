/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "common_header_handling.h"
#include "strip_bytes_fields.h"

#include <google/protobuf/any.pb.h>

#include <chrono>
#include <sstream>
#include <string>

#include "bosdyn/common/time.h"

namespace bosdyn {

namespace common {

void PrepareResponseHeader(const ::bosdyn::api::RequestHeader& request_header,
                           const google::protobuf::Message* reflected_request,
                           ::bosdyn::api::ResponseHeader* out_header) {
    SetTimestamp(NowNsec(), out_header->mutable_request_received_timestamp());
    out_header->mutable_request_header()->CopyFrom(request_header);
    if (reflected_request) {
        std::unique_ptr<google::protobuf::Message> copy(reflected_request->New());
        copy->CopyFrom(*reflected_request);
        StripLargeByteFields(copy.get());
        out_header->mutable_request()->PackFrom(*copy);
    }
}

void StripLargeByteFields(::google::protobuf::Message* proto_message) {
    // Operates on Request and Response messages.
    const std::string message_type = proto_message->GetTypeName();
    const auto& whitelist_map = ::bosdyn::common::strip_messages::kWhitelistedBytesFieldsMap;
    auto strip_message_func = whitelist_map.find(message_type);
    if (strip_message_func != whitelist_map.end()) {
        strip_message_func->second(proto_message);
    }
}

void SetError(::bosdyn::api::CommonError::Code code, const std::string& error_message,
              ::bosdyn::api::ResponseHeader* out_header) {
    out_header->mutable_error()->set_code(code);
    out_header->mutable_error()->set_message(error_message);
    SetTimestamp(NowNsec(), out_header->mutable_response_timestamp());
}

bool SetOk(::bosdyn::api::ResponseHeader* out_header) {
    SetError(::bosdyn::api::CommonError::CODE_OK, "", out_header);
    return true;
}

bool SetOkIfNotError(::bosdyn::api::ResponseHeader* out_header) {
    if (out_header->error().code() == ::bosdyn::api::CommonError::CODE_UNSPECIFIED) {
        return SetOk(out_header);
    }
    SetTimestamp(NowNsec(), out_header->mutable_response_timestamp());
    return out_header->error().code() == ::bosdyn::api::CommonError::CODE_OK;
}

bool SetInvalidRequest(const std::string& error_message, ::bosdyn::api::ResponseHeader* out_header) {
    SetError(::bosdyn::api::CommonError::CODE_INVALID_REQUEST, error_message, out_header);
    return false;
}

bool SetInternalError(const std::string& error_message, ::bosdyn::api::ResponseHeader* out_header) {
    SetError(::bosdyn::api::CommonError::CODE_INTERNAL_SERVER_ERROR, error_message, out_header);
    return false;
}

bool ValidateRequestHeaderAndRespond(const ::bosdyn::api::RequestHeader& request_header,
                                     const ::google::protobuf::Message* reflected_request,
                                     ::bosdyn::api::ResponseHeader* out_response_header) {
    PrepareResponseHeader(request_header, reflected_request, out_response_header);
    if (!request_header.has_request_timestamp()) {
        return SetInvalidRequest("No request_timestamp message present in header",
                                 out_response_header);
    }
    const ::google::protobuf::Timestamp& timestamp = request_header.request_timestamp();
    if (timestamp.seconds() < 0 || timestamp.nanos() < 0) {
        std::ostringstream error_msg;
        error_msg << "Invalid request_timestamp " << timestamp.seconds() << "." << timestamp.nanos()
                  << " in header.";
        return SetInvalidRequest(error_msg.str(), out_response_header);
    }
    if (request_header.client_name().empty()) {
        return SetInvalidRequest("Invalid client_name in header", out_response_header);
    }

    return SetOk(out_response_header);
}

}  // namespace common

}  // namespace bosdyn
