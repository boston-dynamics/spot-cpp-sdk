/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <map>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/spot_cam/audio/audio_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::PlaySoundResponse> PlaySoundResultType;
typedef Result<::bosdyn::api::spot_cam::DeleteSoundResponse> DeleteSoundResultType;
typedef Result<::bosdyn::api::spot_cam::ListSoundsResponse> ListSoundsResultType;
typedef Result<::bosdyn::api::spot_cam::SetVolumeResponse> SetVolumeResultType;
typedef Result<::bosdyn::api::spot_cam::GetVolumeResponse> GetVolumeResultType;
typedef Result<::bosdyn::api::spot_cam::LoadSoundResponse> LoadSoundResultType;
typedef Result<::bosdyn::api::spot_cam::SetAudioCaptureChannelResponse> SetAudioCaptureChannelResultType;
typedef Result<::bosdyn::api::spot_cam::GetAudioCaptureChannelResponse> GetAudioCaptureChannelResultType;
typedef Result<::bosdyn::api::spot_cam::SetAudioCaptureGainResponse> SetAudioCaptureGainResultType;
typedef Result<::bosdyn::api::spot_cam::GetAudioCaptureGainResponse> GetAudioCaptureGainResultType;

class AudioClient : public ServiceClient {
 public:
    AudioClient() = default;

    ~AudioClient() = default;

