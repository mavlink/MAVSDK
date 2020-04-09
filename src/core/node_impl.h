#pragma once

#include "global_include.h"
#include "mavlink_address.h"
#include "mavlink_include.h"
#include "mavlink_parameters.h"
#include "mavlink_commands.h"
#include "mavlink_message_handler.h"
#include "mavlink_mission_transfer.h"
#include "timeout_handler.h"
#include "call_every_handler.h"
#include "thread_pool.h"
#include "timesync.h"
#include "system.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_set>
#include <map>
#include <thread>
#include <mutex>
#include <future>

namespace mavsdk {

class MavsdkImpl;
class PluginImplBase;

// This class is the impl of Node. this is to hide the private methods
// and functionality from the public library API.
class NodeImpl : public Sender {
public:
    explicit NodeImpl(
            MavsdkImpl& parent, uint8_t system_id, uint8_t component_id);
    ~NodeImpl();

    void process_mavlink_message(mavlink_message_t& message);

    uint8_t get_system_id();
    uint8_t get_component_id();
    bool is_connected();

    bool send_message(mavlink_message_t& message) override;

    // Non-copyable
    NodeImpl(const NodeImpl&) = delete;
    const NodeImpl& operator=(const NodeImpl&) = delete;

private:
    MavsdkImpl& _parent;
    std::atomic<bool> _should_exit{false};
    MAVLinkAddress node_address{};
};

} // namespace mavsdk
