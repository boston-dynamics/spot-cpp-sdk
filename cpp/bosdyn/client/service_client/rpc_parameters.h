/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once
#include "bosdyn/common/time.h"

namespace bosdyn{

namespace client{

// For client requests, parameters of this type can request that logging of GRPC request
// and response messages are disabled.
enum class LogRequestMode {
    kDisabled = 0,
    kEnabled = 1,
    kDefault = 2,
};

// Specifying the timeout defined below means the RPC timeout is not set and the default timeout value
// specified in the service client will be used.
const ::bosdyn::common::Duration kRPCTimeoutNotSpecified = std::chrono::milliseconds(-1);

// Default timeout used for all RPCs in the service clients. The Robot or each client can change
// this value in their own instantiation.
const ::bosdyn::common::Duration kDefaultRPCTimeout = std::chrono::seconds(30);

struct RPCParameters {
    // kDefault means that the field disable_rpc_logging in the message headers will keep the
    // default proto value.
    LogRequestMode logging_control = LogRequestMode::kDefault;

    // kRPCTimeoutNotSpecified is used when timeout is not specified, meaning the default value
    // specified in ServiceClient will be used.
    ::bosdyn::common::Duration timeout = kRPCTimeoutNotSpecified;
};

}  // namespace client

}  // namespace bosdyn

