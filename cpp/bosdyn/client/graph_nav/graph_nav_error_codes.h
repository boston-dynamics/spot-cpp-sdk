/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <bosdyn/api/graph_nav/graph_nav.pb.h>
#include "bosdyn/client/error_codes/proto_enum_to_stderror_macro.h"

DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(ClearGraphResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(SetLocalizationResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(NavigateRouteResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(NavigateToResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(NavigateToAnchorResponse_Status)
// NOTE: NavigationFeedbackResponse is not converted to std::error_code on purpose.
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(DownloadWaypointSnapshotResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(DownloadEdgeSnapshotResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(UploadWaypointSnapshotResponse_Status)
DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(UploadGraphResponse_Status)