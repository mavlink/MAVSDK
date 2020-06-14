#pragma once

#include "global_include.h"
#include "autopilot_interface.h"
#include "node_impl.h"

namespace mavsdk {

class AutopilotInterfaceImpl {
public:
    AutopilotInterfaceImpl(std::shared_ptr<NodeImpl> node_impl);

    uint64_t get_uuid() const;
private:
    std::shared_ptr<NodeImpl> node_impl() { return _node_impl; };

    uint64_t _uuid;

    void request_autopilot_version();
    void process_autopilot_version(const mavlink_message_t& message);

    std::shared_ptr<NodeImpl> _node_impl;
};

} // namespace mavsdk
