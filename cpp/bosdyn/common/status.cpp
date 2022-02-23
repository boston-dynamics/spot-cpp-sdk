/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/common/status.h"
#include "bosdyn/common/success_condition.h"

#include <sstream>

namespace bosdyn {

namespace common {

Status::Status(const std::error_code& code, std::string message)
    : m_code(code), m_message(std::move(message)) {}


Status Status::Chain(std::string message) const {
    message += ": ";
    message += m_message;
    return Status(m_code, std::move(message));
}

Status Status::Chain(std::error_code code, std::string message) const {
    message += ": ";
    message += DebugString();
    return Status(code, std::move(message));
}

std::string Status::DebugString() const {
    std::string result = std::to_string(m_code.value());
    result += "(";
    result += m_code.message();
    result += "): ";
    result += m_message;
    return result;
}

Status::operator bool() const {
    // ::bosdyn::common::Status is the opposite of an std::error_code, meaning if error_code is set to
    // success/0, which casts to false, ::bosdyn::common::Status will be set to true. This makes it more
    // intuitive if it is used as the return type of a method
    return (m_code == SuccessCondition::Success);
}

}  // namespace common

}  // namespace bosdyn
