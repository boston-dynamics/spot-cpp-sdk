/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "message_pump.h"


namespace bosdyn {

namespace client {

void MessagePump::AutoUpdate(::bosdyn::common::Duration duration) {
    if (m_has_auto_update_started) return;
    m_auto_update_thread = std::make_unique<std::thread>(
        std::bind(&MessagePump::UpdateLoop, this, std::placeholders::_1), duration);
    m_has_auto_update_started = true;
}

void MessagePump::UpdateLoop(::bosdyn::common::Duration duration) {
    while (CompleteOne(duration) != Shutdown) {
        // do nothing here
    }
}

UpdateStatus MessagePump::CompleteOne(::bosdyn::common::Duration duration) {
    auto end_time = std::chrono::system_clock::now() + CONVERT_DURATION_FOR_GRPC(duration);
    void* tag = nullptr;
    bool ok = false;
    grpc::CompletionQueue::NextStatus next_status =
        m_completion_queue.AsyncNext(&tag, &ok, end_time);

    std::lock_guard<std::mutex> lock(m_shutdown_mutex);
    if (m_shutdown_requested) {
        // Don't wait for status==SHUTDOWN, gRPC won't return that until
        // it has drained its queue.
        return Shutdown;
    }

    switch (next_status) {
        case grpc::CompletionQueue::SHUTDOWN:
            return Shutdown;
        case grpc::CompletionQueue::TIMEOUT:
            return Complete;
        case grpc::CompletionQueue::GOT_EVENT:
            if (tag != nullptr) {
                MessagePumpCallBase* call_base = static_cast<MessagePumpCallBase*>(tag);
                if (call_base->OnCompletionQueueEvent(ok)) {
                    // IMPORTANT NOTE: The OutstandingCallTracker owns a unique_ptr to the call base object. The
                    // call base object will be deleted by this RemoveCall function because the unique_ptr will be
                    // when it is removed from the map in the OutstandingCallTracker.
                    m_outstanding_calls.RemoveCall(call_base);
                }
            }
            return Complete;
    }
    return Complete;
}

void MessagePump::RequestShutdown() {
    {
        std::lock_guard<std::mutex> lock(m_shutdown_mutex);
        if (m_shutdown_requested) {
            return;
        }
        m_shutdown_requested = true;
    }

    // The call to CancelAll will simply cancel the remaining call objects - setting the promise to
    // ClientCancelledOperationError and the call status to cancelled.
    m_outstanding_calls.CancelAll();

    // Shutdown the completion queue, which will try to complete any remaining calls.
    m_completion_queue.Shutdown();

    // Join the auto update thread to kill it.
    if (m_has_auto_update_started) {
        m_auto_update_thread->join();
        m_has_auto_update_started = false;
    }

    // Finally, delete all remaining call objects  from the OutstandingCallTracker. Note, we delete these at
    // the very end because a call object also contains references to m_context and m_response, and these
    // are referenced in any AsyncNext calls that could get issued within the CompleteOne function by the
    // completion queue or when trying to join the auto update thread.
    m_outstanding_calls.RemoveAllCalls();

}

void OutstandingCallTracker::CancelAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& call : m_calls) {
        call.first->Cancel();
    }
}

void OutstandingCallTracker::AddCall(std::unique_ptr<MessagePumpCallBase> call) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_calls[call.get()] = std::move(call);
}

void OutstandingCallTracker::RemoveCall(MessagePumpCallBase* call) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_calls.find(call);
    if (it != m_calls.end()) {
        m_calls.erase(it);
    }
}

size_t OutstandingCallTracker::Count() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_calls.size();
}

void OutstandingCallTracker::RemoveAllCalls() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_calls.clear();
}


}  // namespace client

}  // namespace bosdyn
