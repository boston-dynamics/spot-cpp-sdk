/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/common/success_condition.h"

#include <iostream>

namespace {  // anonymous namespace

struct SuccessConditionCategory : public std::error_category {
 public:
    const char* name() const noexcept override;
    std::string message(int value) const override;
};

const char* SuccessConditionCategory::name() const noexcept { return "SuccessCondition"; }

std::string SuccessConditionCategory::message(int value) const {
    switch (static_cast<SuccessCondition>(value)) {
        case SuccessCondition::Success:
            return "Success";
        default:
            return "Fail";
    }
}

const SuccessConditionCategory SuccessConditionCategory_category{};

}  // anonymous namespace

std::error_condition make_error_condition(SuccessCondition value) {
    return {static_cast<int>(value), SuccessConditionCategory_category};
}
