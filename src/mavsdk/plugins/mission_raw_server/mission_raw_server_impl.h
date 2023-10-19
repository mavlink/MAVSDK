#pragma once

#include "plugins/mission_raw_server/mission_raw_server.h"
#include "server_plugin_impl_base.h"
#include "callback_list.h"

#include <thread>
#include <condition_variable>

namespace mavsdk {

class MissionRawServerImpl : public ServerPluginImplBase {
public:
    explicit MissionRawServerImpl(std::shared_ptr<ServerComponent> server_component);

    ~MissionRawServerImpl() override;

    void init() override;
    void deinit() override;

    MissionRawServer::IncomingMissionHandle
    subscribe_incoming_mission(const MissionRawServer::IncomingMissionCallback& callback);
    void unsubscribe_incoming_mission(MissionRawServer::IncomingMissionHandle handle);

    MissionRawServer::CurrentItemChangedHandle
    subscribe_current_item_changed(const MissionRawServer::CurrentItemChangedCallback& callback);
    void unsubscribe_current_item_changed(MissionRawServer::CurrentItemChangedHandle handle);

    MissionRawServer::ClearAllHandle
    subscribe_clear_all(const MissionRawServer::ClearAllCallback& callback);
    void unsubscribe_clear_all(MissionRawServer::ClearAllHandle handle);

    void set_current_item_complete();

    MissionRawServer::MissionPlan incoming_mission() const;
    MissionRawServer::MissionItem current_item_changed() const;
    uint32_t clear_all() const;

private:
    void process_mission_count(const mavlink_message_t& message);
    void process_mission_set_current(const mavlink_message_t& message);
    void process_mission_clear(const mavlink_message_t message);

    CallbackList<MissionRawServer::Result, MissionRawServer::MissionPlan>
        _incoming_mission_callbacks{};
    CallbackList<MissionRawServer::MissionItem> _current_item_changed_callbacks{};
    CallbackList<uint32_t> _clear_all_callbacks{};
    std::atomic<int> _target_system_id;
    std::atomic<int> _target_component_id;
    std::atomic<int> _mission_count;
    std::atomic<bool> _mission_completed;

    std::vector<MavlinkMissionTransferServer::ItemInt> _current_mission;
    std::size_t _current_seq;

    std::weak_ptr<MavlinkMissionTransferServer::WorkItem> _last_download{};

    void set_current_seq(std::size_t seq);
};

} // namespace mavsdk
