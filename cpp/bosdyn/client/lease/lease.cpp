/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "lease.h"
#include "bosdyn/client/lease/lease_resources.h"
#include "bosdyn/common/assert_precondition.h"

#include <mutex>

namespace {
::bosdyn::api::ResourceTree s_default_resource;
::bosdyn::client::ResourceHierarchy s_default_hierarchy;
std::once_flag resource_flag;
std::once_flag hierarchy_flag;

void build_default_resource(::bosdyn::client::LeaseHierarchyRequirements use_arm) {
    s_default_resource.set_resource(::bosdyn::client::kBodyResource);
    s_default_resource.add_sub_resources()->set_resource(::bosdyn::client::kMobilityResource);
    s_default_resource.add_sub_resources()->set_resource(::bosdyn::client::kFanResource);
    if (use_arm == ::bosdyn::client::LeaseHierarchyRequirements::ARM_AND_GRIPPER) {
        auto* full_arm = s_default_resource.add_sub_resources();
        full_arm->set_resource(::bosdyn::client::kFullArmResource);
        full_arm->add_sub_resources()->set_resource(::bosdyn::client::kArmResource);
        full_arm->add_sub_resources()->set_resource(::bosdyn::client::kGripperResource);
    }
}

void build_default_hierarchy(::bosdyn::client::LeaseHierarchyRequirements use_arm) {
    s_default_hierarchy =
        ::bosdyn::client::ResourceHierarchy(::bosdyn::client::DefaultResourceTree(use_arm));
}

const char kClientNameExtender[] = "<unknown>";

void updateClientNamesList(::bosdyn::api::Lease* lease_proto) {
    if (lease_proto->sequence_size() < lease_proto->client_names_size()) {
        // Truncate the client names to match the size of the sequence.
        const int size_change = lease_proto->client_names_size() - lease_proto->sequence_size();
        for (int i = 0; i < size_change; i++) {
            lease_proto->mutable_client_names()->RemoveLast();
        }
    } else if (lease_proto->sequence_size() > lease_proto->client_names_size()) {
        // Extend the client names to match the size of the sequence.
        const int size_change = lease_proto->sequence_size() - lease_proto->client_names_size();
        for (int i = 0; i < size_change; i++) {
            lease_proto->add_client_names(kClientNameExtender);
        }
    }
}

}  // namespace

