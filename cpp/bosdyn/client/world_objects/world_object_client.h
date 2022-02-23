/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/world_object.pb.h>
#include <bosdyn/api/world_object_service.grpc.pb.h>
#include <bosdyn/api/world_object_service.pb.h>
#include <google/protobuf/timestamp.pb.h>

#include <future>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "world_object_client_error_codes.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::ListWorldObjectResponse> ListWorldObjectsResultType;
typedef Result<::bosdyn::api::MutateWorldObjectResponse> MutateWorldObjectsResultType;

/**
 * The world object service provides a way to track and store objects detected in the world around
 * Spot. A world object is considered a higher-level item in the scene that has some amount of
 * semantic information associated with it
 */
class WorldObjectClient : public ServiceClient {
 public:
    // Constructor for the WorldObject client, which can be used to make RPC requests for any
    // objects detected by the perception system or to add/change/delete objects detected by client
    // applications to the perception scene of the robot.
    WorldObjectClient() = default;

    // Destructor for the WorldObjectClient class.
    ~WorldObjectClient() = default;

    // Asynchronous RPC method to list all world objects with no filters.
    std::shared_future<ListWorldObjectsResultType> ListWorldObjectsAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC method to list all world objects with no filters.
    ListWorldObjectsResultType ListWorldObjects(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC method to list all world objects taking the full request, which can
    // be used to specify filters on the object type or timestamp.
    std::shared_future<ListWorldObjectsResultType> ListWorldObjectsAsync(
        ::bosdyn::api::ListWorldObjectRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC method to list all world objects taking the full request, which can
    // be used to specify filters on the object type or timestamp.
    ListWorldObjectsResultType ListWorldObjects(
        ::bosdyn::api::ListWorldObjectRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous RPC method to mutate (add, change, delete) a world object.
    std::shared_future<MutateWorldObjectsResultType> MutateWorldObjectsAsync(
        ::bosdyn::api::MutateWorldObjectRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous RPC method to mutate (add, change, delete) a world object.
    MutateWorldObjectsResultType MutateWorldObjects(
        ::bosdyn::api::MutateWorldObjectRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Sets the QualityOfService enum for the world object client to be used for network selection
    // optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the robot state service, including the stub and processors
    // for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Get the default service name the WorldObject service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the WorldObject service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // List world objects response callback.
    void OnListWorldObjectsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListWorldObjectRequest& request,
        ::bosdyn::api::ListWorldObjectResponse&& response, const grpc::Status& status,
        std::promise<ListWorldObjectsResultType> promise);

    // Mutate world objects response callback.
    void OnMutateWorldObjectsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::MutateWorldObjectRequest& request,
        ::bosdyn::api::MutateWorldObjectResponse&& response, const grpc::Status& status,
        std::promise<MutateWorldObjectsResultType> promise);

    // The stub for the world object client to communicate it's rpc with (and pass to the robot).
    std::unique_ptr<::bosdyn::api::WorldObjectService::Stub> m_stub;

    // Default service name for the WorldObject service.
    static const char* s_default_service_name;

    // Default service type for the WorldObject service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
