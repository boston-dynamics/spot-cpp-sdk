/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/audio/audio_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* AudioClient::s_default_service_name = "spot-cam-audio";

const char* AudioClient::s_service_type = "bosdyn.api.spot_cam.AudioService";

std::shared_future<PlaySoundResultType> AudioClient::PlaySoundAsync(
    const std::string& sound_name, float gain,
    const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::PlaySoundRequest request;

    request.mutable_sound()->set_name(sound_name);
    request.mutable_gain()->set_value(gain);

    return PlaySoundAsync(request, parameters);
}

std::shared_future<PlaySoundResultType> AudioClient::PlaySoundAsync(
    const std::string& sound_name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::PlaySoundRequest request;

    request.mutable_sound()->set_name(sound_name);

    return PlaySoundAsync(request, parameters);
}

PlaySoundResultType AudioClient::PlaySound(
    const std::string& sound_name, float gain,
    const RPCParameters& parameters) {
    return PlaySoundAsync(sound_name, gain, parameters).get();
}

PlaySoundResultType AudioClient::PlaySound(
    const std::string& sound_name, const RPCParameters& parameters) {
    return PlaySoundAsync(sound_name, parameters).get();
}

std::shared_future<PlaySoundResultType> AudioClient::PlaySoundAsync(
    ::bosdyn::api::spot_cam::PlaySoundRequest& request,
    const RPCParameters& parameters) {
    std::promise<PlaySoundResultType> response;
    std::shared_future<PlaySoundResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::PlaySoundRequest, ::bosdyn::api::spot_cam::PlaySoundResponse,
                          ::bosdyn::api::spot_cam::PlaySoundResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncPlaySound, m_stub.get(), _1, _2, _3),
            std::bind(&AudioClient::OnPlaySoundComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

PlaySoundResultType AudioClient::PlaySound(
    ::bosdyn::api::spot_cam::PlaySoundRequest& request,
    const RPCParameters& parameters) {
    return PlaySoundAsync(request, parameters).get();
}

void AudioClient::OnPlaySoundComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::PlaySoundRequest& request,
    ::bosdyn::api::spot_cam::PlaySoundResponse&& response, const grpc::Status& status,
    std::promise<PlaySoundResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::PlaySoundResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<DeleteSoundResultType> AudioClient::DeleteSoundAsync(
    const std::string& sound_name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::DeleteSoundRequest request;

    request.mutable_sound()->set_name(sound_name);

    return DeleteSoundAsync(request, parameters);
}

DeleteSoundResultType AudioClient::DeleteSound(
    const std::string& sound_name, const RPCParameters& parameters) {
    return DeleteSoundAsync(sound_name, parameters).get();
}

std::shared_future<DeleteSoundResultType> AudioClient::DeleteSoundAsync(
    ::bosdyn::api::spot_cam::DeleteSoundRequest& request,
    const RPCParameters& parameters) {
    std::promise<DeleteSoundResultType> response;
    std::shared_future<DeleteSoundResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::DeleteSoundRequest, ::bosdyn::api::spot_cam::DeleteSoundResponse,
                          ::bosdyn::api::spot_cam::DeleteSoundResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncDeleteSound, m_stub.get(), _1, _2,
                      _3),
            std::bind(&AudioClient::OnDeleteSoundComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DeleteSoundResultType AudioClient::DeleteSound(
    ::bosdyn::api::spot_cam::DeleteSoundRequest& request,
    const RPCParameters& parameters) {
    return DeleteSoundAsync(request, parameters).get();
}

void AudioClient::OnDeleteSoundComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::DeleteSoundRequest& request,
    ::bosdyn::api::spot_cam::DeleteSoundResponse&& response, const grpc::Status& status,
    std::promise<DeleteSoundResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::DeleteSoundResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<ListSoundsResultType> AudioClient::ListSoundsAsync(
    const RPCParameters& parameters) {
    std::promise<ListSoundsResultType> response;
    std::shared_future<ListSoundsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::ListSoundsRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::ListSoundsRequest, ::bosdyn::api::spot_cam::ListSoundsResponse,
                          ::bosdyn::api::spot_cam::ListSoundsResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncListSounds, m_stub.get(), _1, _2,
                      _3),
            std::bind(&AudioClient::OnListSoundsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ListSoundsResultType AudioClient::ListSounds(
    const RPCParameters& parameters) {
    return ListSoundsAsync(parameters).get();
}

void AudioClient::OnListSoundsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::ListSoundsRequest& request,
    ::bosdyn::api::spot_cam::ListSoundsResponse&& response, const grpc::Status& status,
    std::promise<ListSoundsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::ListSoundsResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<SetVolumeResultType> AudioClient::SetVolumeAsync(
    float percentage, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetVolumeRequest request;

    request.set_volume(percentage);
    return SetVolumeAsync(request, parameters);
}

SetVolumeResultType AudioClient::SetVolume(
    float percentage, const RPCParameters& parameters) {
    return SetVolumeAsync(percentage, parameters).get();
}

std::shared_future<SetVolumeResultType> AudioClient::SetVolumeAsync(
    ::bosdyn::api::spot_cam::SetVolumeRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetVolumeResultType> response;
    std::shared_future<SetVolumeResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetVolumeRequest, ::bosdyn::api::spot_cam::SetVolumeResponse,
                          ::bosdyn::api::spot_cam::SetVolumeResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncSetVolume, m_stub.get(), _1, _2, _3),
            std::bind(&AudioClient::OnSetVolumeComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetVolumeResultType AudioClient::SetVolume(
    ::bosdyn::api::spot_cam::SetVolumeRequest& request,
    const RPCParameters& parameters) {
    return SetVolumeAsync(request, parameters).get();
}

void AudioClient::OnSetVolumeComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetVolumeRequest& request,
    ::bosdyn::api::spot_cam::SetVolumeResponse&& response, const grpc::Status& status,
    std::promise<SetVolumeResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetVolumeResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetVolumeResultType> AudioClient::GetVolumeAsync(
    const RPCParameters& parameters) {
    std::promise<GetVolumeResultType> response;
    std::shared_future<GetVolumeResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetVolumeRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetVolumeRequest, ::bosdyn::api::spot_cam::GetVolumeResponse,
                          ::bosdyn::api::spot_cam::GetVolumeResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncGetVolume, m_stub.get(), _1, _2, _3),
            std::bind(&AudioClient::OnGetVolumeComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetVolumeResultType AudioClient::GetVolume(
    const RPCParameters& parameters) {
    return GetVolumeAsync(parameters).get();
}

void AudioClient::OnGetVolumeComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetVolumeRequest& request,
    ::bosdyn::api::spot_cam::GetVolumeResponse&& response, const grpc::Status& status,
    std::promise<GetVolumeResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetVolumeResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

/*
 *
 * RPCs only for Spot CAM+IR
 *  
 */

// SetAudioCaptureChannel

std::shared_future<SetAudioCaptureChannelResultType> AudioClient::SetAudioCaptureChannelAsync(
    ::bosdyn::api::spot_cam::AudioCaptureChannel channel, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest request;

    request.set_channel(channel);
    return SetAudioCaptureChannelAsync(request, parameters);
}

SetAudioCaptureChannelResultType AudioClient::SetAudioCaptureChannel(
    ::bosdyn::api::spot_cam::AudioCaptureChannel channel, const RPCParameters& parameters) {
    return SetAudioCaptureChannelAsync(channel, parameters).get();
}

std::shared_future<SetAudioCaptureChannelResultType> AudioClient::SetAudioCaptureChannelAsync(
    ::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetAudioCaptureChannelResultType> response;
    std::shared_future<SetAudioCaptureChannelResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest, ::bosdyn::api::spot_cam::SetAudioCaptureChannelResponse,
                          ::bosdyn::api::spot_cam::SetAudioCaptureChannelResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncSetAudioCaptureChannel, m_stub.get(), _1, _2, _3),
            std::bind(&AudioClient::OnSetAudioCaptureChannelComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetAudioCaptureChannelResultType AudioClient::SetAudioCaptureChannel(
    ::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest& request,
    const RPCParameters& parameters) {
    return SetAudioCaptureChannelAsync(request, parameters).get();
}

void AudioClient::OnSetAudioCaptureChannelComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest& request,
    ::bosdyn::api::spot_cam::SetAudioCaptureChannelResponse&& response, const grpc::Status& status,
    std::promise<SetAudioCaptureChannelResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetAudioCaptureChannelResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// GetAudioCaptureChannel

std::shared_future<GetAudioCaptureChannelResultType> AudioClient::GetAudioCaptureChannelAsync(
    const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::GetAudioCaptureChannelRequest request;
    std::promise<GetAudioCaptureChannelResultType> response;
    std::shared_future<GetAudioCaptureChannelResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetAudioCaptureChannelRequest, ::bosdyn::api::spot_cam::GetAudioCaptureChannelResponse,
                          ::bosdyn::api::spot_cam::GetAudioCaptureChannelResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncGetAudioCaptureChannel, m_stub.get(), _1, _2, _3),
            std::bind(&AudioClient::OnGetAudioCaptureChannelComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetAudioCaptureChannelResultType AudioClient::GetAudioCaptureChannel(
    const RPCParameters& parameters) {
    return GetAudioCaptureChannelAsync(parameters).get();
}

void AudioClient::OnGetAudioCaptureChannelComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetAudioCaptureChannelRequest& request,
    ::bosdyn::api::spot_cam::GetAudioCaptureChannelResponse&& response, const grpc::Status& status,
    std::promise<GetAudioCaptureChannelResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetAudioCaptureChannelResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// SetAudioCaptureGain

std::shared_future<SetAudioCaptureGainResultType> AudioClient::SetAudioCaptureGainAsync(
    ::bosdyn::api::spot_cam::AudioCaptureChannel channel, double gain, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetAudioCaptureGainRequest request;

    request.set_channel(channel);
    request.set_gain(gain);
    return SetAudioCaptureGainAsync(request, parameters);
}

SetAudioCaptureGainResultType AudioClient::SetAudioCaptureGain(
    ::bosdyn::api::spot_cam::AudioCaptureChannel channel, double gain, const RPCParameters& parameters) {
    return SetAudioCaptureGainAsync(channel, gain, parameters).get();
}

std::shared_future<SetAudioCaptureGainResultType> AudioClient::SetAudioCaptureGainAsync(
    ::bosdyn::api::spot_cam::SetAudioCaptureGainRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetAudioCaptureGainResultType> response;
    std::shared_future<SetAudioCaptureGainResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetAudioCaptureGainRequest, ::bosdyn::api::spot_cam::SetAudioCaptureGainResponse,
                          ::bosdyn::api::spot_cam::SetAudioCaptureGainResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncSetAudioCaptureGain, m_stub.get(), _1, _2, _3),
            std::bind(&AudioClient::OnSetAudioCaptureGainComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetAudioCaptureGainResultType AudioClient::SetAudioCaptureGain(
    ::bosdyn::api::spot_cam::SetAudioCaptureGainRequest& request,
    const RPCParameters& parameters) {
    return SetAudioCaptureGainAsync(request, parameters).get();
}

void AudioClient::OnSetAudioCaptureGainComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetAudioCaptureGainRequest& request,
    ::bosdyn::api::spot_cam::SetAudioCaptureGainResponse&& response, const grpc::Status& status,
    std::promise<SetAudioCaptureGainResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetAudioCaptureGainResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// GetAudioCaptureGain

std::shared_future<GetAudioCaptureGainResultType> AudioClient::GetAudioCaptureGainAsync(
    ::bosdyn::api::spot_cam::AudioCaptureChannel channel, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::GetAudioCaptureGainRequest request;

    request.set_channel(channel);
    return GetAudioCaptureGainAsync(request, parameters);
}

GetAudioCaptureGainResultType AudioClient::GetAudioCaptureGain(
    ::bosdyn::api::spot_cam::AudioCaptureChannel channel, const RPCParameters& parameters) {
    return GetAudioCaptureGainAsync(channel, parameters).get();
}

std::shared_future<GetAudioCaptureGainResultType> AudioClient::GetAudioCaptureGainAsync(
    ::bosdyn::api::spot_cam::GetAudioCaptureGainRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetAudioCaptureGainResultType> response;
    std::shared_future<GetAudioCaptureGainResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetAudioCaptureGainRequest, ::bosdyn::api::spot_cam::GetAudioCaptureGainResponse,
                          ::bosdyn::api::spot_cam::GetAudioCaptureGainResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::AudioService::Stub::AsyncGetAudioCaptureGain, m_stub.get(), _1, _2, _3),
            std::bind(&AudioClient::OnGetAudioCaptureGainComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetAudioCaptureGainResultType AudioClient::GetAudioCaptureGain(
    ::bosdyn::api::spot_cam::GetAudioCaptureGainRequest& request,
    const RPCParameters& parameters) {
    return GetAudioCaptureGainAsync(request, parameters).get();
}

void AudioClient::OnGetAudioCaptureGainComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetAudioCaptureGainRequest& request,
    ::bosdyn::api::spot_cam::GetAudioCaptureGainResponse&& response, const grpc::Status& status,
    std::promise<GetAudioCaptureGainResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetAudioCaptureGainResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}


ServiceClient::QualityOfService AudioClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void AudioClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::AudioService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
