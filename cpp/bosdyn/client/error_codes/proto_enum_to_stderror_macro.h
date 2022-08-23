/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <algorithm>
#include <system_error>

// This macro goes in some helper file.
#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ns, ns_def, ns_def_parenthesis) \
    namespace std {                                                                   \
    template <>                                                                       \
    struct is_error_code_enum<ns::enumclass> : public true_type {};                   \
    }                                                                                 \
    ns_def std::error_code make_error_code(ns::enumclass);                            \
    ns_def_parenthesis

// Define wrapper for macro above for each namespace, so it is easier to call the macro
#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_API(enumclass)              \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, bosdyn::api,         \
                                       namespace bosdyn{namespace api{ \
                                           ,                           \
                                       }})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_AUTO_RETURN(enumclass)                            \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::auto_return,                  \
                                       namespace bosdyn{namespace api{namespace auto_return{ \
                                           ,                                                 \
                                       }}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_AUTOWALK(enumclass)                            \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::autowalk,                  \
                                       namespace bosdyn{namespace api{namespace autowalk{ \
                                           ,                                                 \
                                       }}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_SPOT(enumclass)                            \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::spot,                  \
                                       namespace bosdyn{namespace api{namespace spot{ \
                                           ,                                          \
                                       }}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_MISSION(enumclass)                            \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::mission,                  \
                                       namespace bosdyn{namespace api{namespace mission{ \
                                           ,                                             \
                                       }}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_GRAPHNAV(enumclass)                             \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::graph_nav,                  \
                                       namespace bosdyn{namespace api{namespace graph_nav{ \
                                           ,                                               \
                                       }}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_METRICS(enumclass)                                    \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::metrics_logging,                  \
                                       namespace bosdyn{namespace api{namespace metrics_logging{ \
                                           ,                                                     \
                                       }}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_INTERNAL(enumclass)                            \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::internal,                  \
                                       namespace bosdyn{namespace api{namespace internal{ \
                                           ,                                              \
                                       }}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_LOCALNAV(enumclass)                                               \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::internal::localnav,                           \
                                       namespace bosdyn{namespace api{namespace internal{namespace localnav{ \
                                           ,                                                                 \
                                       }}}})

#define DEFINE_PROTO_ENUM_ERRORCODE_HEADER_DOCKING(enumclass)                            \
    DEFINE_PROTO_ENUM_ERRORCODE_HEADER(enumclass, ::bosdyn::api::docking,                  \
                                       namespace bosdyn{namespace api{namespace docking{ \
                                           ,                                             \
                                       }}})