    std::shared_future<PlaySoundResultType> PlaySoundAsync(
        const std::string& sound_name, float gain,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<PlaySoundResultType> PlaySoundAsync(
        const std::string& sound_name,
        const RPCParameters& parameters = RPCParameters());

    PlaySoundResultType PlaySound(
        const std::string& sound_name, float gain,
        const RPCParameters& parameters = RPCParameters());

    PlaySoundResultType PlaySound(
        const std::string& sound_name,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<PlaySoundResultType> PlaySoundAsync(
        ::bosdyn::api::spot_cam::PlaySoundRequest& request,
        const RPCParameters& parameters = RPCParameters());

    PlaySoundResultType PlaySound(
        ::bosdyn::api::spot_cam::PlaySoundRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<DeleteSoundResultType> DeleteSoundAsync(
        const std::string& sound_name,
        const RPCParameters& parameters = RPCParameters());

    DeleteSoundResultType DeleteSound(
        const std::string& sound_name,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<DeleteSoundResultType> DeleteSoundAsync(
        ::bosdyn::api::spot_cam::DeleteSoundRequest& request,
        const RPCParameters& parameters = RPCParameters());

    DeleteSoundResultType DeleteSound(
        ::bosdyn::api::spot_cam::DeleteSoundRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<ListSoundsResultType> ListSoundsAsync(
        const RPCParameters& parameters = RPCParameters());
    ListSoundsResultType ListSounds(
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetVolumeResultType> SetVolumeAsync(
        float percentage, const RPCParameters& parameters = RPCParameters());

    SetVolumeResultType SetVolume(
        float percentage, const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetVolumeResultType> SetVolumeAsync(
        ::bosdyn::api::spot_cam::SetVolumeRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetVolumeResultType SetVolume(
        ::bosdyn::api::spot_cam::SetVolumeRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetVolumeResultType> GetVolumeAsync(
        const RPCParameters& parameters = RPCParameters());

    GetVolumeResultType GetVolume(
        const RPCParameters& parameters = RPCParameters());


    /*
    *
    * RPCs only for Spot CAM+IR
    *  
    */

    std::shared_future<SetAudioCaptureChannelResultType> SetAudioCaptureChannelAsync(
        ::bosdyn::api::spot_cam::AudioCaptureChannel channel,
        const RPCParameters& parameters = RPCParameters());

    SetAudioCaptureChannelResultType SetAudioCaptureChannel(
        ::bosdyn::api::spot_cam::AudioCaptureChannel channel,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetAudioCaptureChannelResultType> SetAudioCaptureChannelAsync(
        ::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetAudioCaptureChannelResultType SetAudioCaptureChannel(
        ::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetAudioCaptureChannelResultType> GetAudioCaptureChannelAsync(
        const RPCParameters& parameters = RPCParameters());

    GetAudioCaptureChannelResultType GetAudioCaptureChannel(
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetAudioCaptureGainResultType> SetAudioCaptureGainAsync(
        ::bosdyn::api::spot_cam::AudioCaptureChannel channel, double gain,
        const RPCParameters& parameters = RPCParameters());

    SetAudioCaptureGainResultType SetAudioCaptureGain(
        ::bosdyn::api::spot_cam::AudioCaptureChannel channel, double gain,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetAudioCaptureGainResultType> SetAudioCaptureGainAsync(
        ::bosdyn::api::spot_cam::SetAudioCaptureGainRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetAudioCaptureGainResultType SetAudioCaptureGain(
        ::bosdyn::api::spot_cam::SetAudioCaptureGainRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetAudioCaptureGainResultType> GetAudioCaptureGainAsync(
        ::bosdyn::api::spot_cam::AudioCaptureChannel channel,
        const RPCParameters& parameters = RPCParameters());

    GetAudioCaptureGainResultType GetAudioCaptureGain(
        ::bosdyn::api::spot_cam::AudioCaptureChannel channel,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetAudioCaptureGainResultType> GetAudioCaptureGainAsync(
        ::bosdyn::api::spot_cam::GetAudioCaptureGainRequest& request,
        const RPCParameters& parameters = RPCParameters());

    GetAudioCaptureGainResultType GetAudioCaptureGain(
        ::bosdyn::api::spot_cam::GetAudioCaptureGainRequest& request,
        const RPCParameters& parameters = RPCParameters());


    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam audio service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam audio service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnPlaySoundComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::PlaySoundRequest& request,
        ::bosdyn::api::spot_cam::PlaySoundResponse&& response, const grpc::Status& status,
        std::promise<PlaySoundResultType> promise);
    void OnDeleteSoundComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::DeleteSoundRequest& request,
        ::bosdyn::api::spot_cam::DeleteSoundResponse&& response, const grpc::Status& status,
        std::promise<DeleteSoundResultType> promise);
    void OnListSoundsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::ListSoundsRequest& request,
        ::bosdyn::api::spot_cam::ListSoundsResponse&& response, const grpc::Status& status,
        std::promise<ListSoundsResultType> promise);
    void OnSetVolumeComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetVolumeRequest& request,
        ::bosdyn::api::spot_cam::SetVolumeResponse&& response, const grpc::Status& status,
        std::promise<SetVolumeResultType> promise);
    void OnGetVolumeComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetVolumeRequest& request,
        ::bosdyn::api::spot_cam::GetVolumeResponse&& response, const grpc::Status& status,
        std::promise<GetVolumeResultType> promise);
    void OnSetAudioCaptureChannelComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetAudioCaptureChannelRequest& request,
        ::bosdyn::api::spot_cam::SetAudioCaptureChannelResponse&& response, const grpc::Status& status,
        std::promise<SetAudioCaptureChannelResultType> promise);
    void OnGetAudioCaptureChannelComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetAudioCaptureChannelRequest& request,
        ::bosdyn::api::spot_cam::GetAudioCaptureChannelResponse&& response, const grpc::Status& status,
        std::promise<GetAudioCaptureChannelResultType> promise);
    void OnSetAudioCaptureGainComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetAudioCaptureGainRequest& request,
        ::bosdyn::api::spot_cam::SetAudioCaptureGainResponse&& response, const grpc::Status& status,
        std::promise<SetAudioCaptureGainResultType> promise);
    void OnGetAudioCaptureGainComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetAudioCaptureGainRequest& request,
        ::bosdyn::api::spot_cam::GetAudioCaptureGainResponse&& response, const grpc::Status& status,
        std::promise<GetAudioCaptureGainResultType> promise);


    std::unique_ptr<::bosdyn::api::spot_cam::AudioService::Stub> m_stub;

    // Default service name for the spot cam audio service.
    static const char* s_default_service_name;

    // Default service type for the spotcam audio service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
