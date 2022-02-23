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

// RequestHeaderCreator sets common RequestHeaders. These RequestHeaders are included in most Boston
// Dynamics API RPCs.
class CommonRequestProcessor : public RequestProcessor {
 public:
    // Constructs a RequestHeaderCreator. |client_name| will be set in each header, and ideally
    // contains both a string identifying a particular binary, as well as an instance of a binary
    // (such as a PID or GUID).
    explicit CommonRequestProcessor(const std::string& client_name);
    ~CommonRequestProcessor() = default;

    ::bosdyn::common::Status Process(grpc::ClientContext* context, ::bosdyn::api::RequestHeader* request_header,
                   ::google::protobuf::Message* full_request) override;

 private:
    std::string m_client_name;
};

}  // namespace client

}  // namespace bosdyn
