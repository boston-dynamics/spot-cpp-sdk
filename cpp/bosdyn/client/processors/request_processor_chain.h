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
#include <list>
#include "bosdyn/common/status.h"

namespace google {
namespace protobuf {
class Message;
}
}  // namespace google

namespace bosdyn {

namespace client {

namespace api {
class RequestHeader;
}

class RequestProcessor;

class RequestProcessorChain {
 public:
    RequestProcessorChain() = default;
    RequestProcessorChain(const RequestProcessorChain& other) = default;
    ~RequestProcessorChain() = default;

    RequestProcessorChain& operator=(const RequestProcessorChain& other) = default;

    void AppendProcessor(const std::shared_ptr<RequestProcessor>& processor);

    void PrependProcessor(const std::shared_ptr<RequestProcessor>& processor);

    // Run all processors over a new request.
    ::bosdyn::common::Status Process(grpc::ClientContext* context, ::bosdyn::api::RequestHeader* request_header,
                   ::google::protobuf::Message* full_request);

 private:
    std::list<std::shared_ptr<RequestProcessor>> m_processors;
};

}  // namespace client

}  // namespace bosdyn
