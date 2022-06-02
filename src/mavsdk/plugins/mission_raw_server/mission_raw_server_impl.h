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
    CallbackList<MissionRawServer::Result, MissionRawServer::MissionPlan>
        _incoming_mission_callbacks{};
    CallbackList<MissionRawServer::MissionItem> _current_item_changed_callbacks{};
    CallbackList<uint32_t> _clear_all_callbacks{};
    std::thread _thread_mission;
    std::atomic<int> _target_component;
    std::atomic<int> _mission_count;
    std::atomic<bool> _mission_completed;

    std::queue<std::function<void()>> _work_queue;
    std::condition_variable _wait_for_new_task;
    std::mutex _work_mutex;
    std::atomic<bool> _stop_work_thread = false;

    std::vector<MavlinkMissionTransfer::ItemInt> _current_mission;
    std::size_t _current_seq;

    std::weak_ptr<MavlinkMissionTransfer::WorkItem> _last_download{};

    void set_current_seq(std::size_t seq);

    void add_task(std::function<void()> task)
    {
        _work_queue.push(task);
        _wait_for_new_task.notify_one();
    }
};

} // namespace mavsdk
