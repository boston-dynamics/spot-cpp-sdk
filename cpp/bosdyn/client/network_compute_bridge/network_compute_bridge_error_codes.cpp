/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/network_compute_bridge//network_compute_bridge_error_codes.h"
#include "bosdyn/client/error_codes/proto_enum_to_stderror_macro_source.h"

DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(NetworkComputeStatus, valcode == 1)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(ListAvailableModelsStatus, valcode == 1)
