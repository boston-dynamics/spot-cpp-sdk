/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "request_processor.h"

namespace bosdyn {

namespace client {

class AppTokenRequestProcessor : public RequestProcessor {
 public:
    explicit AppTokenRequestProcessor(const std::string& app_token);
    ~AppTokenRequestProcessor() = default;

 private:
    ::bosdyn::common::Status Process(grpc::ClientContext* context, ::bosdyn::api::RequestHeader* request_header,
                   ::google::protobuf::Message* full_request);

    std::string m_app_token;
};

}  // namespace client

}  // namespace bosdyn
