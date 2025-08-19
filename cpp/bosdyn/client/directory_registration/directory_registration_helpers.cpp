/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "directory_registration_helpers.h"

#include "bosdyn/client/fault/util.h"

#include <bosdyn/api/service_fault.pb.h>

namespace bosdyn {

namespace client {

DirectoryRegistrationKeepAlive::DirectoryRegistrationKeepAlive(
    DirectoryRegistrationClient* directory_registration_client,
    const ::bosdyn::api::ServiceEntry& service_entry, const ::bosdyn::api::Endpoint& endpoint,
    ::bosdyn::common::Duration rpc_interval, FaultClient* fault_client,
    std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)> error_callback,
    ::bosdyn::common::Duration registration_initial_retry_interval)
    : m_directory_registration_client(directory_registration_client),
      m_service_entry(service_entry),
      m_endpoint(endpoint),
      m_registration_interval(rpc_interval),
      m_registration_initial_retry_interval(registration_initial_retry_interval),
      m_thread(),
      m_periodic_thread_helper(std::make_unique<PeriodicThreadHelper>()),
      m_thread_stopped(true),
      m_fault_client(fault_client),
      m_error_callback(error_callback) {}


DirectoryRegistrationKeepAlive::~DirectoryRegistrationKeepAlive() {
    Shutdown();
    Unregister().IgnoreError();
}

void DirectoryRegistrationKeepAlive::Start(const std::vector<std::string>& fault_attributes) {
    m_thread_stopped = false;

    // Populate the registration fault with default values.
    m_service_fault = fault::util::MakeServiceFault(
        "Directory Registration Fault", m_service_entry.name(),
        "Service registration or update failed. Will automatically retry.",
        ::bosdyn::api::ServiceFault::SEVERITY_CRITICAL, fault_attributes);

    // Clear any existing service faults from previous keep-alives with this service.
    m_registration_fault_active = false;
    if (m_fault_client) {
        auto clear_result = m_fault_client->ClearServiceFault(m_service_fault.fault_id());
        if (!clear_result &&
            clear_result.response.status() !=
                ::bosdyn::api::ClearServiceFaultResponse::STATUS_FAULT_NOT_ACTIVE) {
            m_registration_fault_active = true;
        }
    }

    m_thread = std::thread(&DirectoryRegistrationKeepAlive::PeriodicReregisterThreadMethod, this);
}

bool DirectoryRegistrationKeepAlive::IsAlive() const { return !m_thread_stopped; }

void DirectoryRegistrationKeepAlive::Shutdown() {
    m_periodic_thread_helper->Stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

UnregisterServiceResultType DirectoryRegistrationKeepAlive::Unregister() {
    return m_directory_registration_client->UnregisterService(m_service_entry.name());
}

void DirectoryRegistrationKeepAlive::PeriodicReregisterThreadMethod() {

    ::bosdyn::common::Duration retry_interval = m_registration_initial_retry_interval;
    ::bosdyn::common::Duration wait_interval = m_registration_interval;

    // Set up register and update requests
    ::bosdyn::api::RegisterServiceRequest register_service_request;
    register_service_request.mutable_endpoint()->CopyFrom(m_endpoint);
    register_service_request.mutable_service_entry()->CopyFrom(m_service_entry);
    ::bosdyn::api::UpdateServiceRequest update_service_request;
    update_service_request.mutable_endpoint()->CopyFrom(m_endpoint);
    update_service_request.mutable_service_entry()->CopyFrom(m_service_entry);

    // Continually attempt to register and update the service.
    do {
        // Register service.
        auto res_register =
            m_directory_registration_client->RegisterService(register_service_request);
        // Successful registration. Service faults are automatically cleared for the new service
        // by the directory registration service.
        if (res_register) {
            m_registration_fault_active = false;
            wait_interval = m_registration_interval;
            continue;
        }
        // If registration failed in a bad way.
        if (res_register.status.code() !=
            ::bosdyn::api::RegisterServiceResponse::STATUS_ALREADY_EXISTS) {
            if (m_fault_client && m_fault_client->TriggerServiceFault(m_service_fault)) {
                m_registration_fault_active = true;
            }
            auto action = ErrorCallbackResult::kResumeNormalOperation;
            if (m_error_callback) {
                try {
                    action = m_error_callback(res_register.status);
                } catch (const std::exception& e) {
                    std::cerr << "Exception thrown in error callback: " << e.what() << std::endl;
                }
            }
            if (action == ErrorCallbackResult::kAbort) {
                break;
            }
            if (action == ErrorCallbackResult::kRetryImmediately) {
                wait_interval = std::chrono::seconds(0);
            } else if (action == ErrorCallbackResult::kRetryWithExponentialBackOff) {
                // Exponentially increase the retry interval.
                wait_interval = retry_interval;
                retry_interval = std::min(retry_interval * 2, m_registration_interval);
            } else {
                // Default action is to continue with the next iteration.
                wait_interval = m_registration_interval;
            }
            continue;
        }

        // The service was already registered. Update service instead.
        auto res_update = m_directory_registration_client->UpdateService(update_service_request);
        // If update failed.
        if (!res_update) {
            if (m_fault_client && m_fault_client->TriggerServiceFault(m_service_fault)) {
                m_registration_fault_active = true;
            }
            auto action = ErrorCallbackResult::kResumeNormalOperation;
            if (m_error_callback) {
                try {
                    action = m_error_callback(res_register.status);
                } catch (const std::exception& e) {
                    std::cerr << "Exception thrown in error callback: " << e.what() << std::endl;
                }
            }
            if (action == ErrorCallbackResult::kAbort) {
                break;
            }
            if (action == ErrorCallbackResult::kRetryImmediately) {
                wait_interval = std::chrono::seconds(0);
            } else if (action == ErrorCallbackResult::kRetryWithExponentialBackOff) {
                // Exponentially increase the retry interval.
                wait_interval = retry_interval;
                retry_interval = std::min(retry_interval * 2, m_registration_interval);
            } else {
                // Default action is to continue with the next iteration.
                wait_interval = m_registration_interval;
            }
            continue;
        }

        // If successful update, clear the registration fault regardless of current fault state
        if (m_fault_client) {
            if (m_fault_client->ClearServiceFault(m_service_fault.fault_id())) {
                m_registration_fault_active = false;
            }
        }
        wait_interval = m_registration_interval;
        retry_interval = m_registration_initial_retry_interval;
    } while (m_periodic_thread_helper->WaitForInterval(wait_interval));

    m_thread_stopped = true;
    m_periodic_thread_helper->Stop();
}

}  // namespace client

}  // namespace bosdyn
