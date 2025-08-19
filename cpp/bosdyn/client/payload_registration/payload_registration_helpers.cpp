/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "payload_registration_helpers.h"

#include "bosdyn/common/time.h"

#include <bosdyn/api/payload_registration.pb.h>

namespace bosdyn {

namespace client {

PayloadRegistrationKeepAlive::PayloadRegistrationKeepAlive(
    PayloadRegistrationClient* payload_registration_client, const ::bosdyn::api::Payload& payload,
    const std::string& secret, ::bosdyn::common::Duration rpc_interval,
    std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)> error_callback,
    ::bosdyn::common::Duration registration_initial_retry_interval)
    : m_payload_registration_client(payload_registration_client),
      m_payload(payload),
      m_rpc_interval(rpc_interval),
      m_periodic_thread_helper(std::make_unique<PeriodicThreadHelper>()),
      m_registration_initial_retry_interval(registration_initial_retry_interval),
      m_error_callback(error_callback),
      m_should_exit(false),
      m_is_alive(false) {
    m_register_request.mutable_payload()->CopyFrom(payload);
    m_register_request.set_payload_secret(secret);

    // Start the thread.
    m_thread = std::thread(&PayloadRegistrationKeepAlive::PeriodicReregisterThreadMethod, this);
}


PayloadRegistrationKeepAlive::~PayloadRegistrationKeepAlive() { Shutdown(); }

bool PayloadRegistrationKeepAlive::IsAlive() const { return m_is_alive; }

void PayloadRegistrationKeepAlive::Shutdown() {
    m_periodic_thread_helper->Stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void PayloadRegistrationKeepAlive::PeriodicReregisterThreadMethod() {
    // Do not maintain the thread if the first registration fails in a bad way.
    m_is_alive = true;

    // Continually attempt to register and update the service.
    std::chrono::seconds last_log_time = std::chrono::seconds(0);
    ::bosdyn::common::Duration wait_time = m_wait_time;
    ::bosdyn::common::Duration retry_interval = m_registration_initial_retry_interval;

    do {
        m_last_result = m_payload_registration_client->RegisterPayload(m_register_request);

        // If payload registration failed in an unexpected way, log the error.
        if (!m_last_result && m_last_result.status.code() !=
                                  ::bosdyn::api::RegisterPayloadResponse::STATUS_ALREADY_EXISTS) {
            ErrorCallbackResult action = ErrorCallbackResult::kResumeNormalOperation;
            if (m_error_callback) {
                try {
                    action = m_error_callback(m_last_result.status);
                } catch (const std::exception& e) {
                    std::cerr << "Exception thrown in error callback: " << e.what() << std::endl;
                }
            }
            if (action == ErrorCallbackResult::kAbort) {
                break;
            } else if (action == ErrorCallbackResult::kRetryImmediately) {
                wait_time = std::chrono::seconds(0);
            } else if (action == ErrorCallbackResult::kRetryWithExponentialBackOff) {
                wait_time = retry_interval;
                retry_interval = std::min(retry_interval * 2, m_rpc_interval);
            } else {
                wait_time = m_rpc_interval;
                retry_interval = m_registration_initial_retry_interval;
            }

            // Do not spam logs.
            if (::bosdyn::common::SecSinceEpoch() > last_log_time + m_log_limit) {
                last_log_time = ::bosdyn::common::SecSinceEpoch();
                std::cerr << m_payload.guid()
                          << " PayloadRegistrationKeepAlive failed registration: "
                          << m_last_result.status.DebugString();
            }
        } else {
            wait_time = m_rpc_interval;
            retry_interval = m_registration_initial_retry_interval;
        }
    } while (m_periodic_thread_helper->WaitForInterval(wait_time));

    m_is_alive = false;
    m_periodic_thread_helper->Stop();
}

}  // namespace client

}  // namespace bosdyn
