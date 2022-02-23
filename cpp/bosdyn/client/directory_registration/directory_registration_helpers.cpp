/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
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
    ::bosdyn::common::Duration rpc_interval, FaultClient* fault_client)
    : m_directory_registration_client(directory_registration_client),
      m_service_entry(service_entry),
      m_endpoint(endpoint),
      m_registration_interval(rpc_interval),
      m_thread(),
      m_thread_stopped(true),
      m_fault_client(fault_client) {}

DirectoryRegistrationKeepAlive::~DirectoryRegistrationKeepAlive() {
    Shutdown();
    Unregister().IgnoreError();
}

void DirectoryRegistrationKeepAlive::Start(const std::vector<std::string>& fault_attributes) {
    m_should_exit = false;
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
        if (!clear_result && clear_result.response.status() !=
            ::bosdyn::api::ClearServiceFaultResponse::STATUS_FAULT_NOT_ACTIVE) {
            m_registration_fault_active = true;
        }
    }

    m_thread = std::thread(&DirectoryRegistrationKeepAlive::PeriodicReregisterThreadMethod, this);
}

bool DirectoryRegistrationKeepAlive::IsAlive() const { return !m_thread_stopped; }

void DirectoryRegistrationKeepAlive::Shutdown() {
    m_should_exit = true;
    m_thread.join();
}

UnregisterServiceResultType DirectoryRegistrationKeepAlive::Unregister() {
    return m_directory_registration_client->UnregisterService(m_service_entry.name());
}

void DirectoryRegistrationKeepAlive::PeriodicReregisterThreadMethod() {
    bool first_time = true;

    // Set up register and update requests
    ::bosdyn::api::RegisterServiceRequest register_service_request;
    register_service_request.mutable_endpoint()->CopyFrom(m_endpoint);
    register_service_request.mutable_service_entry()->CopyFrom(m_service_entry);
    ::bosdyn::api::UpdateServiceRequest update_service_request;
    update_service_request.mutable_endpoint()->CopyFrom(m_endpoint);
    update_service_request.mutable_service_entry()->CopyFrom(m_service_entry);

    // Continually attempt to register and update the service.
    while (!m_should_exit) {
        if (!first_time) {
            std::this_thread::sleep_for(m_registration_interval);
        }
        first_time = false;

        // Register service.
        auto res_register =
            m_directory_registration_client->RegisterService(register_service_request);
        // Successful registration. Service faults are automatically cleared for the new service
        // by the directory registration service.
        if (res_register) {
            m_registration_fault_active = false;
            continue;
        }
        // If registration failed in a bad way.
        if (res_register.status.code() !=
            ::bosdyn::api::RegisterServiceResponse::STATUS_ALREADY_EXISTS) {
            if (m_fault_client && m_fault_client->TriggerServiceFault(m_service_fault)) {
                m_registration_fault_active = true;
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
            continue;
        }

        // If successful update, clear the registration fault regardless of current fault state
        if (m_fault_client) {
            if (m_fault_client->ClearServiceFault(m_service_fault.fault_id())) {
                m_registration_fault_active = false;
            }
        }
    }

    m_thread_stopped = true;
}

}  // namespace client

}  // namespace bosdyn
