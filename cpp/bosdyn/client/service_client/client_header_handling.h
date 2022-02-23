/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/header.pb.h>

#include "bosdyn/common/time.h"

namespace bosdyn {

namespace client {

/// Set the client name and request timestamp in the common header sub-message of the given request
/// message.
///
/// \param      client_name
/// \param[out] req             Request of which header should be populated.
template <class RequestType>
void SetRequestHeader(const std::string client_name, RequestType* req) {
    ::bosdyn::common::SetTimestamp(::bosdyn::common::NowNsec(),
                                 req->mutable_header()->mutable_request_timestamp());
    req->mutable_header()->set_client_name(client_name);
}

}  // namespace client

}  // namespace bosdyn
