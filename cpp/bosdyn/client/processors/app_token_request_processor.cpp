/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "app_token_request_processor.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

namespace bosdyn {

namespace client {

AppTokenRequestProcessor::AppTokenRequestProcessor(const std::string& app_token)
    : m_app_token(app_token) {}

::bosdyn::common::Status AppTokenRequestProcessor::Process(
    grpc::ClientContext* context, ::bosdyn::api::RequestHeader* request_header,
    ::google::protobuf::Message* full_request) {
    context->AddMetadata("x-bosdyn-apptoken", m_app_token);
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

}  // namespace client

}  // namespace bosdyn
