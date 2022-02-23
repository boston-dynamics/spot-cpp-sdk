/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "common_response_processor.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/error_codes/header_error_code.h"

namespace bosdyn {

namespace client {

::bosdyn::common::Status CommonResponseProcessor::Process(
    const grpc::Status& status, const ::bosdyn::api::ResponseHeader& response_header,
    const ::google::protobuf::Message& full_response) {
    const ::bosdyn::api::CommonError& error = response_header.error();
    return ::bosdyn::common::Status(error.code(), error.message());
}

}  // namespace client

}  // namespace bosdyn
