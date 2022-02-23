/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "channel.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

std::shared_ptr<grpc::ChannelCredentials> Channel::CreateSecureChannelCreds(
    const std::string& cert, const std::function<const std::string&()>& getter_function) {
    // Set transport creds.
    grpc::SslCredentialsOptions creds;
    creds.pem_root_certs = cert;

    // Create an SSL ChannelCredentials object.
    auto channel_creds = grpc::SslCredentials(creds);
    auto call_creds = grpc::MetadataCredentialsFromPlugin(
        std::unique_ptr<grpc::MetadataCredentialsPlugin>(new Authenticator(getter_function)));

    std::shared_ptr<grpc::ChannelCredentials> combined_creds =
        grpc::CompositeChannelCredentials(channel_creds, call_creds);
    return combined_creds;
}

void Channel::SetupChannelArgs(grpc::ChannelArguments* channel_args) {
    // Make gRPC reconnect more aggressively and shutdown faster.



    channel_args->SetInt(GRPC_ARG_MIN_RECONNECT_BACKOFF_MS, 10000);
    channel_args->SetInt(GRPC_ARG_MAX_RECEIVE_MESSAGE_LENGTH, kDefaultMaxMessageLengthBytes);
    channel_args->SetInt(GRPC_ARG_MAX_SEND_MESSAGE_LENGTH, kDefaultMaxMessageLengthBytes);
}

std::shared_ptr<grpc::Channel> Channel::CreateSecureChannel(
    const std::string& address, int port, std::shared_ptr<grpc::ChannelCredentials> creds,
    const std::string& authority) {
    grpc::ChannelArguments channel_args;
    channel_args.SetSslTargetNameOverride(authority);
    SetupChannelArgs(&channel_args);
    std::string dest = address + ":" + std::to_string(port);
    std::shared_ptr<grpc::Channel> secure_channel =
        grpc::CreateCustomChannel(dest, creds, channel_args);
    return secure_channel;
}

std::shared_ptr<grpc::Channel> Channel::CreateInsecureChannel(const std::string& address,
                                                              int port) {
    grpc::ChannelArguments channel_args;
    SetupChannelArgs(&channel_args);

    std::string dest = address + ":" + std::to_string(port);
    std::shared_ptr<grpc::Channel> insecure_channel =
            grpc::CreateCustomChannel(dest, grpc::InsecureChannelCredentials(), channel_args);
    return insecure_channel;
}

}  // namespace client

}  // namespace bosdyn
