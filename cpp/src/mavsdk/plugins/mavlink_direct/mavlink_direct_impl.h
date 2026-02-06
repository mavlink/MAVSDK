#pragma once

#include "plugins/mavlink_direct/mavlink_direct.h"

#include "plugin_impl_base.h"
#include "callback_list.h"

#include <json/json.h>
#include <mav/Message.h>
#include <optional>
#include <map>
#include <mutex>

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

    MavlinkDirect::Result load_custom_xml(const std::string& xml_content);

private:
    // Internal callback management
    CallbackList<MavlinkDirect::MavlinkMessage> _callbacks{};
    Handle<Mavsdk::MavlinkMessage> _system_subscription{};

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
