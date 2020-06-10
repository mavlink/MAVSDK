#pragma once

#include "global_include.h"
#include "autopilot_node.h"
#include "node_impl.h"

namespace mavsdk {

class AutopilotNodeImpl : public NodeImpl {
public:
    uint64_t get_uuid() const;

    static bool is_autopilot(Node& node);
private:
    void request_autopilot_version();
};

} // namespace mavsdk
