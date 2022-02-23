/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <mutex>
#include <thread>

#include "directory_registration_client.h"
#include "bosdyn/client/service_client/common_result_types.h"
#include "bosdyn/client/fault/fault_client.h"

#include <bosdyn/api/directory.pb.h>

namespace bosdyn {

namespace client {

// The DirectoryRegistrationKeepAlive is a helper class used to keep a directory entry live
// and updated. It will continually attempt to register/update the service. Each update
// attempt acts as a heartbeat that will satisfy service liveness checks, if the service
// was registered with a timeout.
class DirectoryRegistrationKeepAlive {
 public:
    DirectoryRegistrationKeepAlive(
        DirectoryRegistrationClient* directory_registration_client,
        const ::bosdyn::api::ServiceEntry& service_entry, const ::bosdyn::api::Endpoint& endpoint,
        ::bosdyn::common::Duration rpc_interval = std::chrono::seconds(30),
        FaultClient* fault_client = nullptr);

    ~DirectoryRegistrationKeepAlive();

    // Start the thread if it is not yet running.
    void Start(const std::vector<std::string>& fault_attributes = {});

    // Return true if the thread is running.
    bool IsAlive() const;

    // Shut down the thread if it is running.
    void Shutdown();

    // Unregister the service.
    UnregisterServiceResultType Unregister();

 private:
    // Background thread that continually registers/updates the service.
    void PeriodicReregisterThreadMethod();

    DirectoryRegistrationClient* m_directory_registration_client = nullptr;

    ::bosdyn::api::ServiceEntry m_service_entry;
    ::bosdyn::api::Endpoint m_endpoint;

    // Time between registration/update calls. Should be shorter than the liveness timeout.
    ::bosdyn::common::Duration m_registration_interval;

    std::thread m_thread;

    // Thread should exit.
    bool m_should_exit;

    // Is thread stopped.
    bool m_thread_stopped;

    // Fault client to trigger/clear faults on failure or success.
    FaultClient* m_fault_client = nullptr;

    // Registration fault to be triggered/cleared.
    ::bosdyn::api::ServiceFault m_service_fault;

    // Bool to track if there is a failure.
    bool m_registration_fault_active;
};

}  // namespace client

}  // namespace bosdyn
