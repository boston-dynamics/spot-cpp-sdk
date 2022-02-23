/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <grpc++/grpc++.h>

namespace bosdyn {

namespace client {
class Authenticator : public grpc::MetadataCredentialsPlugin {
 public:
    explicit Authenticator(const std::function<const std::string&()>& getter_function)
        : m_get_user_token_function(getter_function) {}

    grpc::Status GetMetadata(grpc::string_ref service_url, grpc::string_ref method_name,
                             const grpc::AuthContext& channel_auth_context,
                             std::multimap<grpc::string, grpc::string>* metadata) override {
        std::string authorization_value = "Bearer " + m_get_user_token_function();
        metadata->insert(std::make_pair("authorization", authorization_value));
        return grpc::Status::OK;
    }

 private:
    std::function<const std::string&()> m_get_user_token_function;

};

class Channel {
 public:
    static std::shared_ptr<grpc::ChannelCredentials> CreateSecureChannelCreds(
        const std::string& cert, const std::function<const std::string&()>& getter_function);

    /**
     * Set up channel arguments for secure and insecure channels.
     *
     * @param channel_args(grpc::ChannelArguments): Channel arguments object to update.
     */
    static void SetupChannelArgs(grpc::ChannelArguments* channel_args);

    static std::shared_ptr<grpc::Channel> CreateSecureChannel(
        const std::string& address, int port, std::shared_ptr<grpc::ChannelCredentials> creds,
        const std::string& authority);

    static std::shared_ptr<grpc::Channel> CreateInsecureChannel(const std::string& address,
                                                                int port);
};

// All client channels and services are configured to allow messages up to 100 MB in size by
// default. The value of this variable needs to match the value of the global variable
// DEFAULT_MAX_MESSAGE_LENGTH in
// public_api/bosdyn/python/bosdyn-client/src/bosdyn/client/channel.py.
constexpr int kDefaultMaxMessageLengthBytes = 100 * 1024 * 1024;

}  // namespace client

}  // namespace bosdyn
