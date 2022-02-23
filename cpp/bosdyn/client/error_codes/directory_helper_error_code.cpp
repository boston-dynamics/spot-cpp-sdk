/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/directory_helper_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

namespace {  // anonymous namespace

struct DirectoryHelperErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool DirectoryHelperErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* DirectoryHelperErrorCodeCategory::name() const noexcept { return "DirectoryHelperErrorCode"; }

std::string DirectoryHelperErrorCodeCategory::message(int value) const {
    switch (static_cast<DirectoryHelperErrorCode>(value)) {
        case DirectoryHelperErrorCode::NonExistentServiceName:
            return "NonExistentServiceName";
        default:
            return "(DirectoryHelperErrorCode: unrecognized error)";
    }
}

const DirectoryHelperErrorCodeCategory DirectoryHelperErrorCodeCategory_category{};

}  // anonymous namespace

std::error_code make_error_code(DirectoryHelperErrorCode value) {
    return {static_cast<int>(value), DirectoryHelperErrorCodeCategory_category};
}
