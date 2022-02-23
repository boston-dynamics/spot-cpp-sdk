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

// ResponseProcessors process gRPC responses after they are received, but before ServiceClient
// specific handling happens.
//
// Implementations MUST be thread-safe.
class ResponseProcessor {
 public:
    virtual ~ResponseProcessor() = default;

    virtual ::bosdyn::common::Status Process(const grpc::Status& status, const ::bosdyn::api::ResponseHeader& response_header,
                           const ::google::protobuf::Message& full_response) = 0;
};

}  // namespace client

}  // namespace bosdyn
