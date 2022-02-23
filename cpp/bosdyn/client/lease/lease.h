/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <string>
#include <vector>

#include <bosdyn/api/lease.pb.h>

namespace bosdyn {

namespace client {

typedef ::bosdyn::api::Lease LeaseProto;
class Lease;

enum class LeaseStatus {
    UNOWNED,
    REVOKED,
    SELF_OWNER,
    OTHER_OWNER,
    NOT_MANAGED,
};

enum class LeaseHierarchyRequirements {
    BASE_ONLY,
    ARM_AND_GRIPPER,
};

/**
 * Combines the LeaseUseResult and an associated error message for error
 * tracking of leases.
 *
 * Args:
 *      lease_use_result: Non-null pointer to the lease use result proto for a RPC.
 *      error_message: Allowed to be null. Represents an error string, often the
 *          header error message. Helpers will check that the string exists before
 *          trying to set an error message.
 *
 * The LeaseUseResultsAndError can be constructed from either:
 *      - Direct pointers to a LeaseUseResults proto and error message string.
 *      - A RPC response proto which contains both a ResponseHeader field
 * and a LeaseUseResult field.
 */
struct LeaseUseResultsAndError {
    ::bosdyn::api::LeaseUseResult* lease_use_result = nullptr; // Can NOT be null. This is asserted.
    std::string* error_message = nullptr; // Allowed to be null.

    LeaseUseResultsAndError() = default;
    LeaseUseResultsAndError(::bosdyn::api::LeaseUseResult* lease_use, std::string* error);

    template <class ResponseWithLease>
    explicit LeaseUseResultsAndError(ResponseWithLease* response) {
        if (!response) return;
        lease_use_result = response->mutable_lease_use_result();
        error_message = response->mutable_header()->mutable_error()->mutable_message();
    }

    // Helper function to set both the lease use result status and the error message if
    // one exists in the struct.
    void SetStatusAndErrorMessage(const ::bosdyn::api::LeaseUseResult::Status& status,
                                  const std::string& error);

    // Helper function to set an error message if one exists in the struct.
    void MaybeSetErrorMessage(const std::string& error);

    // Helper function to set the lease use result status.
    void SetStatus(const ::bosdyn::api::LeaseUseResult::Status& status);

    // Helper function to merge an incoming lease use results proto with the existing one
    // tracked in the struct.
    void MergeLeaseUseResult(const ::bosdyn::api::LeaseUseResult& res);

    // Helper to overwrite the existing lease use results proto with a pointer to this new
    // lease use result proto.
    void SetLeaseUseResult(const ::bosdyn::api::LeaseUseResult& res);
};

// Represents a tree of resources, making it easy to jump to anywhere in the tree.
// It takes the ResourceTree proto, and processes it into useful maps and sets to
// make accessing relevant information quick and easy.
class ResourceHierarchy {
 public:
    ResourceHierarchy() = default;
    explicit ResourceHierarchy(const ::bosdyn::api::ResourceTree& resource_tree);
    ResourceHierarchy(const ResourceHierarchy& other) = default;
    ResourceHierarchy& operator=(const ResourceHierarchy& other) = default;

    // Is the specified resource contained somewhere within the hierarchy?
    bool HasResource(const std::string& resource) const;
    bool HasSubResources() const;

    const std::string& Resource() const;

    Lease SubResourceLease(const Lease& parent, const std::string& resource) const;

    // Access the Resource proto.
    const ::bosdyn::api::ResourceTree& ResourceTree() const;

    // Get a sub-tree corresponding to the specified resource.
    const ResourceHierarchy& GetHierarchy(const std::string& resource) const;

    const std::set<std::string>& LeafResources() const;

    // iterate through all sub resources.
    std::map<std::string, ResourceHierarchy>::const_iterator begin() const {
        return m_sub_hierarchies.begin();
    }
    std::map<std::string, ResourceHierarchy>::const_iterator end() const {
        return m_sub_hierarchies.end();
    }

    bool MaybeGetHierarchy(const std::string& resource, ResourceHierarchy* hierarchy);

