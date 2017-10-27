#pragma once

#include "follow_me.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "device_impl.h"
#include "timeout_handler.h"

namespace dronecore {

class FollowMeImpl : public PluginImplBase
{
public:
    FollowMeImpl();
    ~FollowMeImpl();

    void init() override;
    void deinit() override;

    FollowMe::Result start();
    FollowMe::Result stop() const;
    FollowMe::Result start(const FollowMe::MotionReport &mr);
    void set_motion_report(const FollowMe::MotionReport &mr);

private:
    void process_heartbeat(const mavlink_message_t &message);
    FollowMe::Result get_result_from_mavcmd_result(MavlinkCommands::Result result) const;
    void timeout_occurred();
    void send_gcs_motion_report();

    // required for emitting MotionReport updates to Vehicle
    FollowMe::MotionReport _motion_report;

    TimeoutHandler _motion_report_timer;
    void *_timeout_cookie = nullptr;
    std::chrono::system_clock::time_point _start;
    uint8_t _estimatation_capabilities;

    bool _followme_mode_active;
};

} // namespace dronecore
