/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <list>
#include <memory>
#include <bosdyn/api/header.pb.h>
#include "bosdyn/common/status.h"

namespace grpc {
class Status;
}

namespace google {
namespace protobuf {
class Message;
}
}  // namespace google

namespace bosdyn {

namespace client {

namespace api {
class ResponseHeader;
}

class ResponseProcessor;

class ResponseProcessorChain {
 public:
    ResponseProcessorChain() = default;
    ResponseProcessorChain(const ResponseProcessorChain& other) = default;
    ~ResponseProcessorChain() = default;

    ResponseProcessorChain& operator=(const ResponseProcessorChain& other) = default;

    void AppendProcessor(const std::shared_ptr<ResponseProcessor>& processor);

    void PrependProcessor(const std::shared_ptr<ResponseProcessor>& processor);

    ::bosdyn::common::Status Process(const grpc::Status& status, const ::bosdyn::api::ResponseHeader& response_header,
                   const ::google::protobuf::Message& full_response);

 private:
    std::list<std::shared_ptr<ResponseProcessor>> m_processors;
};

}  // namespace client

}  // namespace bosdyn
