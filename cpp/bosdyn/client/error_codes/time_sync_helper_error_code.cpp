/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/time_sync_helper_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/common/success_condition.h"

namespace bosdyn {

namespace client {

namespace {

struct TimeSyncHelperErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool TimeSyncHelperErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* TimeSyncHelperErrorCodeCategory::name() const noexcept { return "TimeSyncHelperErrorCode"; }

std::string TimeSyncHelperErrorCodeCategory::message(int value) const {
    switch (static_cast<TimeSyncHelperErrorCode>(value)) {
        case TimeSyncHelperErrorCode::ClockIdentifierUnset:
            return "ClockIdentifierUnset";
    }
    return "(TimeSyncHelperErrorCode: unrecognized error)";
}

const TimeSyncHelperErrorCodeCategory TimeSyncHelperErrorCodeCategory_category{};


struct EstablishTimeSyncErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool EstablishTimeSyncErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* EstablishTimeSyncErrorCodeCategory::name() const noexcept { return "EstablishTimeSyncErrorCode"; }

std::string EstablishTimeSyncErrorCodeCategory::message(int value) const {
    switch (static_cast<EstablishTimeSyncErrorCode>(value)) {
        case EstablishTimeSyncErrorCode::UnableToEstablishTimeSync:
            return "UnableToEstablishTimeSync";
    }
    return "(EstablishTimeSyncErrorCode: unrecognized error)";
}

const EstablishTimeSyncErrorCodeCategory EstablishTimeSyncErrorCodeCategory_category{};

struct RobotTimeSyncErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool RobotTimeSyncErrorCodeCategory::equivalent(int valcode,
                                      const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* RobotTimeSyncErrorCodeCategory::name() const noexcept { return "RobotTimeSyncErrorCode"; }

std::string RobotTimeSyncErrorCodeCategory::message(int value) const {
    switch (static_cast<RobotTimeSyncErrorCode>(value)) {
        case RobotTimeSyncErrorCode::PreviousTimeSyncUnavailableYet:
            return "PreviousTimeSyncUnavailableYet";
    }
    return "(RobotTimeSyncErrorCode: unrecognized error)";
}

const RobotTimeSyncErrorCodeCategory RobotTimeSyncErrorCodeCategory_category{};

} // anonymous namespace

std::error_code make_error_code(RobotTimeSyncErrorCode value) {
    return {static_cast<int>(value), RobotTimeSyncErrorCodeCategory_category};
}

std::error_code make_error_code(TimeSyncHelperErrorCode value) {
    return {static_cast<int>(value), TimeSyncHelperErrorCodeCategory_category};
}

std::error_code make_error_code(EstablishTimeSyncErrorCode value) {
    return {static_cast<int>(value), EstablishTimeSyncErrorCodeCategory_category};
}

}  // namespace client

}  // namespace bosdyn
