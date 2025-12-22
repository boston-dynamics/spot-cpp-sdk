/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <algorithm>
#include <system_error>

// This macro goes in some helper file.
#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ns)             \
    namespace std {                                                   \
    template <>                                                       \
    struct is_error_code_enum<::ns::enumclass> : public true_type {}; \
    }                                                                 \
    namespace ns {                                                    \
    std::error_code make_error_code(::ns::enumclass);                 \
    }

// Define wrapper for macro above for each namespace, so it is easier to call the macro
#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_API(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_AUTO_RETURN(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::auto_return)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_AUTOWALK(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::autowalk)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_KEEPALIVE(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::keepalive)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_LOG_STATUS(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::log_status)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_SPOT(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::spot)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_MISSION(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::mission)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::graph_nav)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_METRICS(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::metrics_logging)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_INTERNAL(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::internal)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_LOCALNAV(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::internal::localnav)

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_DOCKING(enumclass) \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api::docking)