 private:
    ::bosdyn::api::ResourceTree m_resource_tree;
    std::map<std::string, ResourceHierarchy> m_sub_hierarchies;
    std::set<std::string> m_leaf_resources;
};

const ::bosdyn::api::ResourceTree& DefaultResourceTree(LeaseHierarchyRequirements use_arm);
const ::bosdyn::client::ResourceHierarchy& DefaultResourceHierarchy(
    LeaseHierarchyRequirements use_arm);

// Leases are used to coordinate exclusive access to shared resources on a Boston Dynamics robot.
//
// A service will grant access to the shared resource if the lease which accompanies a request is
// "more recent" than any previously seen leases. Recency is determined using a sequence of
// monotonically increasing numbers, similar to a Lamport logical clock.
class Lease {
 public:
    // Default Lease constructor is supported. However, the Lease object will be in an Invalid state
    // (IsValid() will return false). Prefer to use the constructor with resource/sequence
    // arguments, or the copy constructor with a valid Lease.
    Lease() = default;

    // Copy constructor.
    Lease(const Lease&) = default;

    // Create a lease for the |resource| string using the logical clock specified by |sequence|.
    // Uses a default status of SELF_OWNER.
    explicit Lease(const LeaseProto& lease_proto);

    // Create a lease for the |resource| string using the logical clock specified by |sequence|.
    // Uses the status input to the constructor.
    explicit Lease(const LeaseProto& lease_proto, const LeaseStatus& lease_status);

    // Destructor.
    ~Lease() = default;

    // Assignment. A Lease is generally immutable, but assignment operation necessary for practical
    // application.
    Lease& operator=(const Lease&) = default;

    // Result of comparing two leases.
    enum class CompareResult {
        // This lease is the same as the other lease.
        SAME,

        // This lease is a "super-lease" of the other lease - in other words, the other lease is a
        // sublease of this lease.
        SUPER_LEASE,

        // This lease is a sub-lease of the other lease.
        SUB_LEASE,

        // This lease is older than the other lease.
        OLDER,

        // This lease is newer than the other lease.
        NEWER,

        // This lease is for a different resource than the other lease. There is no way to compare
        // recency/time of Leases for two different resources.
        DIFFERENT_RESOURCES,

        // This is lease is for a different epoch than the other lease. There is no way to compare
        // recency/time of Leases for two different epochs.
        DIFFERENT_EPOCHS,

        // At lease one of the leases is invalid. No way to compare recency/time of invalid Leases.
        INVALID,
    };

    // Compare this lease with another lease object.
    CompareResult Compare(const Lease& lease) const;

    // When using hierarchical resources, it is useful to compare leases without regard to resource.
    CompareResult CompareIgnoringResource(const Lease& lease) const;

    // Create a new Lease which is more recent than the current lease.
    Lease Increment() const;

    // Create a new Lease which duplicates the sequence number of another lease.
    Lease DuplicateSequence(const Lease& other) const;

    // Create a sublease derived from the current lease.
    Lease CreateSublease(const std::string& client_name) const;

    // Accessor for underlying lease information.
    const LeaseProto& GetProto() const;

    // Accessor for the resource of the lease.
    const std::string& GetResource() const;

    // Check if this lease is valid. Leases are valid if they are for a non-empty resource string,
    // with a non-empty sequence.
    bool IsValid() const;

    // Update internal instance of LeaseState from given lease.
    void UpdateFromLeaseUseResult(const ::bosdyn::api::LeaseUseResult& lease_use_result);

    // Get the current ownership status of the lease.
    LeaseStatus GetLeaseStatus() const { return m_lease_status; }

    // Check if we are the owner of the lease based on the status.
    bool IsLeaseSelfOwned() const { return m_lease_status == LeaseStatus::SELF_OWNER; }

    // Split up a Lease by extracting a single sub-resource.
    Lease SplitLease(const std::string& resource, const ResourceHierarchy& hierarchy) const;

 private:
    LeaseProto m_lease_proto;

    // ::bosdyn::common::Status of the current lease stored in m_lease_proto
    LeaseStatus m_lease_status = LeaseStatus::NOT_MANAGED;
};


}  // namespace client

}  // namespace bosdyn
