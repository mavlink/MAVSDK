#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <atomic>

#include "dronecode_sdk.h"
#include "system.h"
#include "mavlink_include.h"

namespace dronecode_sdk {

class DronecodeSDKImpl {
public:
    DronecodeSDKImpl();
    ~DronecodeSDKImpl();

    std::vector<uint64_t> get_system_uuids() const;
    System &get_system();
    System &get_system(uint64_t uuid);

    bool is_connected() const;
    bool is_connected(uint64_t uuid) const;

    void register_on_discover(DronecodeSDK::event_callback_t callback);
    void register_on_timeout(DronecodeSDK::event_callback_t callback);

    void notify_on_discover(uint64_t uuid);
    void notify_on_timeout(uint64_t uuid);

private:

    void make_system_with_component(uint8_t system_id, uint8_t component_id);
    bool does_system_exist(uint8_t system_id);

    using system_entry_t = std::pair<uint8_t, std::shared_ptr<System>>;

    mutable std::recursive_mutex _systems_mutex;
    std::map<uint8_t, std::shared_ptr<System>> _systems;

    DronecodeSDK::event_callback_t _on_discover_callback;
    DronecodeSDK::event_callback_t _on_timeout_callback;

    std::atomic<bool> _should_exit = {false};
};

} // namespace dronecode_sdk
