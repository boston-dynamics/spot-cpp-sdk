/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/header.pb.h>

#include <chrono>
#include <string>
#include <typeinfo>
#include <vector>

#include "bosdyn/common/time.h"

namespace bosdyn {

namespace common {

/// Sets fields of out_response_header.
///
/// To be called at the start of a GRPC service call.
///
/// \param   request_header         RequestHeader to be copied into out_response_header.
/// \param   reflected_request      If not nullptr, request to be copied into out_response_header.
/// \param[out] out_response_header RequestHeader filled-in by this function.
void PrepareResponseHeader(const ::bosdyn::api::RequestHeader& request_header,
                           const google::protobuf::Message* reflected_request,
                           ::bosdyn::api::ResponseHeader* out_response_header);

/// Called by PrepareResponseHeader (and grpc logging) to remove any large bytes fields from
/// messages in a request/responses being copied into the response header. Only fields/messages
/// added in the whitelist defined in GetBytesFieldWhitelist() in strip_bytes_fields.h will
/// be removed by this function.
///
/// \param      proto_message   Protobuf message to be stripped of any bytes fields.
void StripLargeByteFields(::google::protobuf::Message* proto_message);

/// Indicate service call succeeded.
///
/// To be called at the end of a GRPC service call.
/// Sets status to CommonError::OK, and sets response_timestamp to the current time.
///
/// \param[out] out_header  ResponseHeader to be updated by this call.
/// \return  Always returns true.
bool SetOk(::bosdyn::api::ResponseHeader* out_header);

/// Indicate service call succeeded.
///
/// To be called at the end of a GRPC service call.
/// Sets status to CommonError::OK, and sets response_timestamp to the current time.
///
/// \param[out] response    Response whose header is updated by this call.
/// \return  Always returns true.
template <class ResponseType>
bool SetOk(ResponseType* response) {
    return SetOk(response->mutable_header());
}

/// Call at the end of the a GRPC call to set the error code to OK if it is still UNSPECIFIED.
///
/// To be called at the end of a GRPC service call.
/// Sets status to CommonError::OK, and sets response_timestamp to the current time.
/// \param[out] out_header     response header to be updated by this call.
/// \return     Returns true if the error code is set to OK, false otherwise.
bool SetOkIfNotError(::bosdyn::api::ResponseHeader* out_header);

/// Call at the end of the a GRPC call to set the error code to OK if it is still UNSPECIFIED.
///
/// To be called at the end of a GRPC service call.
/// Sets status to CommonError::OK, and sets response_timestamp to the current time.
/// \param[out] response       Response whose common header is updated by this call.
/// \return     Returns true if the error code is set to OK, false otherwise.
template <class ResponseType>
bool SetOkIfNotError(ResponseType* response) {
    return SetOkIfNotError(response->mutable_header());
}

/// Call at the end of handling a GRPC request to indicate that the request is invalid.
/// Populates the common code with INVALID_REQUEST and sets the response_timestamp.
///
/// \param      error_message   Error message to set in response.
/// \param[out] out_header      ResponseHeader to be updated by this call.
/// \return  Always returns false.
bool SetInvalidRequest(const std::string& error_message, ::bosdyn::api::ResponseHeader* out_header);

/// Call at the end of handling a GRPC request to indicate that the request is invalid.
/// Populates the common code with INVALID_REQUEST and sets the response_timestamp.
///
/// \param      error_message   Error message to set in response.
/// \param[out] response        Response whose common header is updated by this call.
/// \return  Always returns false.
template <class ResponseType>
bool SetInvalidRequest(const std::string& error_message, ResponseType* response) {
    return SetInvalidRequest(error_message, response->mutable_header());
}

/// Call at the end of handling a GRPC request to indicate that there was an internal error.
/// Populates the common code with INTERNAL and sets the response_timestamp.
///
/// \param      error_message   Error message to set in response.
/// \param[out] out_header      ResponseHeader to be updated by this call.
/// \return  Always returns false.
bool SetInternalError(const std::string& error_message, ::bosdyn::api::ResponseHeader* out_header);

/// Call at the end of handling a GRPC request to indicate that there was an internal error.
/// Populates the common code with INTERNAL and sets the response_timestamp.
///
/// \param      error_message   Error message to set in response.
/// \param[out] out_header      Response whose common header updated by this call.
/// \return  Always returns false.
template <class ResponseType>
bool SetInternalError(const std::string& error_message, ResponseType* response) {
    return SetInternalError(error_message, response->mutable_header());
}

/// Call at the start of GRPC to validate the request_header and initialize the response header.
///
/// In out_response_header, set request_received_nsec, and set request if reflected_request is not
/// nullptr.  Then, the request header is validated.  If the request header has no timestamp,
/// the timestamp is zero or negative, or no client name is given, then out_response_header is
/// filled-out to report the error.  If the request header did not validate, this returns false
/// and the response is ready to be returned.  If the request header was validated, this returns
/// true.
///
/// \param      request_header     RequestHeader to be copied into out_response_header.
/// \param      reflected_request  If not null, request to be copied into out_response_header.
/// \param[out] out_header         ResponseHeader to be updated by this call.
/// \return     False if header was not validated, true if validated.
bool ValidateRequestHeaderAndRespond(const ::bosdyn::api::RequestHeader& request_header,
                                     const ::google::protobuf::Message* reflected_request,
                                     ::bosdyn::api::ResponseHeader* out_header);

/// Enumeration to determine whether ResponseHeader should be populated with copy of request.
enum RequestReflectOption {
    /// ResponseHeader should not be populated with a copy of request.
    NO_REQUEST_COPY = 0,
    /// ResponseHeader should be populated with a copy of request.
    REQUEST_COPY = 1,
};

/// Call at the start of GRPC to validate the request_header and initialize the response header.
///
/// This is called on a request proto message with a RequestHeader field 'header', and a
/// response message with a ResponseHeader field 'header'.
/// If request has 'header' filled in, it validates the header and fills in the response.
///
/// \param      request                Request proto.
/// \param      request_reflect_option Specifies whether request will be copied into header.
/// \param[out] out_response           Response, whose header is to be updated by this call.
/// \return     False if header was not validated, true if validated.
template <class RequestType, class ResponseType>
bool ValidateRequestHeaderAndRespond(const RequestType& request,
                                     RequestReflectOption response_reflect_option,
                                     ResponseType* out_response) {
    if (!request.has_header()) {
        SetTimestamp(::bosdyn::common::NowNsec(),
                     out_response->mutable_header()->mutable_request_received_timestamp());
        if (REQUEST_COPY == response_reflect_option) {
            out_response->mutable_header()->mutable_request()->PackFrom(request);
        }
        return SetInvalidRequest("No header message present in request",
                                 out_response->mutable_header());
    }
    const google::protobuf::Message* reflected_request =
        (REQUEST_COPY == response_reflect_option ? &request : nullptr);
    return ValidateRequestHeaderAndRespond(request.header(), reflected_request,
                                           out_response->mutable_header());
}

}  // namespace common

}  // namespace bosdyn
