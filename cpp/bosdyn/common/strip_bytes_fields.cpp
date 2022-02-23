/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "strip_bytes_fields.h"

#include <map>
#include <string>

namespace bosdyn {

namespace common {

namespace strip_messages {

std::map<std::string, bool (&)(google::protobuf::Message*)> kWhitelistedBytesFieldsMap = {
    {"bosdyn.api.GetImageResponse", StripGetImageResponse},
    {"bosdyn.api.GetLocalGridsResponse", StripLocalGridResponse},
    {"bosdyn.api.GetPointCloudResponse", StripPointCloudResponse},
    {"bosdyn.api.graph_nav.UploadWaypointSnapshotRequest", StripUploadWaypointRequest},
    {"bosdyn.api.graph_nav.UploadEdgeSnapshotRequest", StripUploadEdgeRequest},
    {"bosdyn.api.graph_nav.DownloadWaypointSnapshotResponse", StripDownloadWaypointResponse},
    {"bosdyn.api.graph_nav.DownloadEdgeSnapshotResponse", StripDownloadEdgeResponse},
    {"bosdyn.api.RecordDataBlobsRequest", StripRecordDataBlobsRequest},
    {"bosdyn.api.RecordSignalTicksRequest", StripRecordSignalTickRequest},
    {"bosdyn.api.StoreImageRequest", StripStoreImageRequest},
    {"bosdyn.api.StoreDataRequest", StripStoreDataRequest},
};

bool StripGetImageResponse(::google::protobuf::Message* proto_message) {
    if (auto* image_message = dynamic_cast<::bosdyn::api::GetImageResponse*>(proto_message)) {
        for (int i = 0; i < image_message->image_responses_size(); i++) {
            image_message->mutable_image_responses(i)
                ->mutable_shot()
                ->mutable_image()
                ->clear_data();
        }
        return true;
    }
    return false;
}

bool StripLocalGridResponse(::google::protobuf::Message* proto_message) {
    if (auto* grid_message = dynamic_cast<::bosdyn::api::GetLocalGridsResponse*>(proto_message)) {
        for (int i = 0; i < grid_message->local_grid_responses_size(); i++) {
            grid_message->mutable_local_grid_responses(i)->mutable_local_grid()->clear_data();
        }
        return true;
    }
    return false;
}

bool StripPointCloudResponse(::google::protobuf::Message* proto_message) {
    if (auto* pc_message = dynamic_cast<::bosdyn::api::GetPointCloudResponse*>(proto_message)) {
        for (int i = 0; i < pc_message->point_cloud_responses_size(); i++) {
            pc_message->mutable_point_cloud_responses(i)->mutable_point_cloud()->clear_data();
        }
        return true;
    }
    return false;
}


bool StripUploadWaypointRequest(::google::protobuf::Message* proto_message) {
    if (auto* upload_msg =
            dynamic_cast<::bosdyn::api::graph_nav::UploadWaypointSnapshotRequest*>(proto_message)) {
        upload_msg->mutable_chunk()->clear_data();
        return true;
    }
    return false;
}

bool StripUploadEdgeRequest(::google::protobuf::Message* proto_message) {
    if (auto* upload_msg =
            dynamic_cast<::bosdyn::api::graph_nav::UploadEdgeSnapshotRequest*>(proto_message)) {
        upload_msg->mutable_chunk()->clear_data();
        return true;
    }
    return false;
}

bool StripDownloadWaypointResponse(::google::protobuf::Message* proto_message) {
    if (auto* download_msg =
            dynamic_cast<::bosdyn::api::graph_nav::DownloadWaypointSnapshotResponse*>(
                proto_message)) {
        download_msg->mutable_chunk()->clear_data();
        return true;
    }
    return false;
}

bool StripDownloadEdgeResponse(::google::protobuf::Message* proto_message) {
    if (auto* download_msg =
            dynamic_cast<::bosdyn::api::graph_nav::DownloadEdgeSnapshotResponse*>(proto_message)) {
        download_msg->mutable_chunk()->clear_data();
        return true;
    }
    return false;
}

bool StripLogAnnotationRequest(::google::protobuf::Message* proto_message) {
    if (auto* req_msg = dynamic_cast<::bosdyn::api::AddLogAnnotationRequest*>(proto_message)) {
        for (int i = 0; i < req_msg->mutable_annotations()->blob_data_size(); i++) {
            req_msg->mutable_annotations()->mutable_blob_data(i)->clear_data();
        }
        return true;
    }
    return false;
}

bool StripRecordDataBlobsRequest(::google::protobuf::Message* proto_message) {
    if (auto* req_msg = dynamic_cast<::bosdyn::api::RecordDataBlobsRequest*>(proto_message)) {
        for (int i = 0; i < req_msg->blob_data_size(); i++) {
            req_msg->mutable_blob_data(i)->clear_data();
        }
        return true;
    }
    return false;
}

bool StripRecordSignalTickRequest(::google::protobuf::Message* proto_message) {
    if (auto* req_msg = dynamic_cast<::bosdyn::api::RecordSignalTicksRequest*>(proto_message)) {
        for (int i = 0; i < req_msg->tick_data_size(); i++) {
            req_msg->mutable_tick_data(i)->clear_data();
        }
        return true;
    }
    return false;
}


bool StripStoreImageRequest(::google::protobuf::Message* proto_message) {
    if (auto* req_msg = dynamic_cast<::bosdyn::api::StoreImageRequest*>(proto_message)) {
        req_msg->mutable_image()->mutable_image()->clear_data();
        return true;
    }
    return false;
}

bool StripStoreDataRequest(::google::protobuf::Message* proto_message) {
    if (auto* req_msg = dynamic_cast<::bosdyn::api::StoreDataRequest*>(proto_message)) {
        req_msg->clear_data();
        return true;
    }
    return false;
}

}  // namespace strip_messages
}  // namespace common

}  // namespace bosdyn
