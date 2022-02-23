/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/directory.pb.h>

#include <string>

namespace bosdyn {

namespace client {
namespace directory {
namespace util {

/// Create a service endpoint object.
::bosdyn::api::Endpoint MakeEndpoint(const std::string& host_name, int grpc_port);

}  // namespace util
}  // namespace directory
}  // namespace client

}  // namespace bosdyn
