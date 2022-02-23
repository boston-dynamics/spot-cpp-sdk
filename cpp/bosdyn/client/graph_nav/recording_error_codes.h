/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <bosdyn/api/graph_nav/recording.pb.h>
#include "bosdyn/client/error_codes/proto_enum_to_stderror_macro.h"

DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(StartRecordingResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(StopRecordingResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(CreateWaypointResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(CreateEdgeResponse_Status)
