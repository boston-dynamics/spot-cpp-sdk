/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/client_creation_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

namespace {  // anonymous namespace

struct ClientCreationErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool ClientCreationErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* ClientCreationErrorCodeCategory::name() const noexcept { return "ClientCreationErrorCode"; }

std::string ClientCreationErrorCodeCategory::message(int value) const {
    switch (static_cast<ClientCreationErrorCode>(value)) {
        case ClientCreationErrorCode::Success:
            return "Success";
        case ClientCreationErrorCode::IncorrectServiceType:
            return "IncorrectServiceType";
        case ClientCreationErrorCode::UnregisteredService:
            return "UnregisteredService";
        default:
            return "(ClientCreationErrorCode: unrecognized error)";
    }
}

const ClientCreationErrorCodeCategory ClientCreationErrorCodeCategory_category{};

}  // anonymous namespace

std::error_code make_error_code(ClientCreationErrorCode value) {
    return {static_cast<int>(value), ClientCreationErrorCodeCategory_category};
}
