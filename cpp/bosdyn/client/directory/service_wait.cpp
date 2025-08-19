/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "service_wait.h"
#include "bosdyn/common/success_condition.h"

namespace bosdyn::directory::util {

// Error category for WaitError.
class WaitCategory : public std::error_category {
 public:
    const char* name() const noexcept override { return "wait_error"; }
    std::string message(int ev) const override {
        switch (static_cast<WaitError>(ev)) {
            case WaitError::kSuccess:
                return "Success";
            case WaitError::kTimeout:
                return "Timeout";
        }
        return "Unknown";
    }
    std::error_condition default_error_condition(int ev) const noexcept override {
        return std::error_condition(ev, *this);
    }
    bool equivalent(int code, const std::error_condition& condition) const noexcept override {
        if (condition == SuccessCondition::Success) {
            return code == static_cast<int>(WaitError::kSuccess);
        }
        return false;
    }
};

const WaitCategory s_wait_category{};

std::error_code make_error_code(WaitError value) {
    return {static_cast<int>(value), s_wait_category};
}

WaitResult WaitForAllServices(const std::set<std::string>& service_names,
                              bosdyn::client::Robot& robot, const bosdyn::common::Duration& timeout,
                              const bosdyn::common::Duration& interval) {
    auto start_time = bosdyn::common::NowTimePoint();
    auto end_time = start_time + timeout;
    WaitResult result;

    do {
        bosdyn::client::Result<std::vector<::bosdyn::api::ServiceEntry>> list_result =
            robot.ListServices();
        if (!list_result) {
            if (list_result.status.code() != RetryableRPCCondition::Retryable) {
                // Return immediately if the error is not retryable.
                result.status = list_result.status;
                return result;
            }
            // Retryable error, keep trying.
        } else {
            result.missing_services = service_names;
            for (const api::ServiceEntry& entry : list_result.response) {
                result.missing_services.erase(entry.name());
            }
            if (result.missing_services.empty()) {
                result.status = {WaitError::kSuccess};
                return result;
            }
        }
        std::this_thread::sleep_for(interval);
    } while (bosdyn::common::NowTimePoint() < end_time);

    // Failed to find all services.  Return the ones that are missing according to the latest set of
    // found services.
    result.status = {WaitError::kTimeout};
    return result;
}

}  // namespace bosdyn::directory::util
