/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/graph_nav/graph_nav_error_codes.h"
#include "bosdyn/client/error_codes/proto_enum_to_stderror_macro_source.h"

// Accept STATUS_UNKNOWN (code was introduced in 3.1)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(ClearGraphResponse_Status, valcode == 1 || valcode == 0)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(SetLocalizationResponse_Status, valcode == 1)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(NavigateRouteResponse_Status, valcode == 1)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(NavigateToResponse_Status, valcode == 1)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(NavigateToAnchorResponse_Status, valcode == 1)
// NOTE: NavigationFeedbackResponse is not converted to std::error_code on purpose.
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(DownloadWaypointSnapshotResponse_Status, valcode == 1)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(DownloadEdgeSnapshotResponse_Status, valcode == 1)
