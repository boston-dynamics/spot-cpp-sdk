/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <google/protobuf/duration.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include "bosdyn/client/service_client/result.h"

namespace bosdyn {

namespace client {

typedef Result<std::shared_ptr<std::string>> StringResultType;

typedef Result<std::shared_ptr<::google::protobuf::Duration>> DurationResultType;

typedef Result<std::shared_ptr<::google::protobuf::Timestamp>> TimestampResultType;

}  // namespace client

}  // namespace bosdyn
