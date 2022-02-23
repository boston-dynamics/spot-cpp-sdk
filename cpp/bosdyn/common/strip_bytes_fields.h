/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <bosdyn/api/image.pb.h>
#include <bosdyn/api/local_grid.pb.h>
#include <bosdyn/api/point_cloud.pb.h>
#include <bosdyn/api/data_acquisition_store.pb.h>
#include <bosdyn/api/data_buffer.pb.h>
#include <bosdyn/api/graph_nav/graph_nav.pb.h>
#include <bosdyn/api/graph_nav/nav.pb.h>
#include <bosdyn/api/log_annotation.pb.h>
#include <functional>

namespace bosdyn {

namespace common {

namespace strip_messages {

/**
 * Creates a map linking a protobuf message type (string) to the specific function
 * which will remove large fields that should not be added in the grpc logs or copied
 * requests in a response header.
 */
extern std::map<std::string, bool (&)(google::protobuf::Message*)> kWhitelistedBytesFieldsMap;

/**
 * Message-specific helper functions that perform a dynamic cast to convert the
 * generic protobuf message into a specific type of message and remove the bytes
 * fields (or other large) fields from that message.
 */
bool StripGetImageResponse(::google::protobuf::Message* proto_message);
bool StripLocalGridResponse(::google::protobuf::Message* proto_message);
bool StripPointCloudResponse(::google::protobuf::Message* proto_message);
bool StripUploadWaypointRequest(::google::protobuf::Message* proto_message);
bool StripUploadEdgeRequest(::google::protobuf::Message* proto_message);
bool StripDownloadWaypointResponse(::google::protobuf::Message* proto_message);
bool StripDownloadEdgeResponse(::google::protobuf::Message* proto_message);
bool StripLogAnnotationRequest(::google::protobuf::Message* proto_message);
bool StripRecordDataBlobsRequest(::google::protobuf::Message* proto_message);
bool StripRecordSignalTickRequest(::google::protobuf::Message* proto_message);
bool StripStoreImageRequest(::google::protobuf::Message* proto_message);
bool StripStoreDataRequest(::google::protobuf::Message* proto_message);

}  // namespace strip_messages
}  // namespace common

}  // namespace bosdyn
