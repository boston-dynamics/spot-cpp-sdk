/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

namespace bosdyn {

namespace client {

namespace {  // anonymous namespace

struct SDKErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool SDKErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* SDKErrorCodeCategory::name() const noexcept { return "SDKErrorCode"; }

std::string SDKErrorCodeCategory::message(int value) const {
    switch (static_cast<SDKErrorCode>(value)) {
        case SDKErrorCode::Success:
            return "Success";
        case SDKErrorCode::GenericSDKError:
            return "GenericSDKError";
    }
    return "(SDKErrorCode: unrecognized error)";
}

const SDKErrorCodeCategory SDKErrorCodeCategory_category{};

}  // anonymous namespace

std::error_code make_error_code(SDKErrorCode value) {
    return {static_cast<int>(value), SDKErrorCodeCategory_category};
}

}  // namespace client

}  // namespace bosdyn
