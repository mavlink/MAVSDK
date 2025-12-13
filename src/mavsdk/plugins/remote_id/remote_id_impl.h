#pragma once

#include "plugins/remote_id/remote_id.h"

#include "plugin_impl_base.h"
#include "callback_list.h"

#include <mutex>

namespace mavsdk {

class RemoteIdImpl : public PluginImplBase {
public:
    explicit RemoteIdImpl(System& system);
    explicit RemoteIdImpl(std::shared_ptr<System> system);

    ~RemoteIdImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    RemoteId::Result set_basic_id(RemoteId::BasicId basic_id);

    RemoteId::Result set_location(RemoteId::Location location);

    RemoteId::Result set_system(RemoteId::SystemId system);

    RemoteId::Result set_operator_id(RemoteId::OperatorId operator_id);

    RemoteId::Result set_self_id(RemoteId::SelfId self_id);

    RemoteId::ArmStatusHandle subscribe_arm_status(const RemoteId::ArmStatusCallback& callback);

    void unsubscribe_arm_status(RemoteId::ArmStatusHandle handle);

    RemoteId::ArmStatus arm_status() const;

private:
    void process_arm_status(const mavlink_message_t& message);

    std::mutex _subscription_mutex{};
    CallbackList<RemoteId::ArmStatus> _arm_status_subscriptions{};

    mutable std::mutex _arm_status_mutex{};
    RemoteId::ArmStatus _arm_status{};
};

} // namespace mavsdk