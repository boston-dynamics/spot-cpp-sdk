/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "response_processor_chain.h"
#include "response_processor.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/common/assert_precondition.h"

namespace bosdyn {

namespace client {

void ResponseProcessorChain::AppendProcessor(
    const std::shared_ptr<ResponseProcessor>& processor) {
    // Cannot have a null response processor.
    BOSDYN_ASSERT_PRECONDITION(processor, "Cannot append a null response processor.");
    m_processors.push_back(processor);
}

void ResponseProcessorChain::PrependProcessor(
    const std::shared_ptr<ResponseProcessor>& processor) {
    // Cannot have a null response processor.
    BOSDYN_ASSERT_PRECONDITION(processor, "Cannot prepend a null response processor.");
    m_processors.push_front(processor);
}

::bosdyn::common::Status ResponseProcessorChain::Process(
     const grpc::Status& status, const ::bosdyn::api::ResponseHeader& response_header,
     const ::google::protobuf::Message& full_response) {
    ::bosdyn::common::Status ret_status = ::bosdyn::common::Status(SDKErrorCode::Success);
    for (const auto& processor : m_processors) {
        ret_status = processor->Process(status, response_header, full_response);
        if (!ret_status) {
            // Stop after the first detected error
            return ret_status;
        }
    }
    return ret_status;
}

}  // namespace client

}  // namespace bosdyn
