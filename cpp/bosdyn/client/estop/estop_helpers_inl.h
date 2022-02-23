/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <bosdyn/api/estop.pb.h>

#include "bosdyn/client/estop/estop_endpoint.h"

namespace bosdyn {

namespace client {

::bosdyn::api::RegisterEstopEndpointRequest MakeRegisterRequest(
    const std::string& target_config_id, ::bosdyn::client::EstopEndpoint* estop_endpoint) {
    ::bosdyn::api::RegisterEstopEndpointRequest request;
    request.set_target_config_id(target_config_id);
    request.mutable_new_endpoint()->CopyFrom(estop_endpoint->ToProto());
    // Also set target_endpoint, otherwise you get a STATUS_ENDPOINT_MISMATCH.
    request.mutable_target_endpoint()->CopyFrom(request.new_endpoint());
    return request;
}

::bosdyn::api::DeregisterEstopEndpointRequest MakeDeregisterRequest(
    const std::string& target_config_id, ::bosdyn::client::EstopEndpoint* estop_endpoint) {
    ::bosdyn::api::DeregisterEstopEndpointRequest request;
    request.set_target_config_id(target_config_id);
    request.mutable_target_endpoint()->CopyFrom(estop_endpoint->ToProto());
    return request;
}

::bosdyn::api::EstopCheckInRequest MakeCheckInRequest(const ::bosdyn::api::EstopStopLevel& stop_level,
                                                      ::bosdyn::client::EstopEndpoint* estop_endpoint,
                                                    int challenge, int response) {
    ::bosdyn::api::EstopCheckInRequest request;
    request.set_stop_level(stop_level);
    request.mutable_endpoint()->CopyFrom(estop_endpoint->ToProto());
    request.set_challenge(challenge);
    request.set_response(response);
    return request;
}

}  // namespace client

}  // namespace bosdyn
