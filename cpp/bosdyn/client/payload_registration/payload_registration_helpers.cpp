/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
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
    const std::string& secret, ::bosdyn::common::Duration rpc_interval)
    : m_payload_registration_client(payload_registration_client),
      m_payload(payload),
      m_rpc_interval(rpc_interval),
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
    m_should_exit = true;
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void PayloadRegistrationKeepAlive::PeriodicReregisterThreadMethod() {
    // Do not maintain the thread if the first registration fails in a bad way.
    m_is_alive = true;

    // Continually attempt to register and update the service.
    std::chrono::seconds last_request_success_time = std::chrono::seconds(0);
    std::chrono::seconds last_log_time = std::chrono::seconds(0);
    while (!m_should_exit) {
        if (::bosdyn::common::SecSinceEpoch() > last_request_success_time + m_rpc_interval) {
            m_last_result = m_payload_registration_client->RegisterPayload(m_register_request);

            // If payload registration failed in an unexpected way, log the error.
            if (!m_last_result && m_last_result.status.code() !=
                                      ::bosdyn::api::RegisterPayloadResponse::STATUS_ALREADY_EXISTS) {
                // Do not spam logs.
                if (::bosdyn::common::SecSinceEpoch() > last_log_time + m_log_limit) {
                    last_log_time = ::bosdyn::common::SecSinceEpoch();
                    std::cerr << m_payload.guid()
                              << " PayloadRegistrationKeepAlive failed registration: "
                              << m_last_result.status.DebugString();
                }
            } else {
                last_request_success_time = ::bosdyn::common::SecSinceEpoch();
            }
        }
        std::this_thread::sleep_for(m_wait_time);
    }

    m_is_alive = false;
}

}  // namespace client

}  // namespace bosdyn
