/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/lease_wallet_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

namespace bosdyn {

namespace client {

namespace {  // anonymous namespace

struct LeaseSystemErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool LeaseSystemErrorCodeCategory::equivalent(int valcode,
                                              const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* LeaseSystemErrorCodeCategory::name() const noexcept { return "LeaseWalletErrorCode"; }

std::string LeaseSystemErrorCodeCategory::message(int value) const {
    switch (static_cast<LeaseWalletErrorCode>(value)) {
        case LeaseWalletErrorCode::Success:
            return "Success";
        case LeaseWalletErrorCode::ResourceNotInWalletError:
            return "ResourceNotInWalletError";
        case LeaseWalletErrorCode::ResourceNotOwnedError:
            return "ResourceNotOwnedError";
        case LeaseWalletErrorCode::LeaseInvalidError:
            return "LeaseInvalidError";
        case LeaseWalletErrorCode::GenericLeaseError:
            return "GenericLeaseError";
    }
    return "(LeaseWalletErrorCode: unrecognized error)";
}

const LeaseSystemErrorCodeCategory LeaseSystemErrorCodeCategory_category{};

}  // anonymous namespace

std::error_code make_error_code(LeaseWalletErrorCode value) {
    return {static_cast<int>(value), LeaseSystemErrorCodeCategory_category};
}
const std::error_category& LeaseWalletErrorCategory() {
    return LeaseSystemErrorCodeCategory_category;
}

}  // namespace client

}  // namespace bosdyn
