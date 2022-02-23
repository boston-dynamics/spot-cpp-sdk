/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/estop.pb.h>

#include <future>

#include "bosdyn/common/status.h"
#include "bosdyn/client/estop/estop_client.h"

namespace bosdyn {

namespace client {

namespace {

const std::string kRequiredRole = "PDB_rooted";

}  // namespace

class EstopEndpoint {
 public:
    EstopEndpoint(EstopClient* estop_client, const std::string& name,
                  ::bosdyn::common::Duration estop_timeout, const std::string& role = kRequiredRole,
                  bool first_checkin = true,
                  ::bosdyn::common::Duration estop_cut_power_timeout = std::chrono::seconds(0));

    // Returns whether or not the first E-Stop checkin has been performed. By default, the E-Stop
    // endpoint is initialized with first checkin being true (indicating whenever a CheckInAtLevel
    // is called, it will be considered the first one). Note, this method is thread-safe.
    bool GetFirstCheckIn();

    // Read the current challenge number. Note, this method is thread-safe.
    int GetChallenge();

    // Write a new challenge number for the endpoint. Note, this method is thread safe.
    void SetChallenge(int challenge);

    // Generate a response number for the current endpoint's challenge number.
    int GetResponse();

    // Get the endpoint's unique id.
    std::string GetUniqueId() { return m_unique_id; }

    // Get the configuration id.
    std::string GetConfigId() { return m_config_id; }

    // Helper method to convert the EstopEndpoint class into a protobuf EstopEndpoint message.
    ::bosdyn::api::EstopEndpoint ToProto();

    // Helper method to apply the fields of a protobuf EstopEndpoint message onto the existing,
    // mutable EstopEndpoint class.
    void FromProto(const ::bosdyn::api::EstopEndpoint& proto);

    // Replaces the existing E-Stop configuration on robot with a single-endpoint configuration
    // based off of this EstopEndpoint class and values.
    ::bosdyn::common::Status ForceSimpleSetup();

    // Takes over as the endpoint of this existing simple estop configuration.
    ::bosdyn::common::Status TakeOverSimpleSetup(const std::string& name);

    // Register this endpoint to the given configuration.
    ::bosdyn::common::Status Register(const std::string& target_config_id);

    // Deregister this endpoint from the configuration.
    ::bosdyn::common::Status Deregister();

    // Async version for the Deregister function.
    std::shared_future<DeregisterEstopEndpointResultType> DeregisterAsync();

    // Check in at a specified level. Note, this function is meant for internal use by the different
    // level-based E-Stop calls, but can be useful for higher-level wrappers.
    ::bosdyn::common::Status CheckInAtLevel(const ::bosdyn::api::EstopStopLevel& stop_level,
                                  const RPCParameters& parameters = RPCParameters());

    // Issue a CUT stop level command to the robot, cutting motor power immediately.
    ::bosdyn::common::Status Stop();

    // Issue a SETTLE_THEN_CUT stop level. The robot will attempt to sit before cutting motor power.
    ::bosdyn::common::Status SettleThenCut();

    // Issue a NONE stop level command to the robot, allowing motor power.
    ::bosdyn::common::Status Allow();

    // Get the E-Stop timeout.
    ::bosdyn::common::Duration GetEstopTimeout() const { return m_estop_timeout; }

    // Get the E-Stop client associated with this endpoint.
    EstopClient* GetEstopClient() { return m_estop_client; }


    // A component of the endpoint class is std::mutex, which is not copyable.
    // Therefore, the endpoint class is also not movable or copyable.
    EstopEndpoint(const EstopEndpoint&) = delete;
    EstopEndpoint operator=(const EstopEndpoint&) = delete;

 private:
    // Lock for various state variables for the E-Stop endpoint.
    std::mutex m_estop_endpoint_lock;

    // The name for the endpoint. This is set either by the constructor or by a protobuf message
    // that is applied to the class using the FromProto() method.
    std::string m_endpoint_name;

    // A shared pointer to the E-Stop client, so that different endpoint changes can be applied to
    // the E-Stop service on robot.
    EstopClient* m_estop_client = nullptr;

    // Maximum delay between challenge and response for this endpoint prior to soft power off
    // handling (similar to settle then cut).
    ::bosdyn::common::Duration m_estop_timeout;

    // Optional maximum delay between challenge and response for this endpoint prior to disabling
    // motor power. After cut_power_timeout has passed, motor power will be disconnected
    // immediately regardless of current robot state.
    ::bosdyn::common::Duration m_estop_cut_power_timeout;

    // Tracks whether or not the first checkin has been completed yet or not.
    bool m_locked_first_checkin;

    // The role of the endpoint; should be a human-readable string.
    std::string m_endpoint_role;

    // Unique id for the endpoint; will get assigned by the server.
    std::string m_unique_id;

    // Tracks the target config id for the current configuration associated with this endpoint.
    std::string m_config_id;

    // The current challenge number for the endpoint.
    int m_challenge = 0;

    // Create a response number from a non-zero challenge number by taking the complement/inverse of
    // the number.
    int GenerateChallengeResponse(int challenge);

    // Update the current state of whether or not an E-Stop checkin has been performed.
    void SetFirstCheckIn(bool checkin_val);
};

}  // namespace client

}  // namespace bosdyn
