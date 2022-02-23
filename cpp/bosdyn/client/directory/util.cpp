/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "util.h"

#include <string>

namespace bosdyn {

namespace client {
namespace directory {
namespace util {

::bosdyn::api::Endpoint MakeEndpoint(const std::string& host_name, const int grpc_port) {
    ::bosdyn::api::Endpoint endpoint;
    endpoint.set_host_ip(host_name);
    endpoint.set_port(grpc_port);
    return endpoint;
}

}  // namespace util
}  // namespace directory
}  // namespace client

}  // namespace bosdyn
