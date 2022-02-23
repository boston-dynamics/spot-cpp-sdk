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

#include <bosdyn/api/payload.pb.h>

#include "payload_registration_client.h"
#include "bosdyn/client/service_client/common_result_types.h"
#include "bosdyn/client/robot/robot.h"

namespace bosdyn {

namespace client {

/*
The PayloadRegistrationKeepAlive is a helper class used to keep a payload entry updated and,
optionally, maintain liveness with the robot. It will continually attempt to register the payload.
Each registration attempt acts as a heartbeat that will satisfy payload liveness checks, if the
payload was registered with a timeout.
*/
class PayloadRegistrationKeepAlive {
 public:
    PayloadRegistrationKeepAlive(PayloadRegistrationClient* payload_registration_client,
                                 const ::bosdyn::api::Payload& payload, const std::string& secret,
                                 ::bosdyn::common::Duration rpc_interval = std::chrono::seconds(30));

    ~PayloadRegistrationKeepAlive();

    // Start the thread if it is not yet running.
    void Start();

    // Return true if the thread is running.
    bool IsAlive() const;

    // Shut down the thread if it is running.
    void Shutdown();

    // The result from the most recent RegisterPayloadRequest.
    RegisterPayloadResultType GetLastResult() { return m_last_result; }

 private:
    // The result from the most recent RegisterPayloadRequest.
    RegisterPayloadResultType m_last_result;

    // Attempt to register the payload and return a bool indicating if it failed in a bad way.
    bool RegisterPayload();

    // Background thread that continually registers/updates the service.
    void PeriodicReregisterThreadMethod();

    PayloadRegistrationClient* m_payload_registration_client = nullptr;

    ::bosdyn::api::Payload m_payload;

    // Repeated registration request.
    ::bosdyn::api::RegisterPayloadRequest m_register_request;

    // Time between registration/update calls. Should be shorter than the liveness timeout.
    ::bosdyn::common::Duration m_rpc_interval;

    std::thread m_thread;

    // Time to wait between checking if exit has been requested.
    ::bosdyn::common::Duration m_wait_time = std::chrono::seconds(1);

    // Limit on how frequently an error should be logged.
    ::bosdyn::common::Duration m_log_limit = std::chrono::seconds(3);

    // Thread should exit.
    bool m_should_exit;

    // Is thread alive.
    bool m_is_alive;
};

}  // namespace client

}  // namespace bosdyn
