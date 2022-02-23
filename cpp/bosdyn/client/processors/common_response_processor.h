/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "response_processor.h"

namespace bosdyn {

namespace client {

// ResponseHeaderProcessor handles common gRPC response information used across most of the Boston
// Dynamics API.
//
// API client code will typically have the ResponseHeaderProcess process a response first, followed
// by RPC-specific handling.
class CommonResponseProcessor : public ResponseProcessor {
 public:
    CommonResponseProcessor() = default;
    ~CommonResponseProcessor() = default;

    ::bosdyn::common::Status Process(const grpc::Status& status, const ::bosdyn::api::ResponseHeader& response_header,
                           const ::google::protobuf::Message& full_response) override;
};

}  // namespace client

}  // namespace bosdyn
