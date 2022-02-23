/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <iostream>

#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"

namespace {  // anonymous namespace

struct ErrorTypeConditionCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
};

const char* ErrorTypeConditionCategory::name() const noexcept { return "ErrorTypeCondition"; }

std::string ErrorTypeConditionCategory::message(int value) const {
    switch (static_cast<ErrorTypeCondition>(value)) {
        case ErrorTypeCondition::ResponseError:
            return "ResponseError";
        case ErrorTypeCondition::RPCError:
            return "RPCError";
        case ErrorTypeCondition::SDKError:
            return "SDKError";
    }
    return "(ErrorTypeCondition: unrecognized error)";
}
const ErrorTypeConditionCategory ErrorTypeConditionCategory_category{};

}  // anonymous namespace

std::error_condition make_error_condition(ErrorTypeCondition value) {
    return {static_cast<int>(value), ErrorTypeConditionCategory_category};
}
