/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/error_codes/joint_control_stream_error_code.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/common/success_condition.h"

namespace {  // anonymous namespace

struct JointControlStreamErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool JointControlStreamErrorCodeCategory::equivalent(
    int valcode, const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success) return (valcode == 0);
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* JointControlStreamErrorCodeCategory::name() const noexcept {
    return "JointControlStreamErrorCode";
}

std::string JointControlStreamErrorCodeCategory::message(int value) const {
    switch (static_cast<JointControlStreamErrorCode>(value)) {
        case JointControlStreamErrorCode::Success:
            return "Success";
        case JointControlStreamErrorCode::RequestWriterFailed:
            return "Getting request writer failed";
        case JointControlStreamErrorCode::ResponseReaderFailed:
            return "Getting response reader failed";
        case JointControlStreamErrorCode::StreamingFailed:
            return "Streaming for joint control failed";
    }
    return "(JointControlStreamErrorCode: unrecognized error)";
}

const JointControlStreamErrorCodeCategory JointControlStreamErrorCodeCategory_category{};

}  // anonymous namespace

std::error_code make_error_code(JointControlStreamErrorCode value) {
    return {static_cast<int>(value), JointControlStreamErrorCodeCategory_category};
}
