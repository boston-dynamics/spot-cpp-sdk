/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/docking_helper_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

namespace {  // anonymous namespace

struct DockingHelperErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool DockingHelperErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* DockingHelperErrorCodeCategory::name() const noexcept { return "DockingHelperErrorCode"; }

std::string DockingHelperErrorCodeCategory::message(int value) const {
    switch (static_cast<DockingHelperErrorCode>(value)) {
        case DockingHelperErrorCode::RetriesExceeded:
            return "Retries exceeded";
        case DockingHelperErrorCode::Cancelled:
            return "Cancelled";
        case DockingHelperErrorCode::CommandFailed:
            return "Docking failed";
    }
    return "(DockingHelperErrorCode: unrecognized error)";
}

const DockingHelperErrorCodeCategory DockingHelperErrorCodeCategory_category{};

}  // anonymous namespace

std::error_code make_error_code(DockingHelperErrorCode value) {
    return {static_cast<int>(value), DockingHelperErrorCodeCategory_category};
}
