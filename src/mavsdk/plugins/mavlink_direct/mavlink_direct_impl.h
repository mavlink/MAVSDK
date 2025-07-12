#pragma once

#include "plugins/mavlink_direct/mavlink_direct.h"

#include "plugin_impl_base.h"

#include <json/json.h>
#include <mav/Message.h>
#include <optional>
#include <map>
#include <mutex>
#include "handle_factory.h"

namespace mavsdk {

class MavlinkDirectImpl : public PluginImplBase {
public:
    explicit MavlinkDirectImpl(System& system);
    explicit MavlinkDirectImpl(std::shared_ptr<System> system);

    ~MavlinkDirectImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    MavlinkDirect::Result send_message(MavlinkDirect::MavlinkMessage message);

    MavlinkDirect::MessageHandle
    subscribe_message(std::string message_name, const MavlinkDirect::MessageCallback& callback);

    void unsubscribe_message(MavlinkDirect::MessageHandle handle);

private:
    // Handle factory
    HandleFactory<MavlinkDirect::MavlinkMessage> _message_handle_factory{};

    // Callback storage
    std::mutex _message_callbacks_mutex;
    std::map<MavlinkDirect::MessageHandle, MavlinkDirect::MessageCallback> _message_callbacks;
    std::map<MavlinkDirect::MessageHandle, std::string> _message_handle_to_name;

    bool _debugging = false;

    // Helper methods for message name/ID conversion
    std::optional<uint32_t> message_name_to_id(const std::string& name) const;
    std::optional<std::string> message_id_to_name(uint32_t id) const;

    // Helper methods for JSON field conversion
    Json::Value libmav_to_json(const mav::Message& msg) const;
    bool json_to_libmav(const Json::Value& json, mav::Message& msg) const;
    bool json_to_libmav_message(const std::string& json_string, mav::Message& msg) const;

    // Message handling
    void process_mavlink_message(mavlink_message_t& message);
};

} // namespace mavsdk