namespace bosdyn {

namespace client {

LeaseUseResultsAndError::LeaseUseResultsAndError(::bosdyn::api::LeaseUseResult* lease_use,
                                                 std::string* error) {
    lease_use_result = lease_use;
    error_message = error;
}

void LeaseUseResultsAndError::MaybeSetErrorMessage(const std::string& error) {
    if (error_message) {
        *error_message = error;
    }
}

void LeaseUseResultsAndError::SetStatus(const ::bosdyn::api::LeaseUseResult::Status& status) {
    lease_use_result->set_status(status);
}

void LeaseUseResultsAndError::SetStatusAndErrorMessage(
    const ::bosdyn::api::LeaseUseResult::Status& status, const std::string& error) {
    this->SetStatus(status);
    this->MaybeSetErrorMessage(error);
}

void LeaseUseResultsAndError::MergeLeaseUseResult(const ::bosdyn::api::LeaseUseResult& res) {
    lease_use_result->MergeFrom(res);
}

void LeaseUseResultsAndError::SetLeaseUseResult(const ::bosdyn::api::LeaseUseResult& res) {
    *lease_use_result = res;
}

const ::bosdyn::api::ResourceTree& DefaultResourceTree(LeaseHierarchyRequirements use_arm) {
    std::call_once(resource_flag, build_default_resource, use_arm);
    return s_default_resource;
}
const ::bosdyn::client::ResourceHierarchy& DefaultResourceHierarchy(
    LeaseHierarchyRequirements use_arm) {
    std::call_once(hierarchy_flag, build_default_hierarchy, use_arm);
    return s_default_hierarchy;
}

Lease::Lease(const LeaseProto& lease_proto)
    : m_lease_proto(lease_proto), m_lease_status(LeaseStatus::SELF_OWNER) {
    updateClientNamesList(&m_lease_proto);
}

Lease::Lease(const LeaseProto& lease_proto, const LeaseStatus& lease_status)
    : m_lease_proto(lease_proto), m_lease_status(lease_status) {
    updateClientNamesList(&m_lease_proto);
}

Lease::CompareResult Lease::Compare(const Lease& other_lease) const {
    if (!IsValid() || !other_lease.IsValid()) {
        return CompareResult::INVALID;
    }
    // Sequences are only valid for leases with the same resource. Fail early if the leases are for
    // different resources.
    if (m_lease_proto.resource() != other_lease.m_lease_proto.resource()) {
        return CompareResult::DIFFERENT_RESOURCES;
    }
    return CompareIgnoringResource(other_lease);
}

Lease::CompareResult Lease::CompareIgnoringResource(const Lease& other_lease) const {
    if (m_lease_proto.epoch() != other_lease.m_lease_proto.epoch()) {
        return CompareResult::DIFFERENT_EPOCHS;
    }

    // If any sequence numbers are different within the common subset of sequence lengths, then one
    // Lease is newer than the other.
    const size_t sequence_size = m_lease_proto.sequence_size();
    const size_t other_sequence_size = other_lease.m_lease_proto.sequence_size();
    const size_t common_sequence_size = std::min(sequence_size, other_sequence_size);
    for (size_t i = 0; i < common_sequence_size; ++i) {
        const uint32_t sequence_num = m_lease_proto.sequence(i);
        const uint32_t other_sequence_num = other_lease.m_lease_proto.sequence(i);
        if (sequence_num < other_sequence_num) {
            return CompareResult::OLDER;
        } else if (sequence_num > other_sequence_num) {
            return CompareResult::NEWER;
        }
    }

    // At this point, the sequence numbers are different within the common subset. If one Lease has
    // more sequence numbers than the other Lease, it is a sublease of the lease use result's lease
    // and considered newer.
    if (sequence_size < other_sequence_size) {
        return CompareResult::SUPER_LEASE;
    } else if (sequence_size > other_sequence_size) {
        return CompareResult::SUB_LEASE;
    }

    // Leases are the same: resources match, and sequences match exactly.
    return CompareResult::SAME;
}

Lease Lease::Increment() const {
    if (!IsValid()) {
        return Lease();
    }

    LeaseProto new_proto = m_lease_proto;
    int last_index = new_proto.sequence_size() - 1;
    new_proto.set_sequence(last_index, new_proto.sequence(last_index) + 1);

    return Lease(new_proto);
}

Lease Lease::DuplicateSequence(const Lease& other) const {
    LeaseProto new_proto = m_lease_proto;
    *new_proto.mutable_sequence() = other.m_lease_proto.sequence();
    *new_proto.mutable_client_names() = other.m_lease_proto.client_names();
    return Lease(new_proto, m_lease_status);
}

Lease Lease::CreateSublease(const std::string& client_name) const {
    if (!IsValid()) {
        return Lease();
    }

    LeaseProto new_proto = m_lease_proto;
    new_proto.add_sequence(0);

    // Add in the client name to track who has used and subleased the lease.
    new_proto.add_client_names(client_name);

    return Lease(new_proto);
}

const LeaseProto& Lease::GetProto() const { return m_lease_proto; }

const std::string& Lease::GetResource() const { return m_lease_proto.resource(); }

bool Lease::IsValid() const {
    return !m_lease_proto.resource().empty() && m_lease_proto.sequence_size() > 0;
}

void Lease::UpdateFromLeaseUseResult(const ::bosdyn::api::LeaseUseResult& lease_use_result) {
    if (lease_use_result.status() == ::bosdyn::api::LeaseUseResult::STATUS_OLDER) {
        // The lease from the lease wallet was an older lease.
        if (Compare(Lease(lease_use_result.attempted_lease())) == CompareResult::SAME) {
            // If the lease use result's lease matches this current lease, then mark it as other
            // owner since it was found to be an older lease and we never incremented it.
            m_lease_status = LeaseStatus::OTHER_OWNER;
        }
    } else if (lease_use_result.status() == ::bosdyn::api::LeaseUseResult::STATUS_WRONG_EPOCH) {
        // The lease from the lease wallet is the wrong epoch.
        if (Compare(Lease(lease_use_result.attempted_lease())) == CompareResult::SAME) {
            // If the lease use result's lease matches this current lease, then mark it as unowned
            // because it is now entirely wrong (different epochs) for the resource.
            m_lease_status = LeaseStatus::UNOWNED;
        }
    } else if (lease_use_result.status() == ::bosdyn::api::LeaseUseResult::STATUS_REVOKED) {
        // The lease from the lease wallet is the wrong epoch.
        if (Compare(Lease(lease_use_result.attempted_lease())) == CompareResult::SAME) {
            // If the lease use result's lease matches this current lease, then mark it as revoked.
            m_lease_status = LeaseStatus::REVOKED;
        }
    }
}

Lease Lease::SplitLease(const std::string& resource, const ResourceHierarchy& hierarchy) const {
    if (m_lease_proto.resource() == resource) {
        return *this;
    }
    if (!hierarchy.HasResource(resource)) {
        // Return an invalid lease.
        return Lease();
    }

    Lease result(*this);
    const auto& sub_hierarchy = hierarchy.GetHierarchy(resource);
    const auto& resource_tree = sub_hierarchy.ResourceTree();
    result.m_lease_proto.set_resource(resource_tree.resource());
    return result;
}

ResourceHierarchy::ResourceHierarchy(const ::bosdyn::api::ResourceTree& resource_tree)
    : m_resource_tree(resource_tree) {
    if (resource_tree.sub_resources_size() == 0) {
        m_leaf_resources.insert(resource_tree.resource());
    }
    for (const auto& sub_tree : resource_tree.sub_resources()) {
        auto it = m_sub_hierarchies.emplace(sub_tree.resource(), ResourceHierarchy(sub_tree));

        // Merge in all the sub-resources from the new hierarchy so that they are quickly
        // accessible.
        const auto& new_sub = it.first->second;
        m_sub_hierarchies.insert(new_sub.m_sub_hierarchies.begin(),
                                 new_sub.m_sub_hierarchies.end());

        m_leaf_resources.insert(new_sub.m_leaf_resources.begin(), new_sub.m_leaf_resources.end());
    }
}

bool ResourceHierarchy::HasResource(const std::string& resource) const {
    return resource == m_resource_tree.resource() || m_sub_hierarchies.count(resource);
}

bool ResourceHierarchy::HasSubResources() const { return !m_sub_hierarchies.empty(); }

const std::string& ResourceHierarchy::Resource() const { return m_resource_tree.resource(); }

Lease ResourceHierarchy::SubResourceLease(const Lease& parent, const std::string& resource) const {
    if (!HasResource(resource)) return Lease();
    LeaseProto proto = parent.GetProto();
    proto.set_resource(resource);
    return Lease(proto);
}

const ::bosdyn::api::ResourceTree& ResourceHierarchy::ResourceTree() const {
    return m_resource_tree;
}

bool ResourceHierarchy::MaybeGetHierarchy(const std::string& resource,
                                          ResourceHierarchy* hierarchy) {
    if (!HasResource(resource)) return false;
    *hierarchy = GetHierarchy(resource);
    return true;
}

const ResourceHierarchy& ResourceHierarchy::GetHierarchy(const std::string& resource) const {
    if (resource == m_resource_tree.resource()) return *this;
    return m_sub_hierarchies.at(resource);
}

const std::set<std::string>& ResourceHierarchy::LeafResources() const { return m_leaf_resources; }

}  // namespace client

}  // namespace bosdyn
