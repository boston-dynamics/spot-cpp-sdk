/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/network/network_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* NetworkClient::s_default_service_name = "spot-cam-network";
const char* NetworkClient::s_service_type = "bosdyn.api.spot_cam.NetworkService";

std::shared_future<SetICEConfigurationResultType> NetworkClient::SetICEConfigurationAsync(
    ::bosdyn::api::spot_cam::SetICEConfigurationRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetICEConfigurationResultType> response;
    std::shared_future<SetICEConfigurationResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetICEConfigurationRequest,
                          ::bosdyn::api::spot_cam::SetICEConfigurationResponse,
                          ::bosdyn::api::spot_cam::SetICEConfigurationResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::NetworkService::Stub::AsyncSetICEConfiguration, m_stub.get(),
                      _1, _2, _3),
            std::bind(&NetworkClient::OnSetICEConfigurationComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetICEConfigurationResultType NetworkClient::SetICEConfiguration(
    ::bosdyn::api::spot_cam::SetICEConfigurationRequest& request,
    const RPCParameters& parameters) {
    return SetICEConfigurationAsync(request, parameters).get();
}

void NetworkClient::OnSetICEConfigurationComplete(
    MessagePumpCallBase* call,const ::bosdyn::api::spot_cam::SetICEConfigurationRequest& request,
    ::bosdyn::api::spot_cam::SetICEConfigurationResponse&& response, const grpc::Status& status,
    std::promise<SetICEConfigurationResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetICEConfigurationResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetICEConfigurationResultType> NetworkClient::GetICEConfigurationAsync(
    const RPCParameters& parameters) {
    std::promise<GetICEConfigurationResultType> response;
    std::shared_future<GetICEConfigurationResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetICEConfigurationRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetICEConfigurationRequest,
                          ::bosdyn::api::spot_cam::GetICEConfigurationResponse,
                          ::bosdyn::api::spot_cam::GetICEConfigurationResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::NetworkService::Stub::AsyncGetICEConfiguration, m_stub.get(),
                      _1, _2, _3),
            std::bind(&NetworkClient::OnGetICEConfigurationComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetICEConfigurationResultType NetworkClient::GetICEConfiguration(
    const RPCParameters& parameters) {
    return GetICEConfigurationAsync(parameters).get();
}

void NetworkClient::OnGetICEConfigurationComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetICEConfigurationRequest& request,
    ::bosdyn::api::spot_cam::GetICEConfigurationResponse&& response, const grpc::Status& status,
    std::promise<GetICEConfigurationResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetICEConfigurationResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService NetworkClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void NetworkClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::NetworkService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
