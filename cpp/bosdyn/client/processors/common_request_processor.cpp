/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "common_request_processor.h"

#include <string>

#include "bosdyn/common/compiler_definitions.h"
#include "bosdyn/common/time.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

namespace bosdyn {

namespace client {

CommonRequestProcessor::CommonRequestProcessor(const std::string& client_name)
    : m_client_name(client_name) {}

::bosdyn::common::Status CommonRequestProcessor::Process(
    grpc::ClientContext* context BOSDYN_UNUSED, ::bosdyn::api::RequestHeader* request_header,
    ::google::protobuf::Message* full_request BOSDYN_UNUSED) {
    request_header->set_client_name(m_client_name);
    ::bosdyn::common::SetTimestamp(::bosdyn::common::NowNsec(),
                                 request_header->mutable_request_timestamp());
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

}  // namespace client

}  // namespace bosdyn
