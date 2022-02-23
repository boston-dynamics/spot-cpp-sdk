/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/estop/estop_endpoint.h"
#include <google/protobuf/duration.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/time_util.h>
#include "bosdyn/common/time.h"
#include "bosdyn/client/estop/estop_helpers_inl.h"

namespace bosdyn {

namespace client {

EstopEndpoint::EstopEndpoint(EstopClient* estop_client, const std::string& name,
                             ::bosdyn::common::Duration estop_timeout, const std::string& role,
                             bool first_checkin,
                             ::bosdyn::common::Duration estop_cut_power_timeout)
    : m_endpoint_name(name),
      m_estop_client(estop_client),
      m_estop_timeout(estop_timeout),
      m_estop_cut_power_timeout(estop_cut_power_timeout),
      m_locked_first_checkin(first_checkin),
      m_endpoint_role(role) {}

bool EstopEndpoint::GetFirstCheckIn() {
    std::lock_guard<std::mutex> lock(m_estop_endpoint_lock);
    return m_locked_first_checkin;
}

void EstopEndpoint::SetFirstCheckIn(bool checkin_val) {
    std::lock_guard<std::mutex> lock(m_estop_endpoint_lock);
    m_locked_first_checkin = checkin_val;
}

int EstopEndpoint::GetChallenge() {
    std::lock_guard<std::mutex> lock(m_estop_endpoint_lock);
    return m_challenge;
}

void EstopEndpoint::SetChallenge(int challenge) {
    std::lock_guard<std::mutex> lock(m_estop_endpoint_lock);
    m_challenge = challenge;
}

::bosdyn::api::EstopEndpoint EstopEndpoint::ToProto() {
    ::bosdyn::api::EstopEndpoint new_endpoint;
    new_endpoint.set_role(m_endpoint_role);
    new_endpoint.set_name(m_endpoint_name);
    new_endpoint.set_unique_id(m_unique_id);
    new_endpoint.mutable_timeout()->CopyFrom(
        google::protobuf::util::TimeUtil::NanosecondsToDuration(
            m_estop_timeout.count()));
    if (m_estop_cut_power_timeout > std::chrono::seconds(0)) {
        new_endpoint.mutable_cut_power_timeout()->CopyFrom(
            google::protobuf::util::TimeUtil::NanosecondsToDuration(
                m_estop_cut_power_timeout.count()));
    }
    return new_endpoint;
}

void EstopEndpoint::FromProto(const ::bosdyn::api::EstopEndpoint& proto) {
    if (!(m_endpoint_name == proto.name())) {
        m_endpoint_name = proto.name();
    }
    m_endpoint_role = proto.role();
    m_unique_id = proto.unique_id();
    m_estop_timeout = ::bosdyn::common::Duration(
        google::protobuf::util::TimeUtil::DurationToNanoseconds(proto.timeout()));
    if (proto.has_cut_power_timeout()) {
        m_estop_cut_power_timeout = ::bosdyn::common::Duration(
            google::protobuf::util::TimeUtil::DurationToNanoseconds(proto.cut_power_timeout()));
    }
}

::bosdyn::common::Status EstopEndpoint::ForceSimpleSetup() {
    ::bosdyn::api::EstopConfig estop_config;
    auto new_config_endpoint = estop_config.add_endpoints();
    new_config_endpoint->CopyFrom(this->ToProto());

    auto active_config_result = m_estop_client->GetEstopConfig();
    if (!active_config_result) {
        // Failed to get the current configuration on robot.
        return active_config_result.status;
    }

    auto unique_id = active_config_result.response.active_config().unique_id();
    auto set_config_result = m_estop_client->SetEstopConfig(estop_config, unique_id);
    if (!set_config_result.status) {
        // Failed to set the new configuration for the estop endpoint on robot.
        return set_config_result.status;
    }
    if (set_config_result.response.active_config().endpoints_size() == 0) {
        // No endpoints.
        return ::bosdyn::common::Status(
            SDKErrorCode::GenericSDKError,
            "Force Simple Setup failed to find any existing endpoint configs on the robot.");
    }

    m_unique_id = set_config_result.response.active_config().endpoints(0).unique_id();
    return this->Register(set_config_result.response.active_config().unique_id());
}

::bosdyn::common::Status EstopEndpoint::TakeOverSimpleSetup(const std::string& name) {
    auto estop_config_result = m_estop_client->GetEstopConfig();
    if (!estop_config_result) {
        return estop_config_result.status;
    }
    const auto& estop_config = estop_config_result.response;

    auto estop_status_result = m_estop_client->GetEstopStatus();
    if (!estop_status_result) {
        return estop_status_result.status;
    }
    const auto& estop_status = estop_status_result.response;

    if (estop_config.active_config().unique_id().empty()) {
        return ::bosdyn::common::Status(
            SDKErrorCode::GenericSDKError,
            "TakeOverSimpleSetup failed, missing unique_id");
    }
    if (estop_status.status().endpoints_size() != 1) {
        return ::bosdyn::common::Status(
            SDKErrorCode::GenericSDKError,
            "TakeOverSimpleSetup failed, expected 1 endpoint");
    }
    if (estop_status.status().endpoints(0).endpoint().name() != name) {
        return ::bosdyn::common::Status(
            SDKErrorCode::GenericSDKError,
            "TakeOverSimpleSetup failed, wrong endpoint name");
    }

    m_config_id = estop_config.active_config().unique_id();
    this->FromProto(estop_status.status().endpoints(0).endpoint());
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

::bosdyn::common::Status EstopEndpoint::Register(const std::string& target_config_id) {
    ::bosdyn::api::RegisterEstopEndpointRequest request =
        ::bosdyn::client::MakeRegisterRequest(target_config_id, this);
    auto new_endpoint_result = m_estop_client->RegisterEstopEndpoint(request);
    if (!new_endpoint_result) {
        // Registering the new estop endpoint failed for some reason.
        return new_endpoint_result.status;
    }
    m_config_id = target_config_id;
    auto new_endpoint_proto = new_endpoint_result.response.new_endpoint();
    this->FromProto(new_endpoint_proto);

    return this->Stop();
}

::bosdyn::common::Status EstopEndpoint::Deregister() {
    ::bosdyn::api::DeregisterEstopEndpointRequest request = MakeDeregisterRequest(m_config_id, this);
    return m_estop_client->DeregisterEstopEndpoint(request).status;
}

std::shared_future<DeregisterEstopEndpointResultType> EstopEndpoint::DeregisterAsync() {
    ::bosdyn::api::DeregisterEstopEndpointRequest request = MakeDeregisterRequest(m_config_id, this);
    return m_estop_client->DeregisterEstopEndpointAsync(request);
}

::bosdyn::common::Status EstopEndpoint::CheckInAtLevel(const ::bosdyn::api::EstopStopLevel& stop_level,
                                             const RPCParameters& parameters) {
    ::bosdyn::api::EstopCheckInRequest request =
        ::bosdyn::client::MakeCheckInRequest(stop_level, this, m_challenge, this->GetResponse());
    auto checkin_results = m_estop_client->EstopCheckIn(request, parameters);
    // If first_checkin == true, then we want to suppress any estop response errors (specifically the
    // incorrect challenge response) and automatically set the challenge
    bool is_first_checkin = this->GetFirstCheckIn();
    if (is_first_checkin) {
        // Ignore any status errors and forcefully update the challenge.
        this->SetChallenge(checkin_results.response.challenge());
        this->SetFirstCheckIn(false);
    } else if (checkin_results.status) {
        // Successful checkin result status.
        this->SetChallenge(checkin_results.response.challenge());
        this->SetFirstCheckIn(false);
    } else {
        // All failed. Python raises an error in this case, instead C++ will just return the status.
        return checkin_results.status;
    }
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

int EstopEndpoint::GetResponse() {
    if (m_challenge == 0) {
        return m_challenge;
    } else {
        return GenerateChallengeResponse(m_challenge);
    }
}

int EstopEndpoint::GenerateChallengeResponse(int challenge) { return ((int)~challenge); }

::bosdyn::common::Status EstopEndpoint::Stop() {
    return this->CheckInAtLevel(::bosdyn::api::EstopStopLevel::ESTOP_LEVEL_CUT);
}

::bosdyn::common::Status EstopEndpoint::SettleThenCut() {
    return this->CheckInAtLevel(::bosdyn::api::EstopStopLevel::ESTOP_LEVEL_SETTLE_THEN_CUT);
}

::bosdyn::common::Status EstopEndpoint::Allow() {
    return this->CheckInAtLevel(::bosdyn::api::EstopStopLevel::ESTOP_LEVEL_NONE);
}


}  // namespace client

}  // namespace bosdyn
