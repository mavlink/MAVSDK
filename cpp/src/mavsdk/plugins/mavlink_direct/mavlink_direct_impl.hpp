#pragma once

#include "plugins/mavlink_direct/mavlink_direct.hpp"

#include "plugin_impl_base.hpp"
#include "callback_list.hpp"

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
    CallbackList<MavlinkDirect::MavlinkMessage> _callbacks{_system_impl->io_context()};
    Handle<Mavsdk::MavlinkMessage> _system_subscription{};

    bool _debugging = false;

    // Message handling
    void process_mavlink_message(mavlink_message_t& message);
};

} // namespace mavsdk
