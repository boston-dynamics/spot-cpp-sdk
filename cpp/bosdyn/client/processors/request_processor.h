/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/header.pb.h>
#include <grpcpp/grpcpp.h>
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

// RequestProcessors process gRPC requests before the requests are actually issued over the wire.
//
// Implementations MUST be thread-safe.
class RequestProcessor {
 public:
    virtual ~RequestProcessor() = default;

    // Process a request before it is sent over the wire.
    virtual ::bosdyn::common::Status Process(grpc::ClientContext* context, ::bosdyn::api::RequestHeader* request_header,
                           ::google::protobuf::Message* full_request) = 0;
};

}  // namespace client

}  // namespace bosdyn
