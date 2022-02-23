/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "request_processor_chain.h"
#include "request_processor.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/common/assert_precondition.h"

namespace bosdyn {

namespace client {

void RequestProcessorChain::AppendProcessor(const std::shared_ptr<RequestProcessor>& processor) {
    // Cannot have a null request processor.
    BOSDYN_ASSERT_PRECONDITION(processor, "Cannot append a null request processor.");
    m_processors.push_back(processor);
}

void RequestProcessorChain::PrependProcessor(const std::shared_ptr<RequestProcessor>& processor) {
    // Cannot have a null request processor.
    BOSDYN_ASSERT_PRECONDITION(processor, "Cannot append a null request processor.");
    m_processors.push_front(processor);
}

::bosdyn::common::Status RequestProcessorChain::Process(
    grpc::ClientContext* context, ::bosdyn::api::RequestHeader* request_header,
    ::google::protobuf::Message* full_request) {
    ::bosdyn::common::Status status = ::bosdyn::common::Status(SDKErrorCode::Success);
    for (const auto& processor : m_processors) {
        status = processor->Process(context, request_header, full_request);
        if (!status) {
            // Stop after the first detected error
            return status;
        }
    }
    return status;
}

}  // namespace client

}  // namespace bosdyn
