/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

// This macro goes in some helper file.
#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ns, ns_def, ns_def_parenthesis,             \
                                         equivalent_condition)                                  \
    namespace {                                                                                 \
    struct enumclass##Category : public std::error_category {                                   \
        const char* name() const noexcept override { return #enumclass; }                       \
        std::string message(int ev) const override {                                            \
            return ns::enumclass##_Name(static_cast<ns::enumclass>(ev));                        \
        }                                                                                       \
        bool equivalent(int valcode, const std::error_condition& cond) const noexcept override; \
    };                                                                                          \
    bool enumclass##Category::equivalent(int valcode,                                           \
                                         const std::error_condition& cond) const noexcept {     \
        if (cond == SuccessCondition::Success) {                                                \
            if (equivalent_condition) return true;                                              \
        } else if (cond == ErrorTypeCondition::ResponseError) {                                 \
            return true;                                                                        \
        }                                                                                       \
        return false;                                                                           \
    }                                                                                           \
    const enumclass##Category enumclass##Category_category{};                                   \
    }                                                                                           \
    ns_def std::error_code make_error_code(enumclass value) {                                   \
        return {static_cast<int>(value), enumclass##Category_category};                         \
    }                                                                                           \
    ns_def_parenthesis

// Define wrapper for macro above for each namespace, so it is easier to call the macro
#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(enumclass, equivalent_condition) \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, bosdyn::api,                  \
                                     namespace bosdyn{namespace api{          \
                                         ,                                    \
                                     }},                                      \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_AUTO_RETURN(enumclass, equivalent_condition)      \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::auto_return,                  \
                                     namespace bosdyn{namespace api{namespace auto_return{ \
                                         ,                                                 \
                                     }}},                                                  \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_AUTOWALK(enumclass, equivalent_condition)      \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::autowalk,                  \
                                     namespace bosdyn{namespace api{namespace autowalk{ \
                                         ,                                                 \
                                     }}},                                                  \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_SPOT(enumclass, equivalent_condition)      \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::spot,                  \
                                     namespace bosdyn{namespace api{namespace spot{ \
                                         ,                                          \
                                     }}},                                           \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_MISSION(enumclass, equivalent_condition)      \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::mission,                  \
                                     namespace bosdyn{namespace api{namespace mission{ \
                                         ,                                             \
                                     }}},                                              \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_GRAPHNAV(enumclass, equivalent_condition)       \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::graph_nav,                  \
                                     namespace bosdyn{namespace api{namespace graph_nav{ \
                                         ,                                               \
                                     }}},                                                \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_METRICS(enumclass, equivalent_condition)              \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::metrics_logging,                  \
                                     namespace bosdyn{namespace api{namespace metrics_logging{ \
                                         ,                                                     \
                                     }}},                                                      \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_INTERNAL(enumclass, equivalent_condition)      \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::internal,                  \
                                     namespace bosdyn{namespace api{namespace internal{ \
                                         ,                                              \
                                     }}},                                               \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_LOCALNAV(enumclass, equivalent_condition)                         \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::internal::localnav,                           \
                                     namespace bosdyn{namespace api{namespace internal{namespace localnav{ \
                                         ,                                                                 \
                                     }}}},                                                                 \
                                     equivalent_condition)

#define DEFINE_PROTO_ENUM_ERRORCODE_IMPL_DOCKING(enumclass, equivalent_condition)      \
    DEFINE_PROTO_ENUM_ERRORCODE_IMPL(enumclass, ::bosdyn::api::docking,                  \
                                     namespace bosdyn{namespace api{namespace docking{ \
                                         ,                                             \
                                     }}},                                              \
                                     equivalent_condition)
