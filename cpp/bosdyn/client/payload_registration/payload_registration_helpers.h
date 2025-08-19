/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <memory>
#include <mutex>
#include <thread>

#include <bosdyn/api/payload.pb.h>

#include "payload_registration_client.h"
#include "bosdyn/client/error_callback/error_callback_result.h"
#include "bosdyn/client/robot/robot.h"
#include "bosdyn/client/service_client/common_result_types.h"
#include "bosdyn/client/util/periodic_thread_helper.h"

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
    PayloadRegistrationKeepAlive(
        PayloadRegistrationClient* payload_registration_client,
        const ::bosdyn::api::Payload& payload, const std::string& secret,
        ::bosdyn::common::Duration rpc_interval = std::chrono::seconds(30),
        std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)> error_callback = {},
        ::bosdyn::common::Duration registration_initial_retry_interval = std::chrono::seconds(1));


    ~PayloadRegistrationKeepAlive();

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

    // Object used to coordinate the lifecycle and timing of the periodic thread.
    std::unique_ptr<PeriodicThreadHelper> m_periodic_thread_helper;

    // Time to wait between checking if exit has been requested.
    ::bosdyn::common::Duration m_wait_time = std::chrono::seconds(1);

    // Limit on how frequently an error should be logged.
    ::bosdyn::common::Duration m_log_limit = std::chrono::seconds(3);

    // Initial retry interval for registration.
    ::bosdyn::common::Duration m_registration_initial_retry_interval = std::chrono::seconds(1);

    // Callback to invoke in the event of a periodic reregistration failure.
    std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)> m_error_callback;

    // Thread should exit.
    bool m_should_exit;

    // Is thread alive.
    bool m_is_alive;
};

}  // namespace client

}  // namespace bosdyn
