#pragma once

#include "system.h"
#include "plugins/geofence/geofence.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <memory>
#include <map>
#include <atomic>

namespace dronecode_sdk {

class GeofenceImpl : public PluginImplBase {
public:
    GeofenceImpl(System &system);
    ~GeofenceImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void send_geofence_async(const std::vector<std::shared_ptr<Geofence::Polygon>> &polygons,
                             const Geofence::result_callback_t &callback);

    // Non-copyable
    GeofenceImpl(const GeofenceImpl &) = delete;
    const GeofenceImpl &operator=(const GeofenceImpl &) = delete;

private:
    void assemble_mavlink_messages(const std::vector<std::shared_ptr<Geofence::Polygon>> &polygons);

    void timeout_happened();

    void process_mission_request_int(const mavlink_message_t &message);
    void process_mission_ack(const mavlink_message_t &message);
    void process_geofence_violation(const mavlink_message_t &message);
    void send_geofence_item(uint16_t seq);

    static void report_geofence_result(const Geofence::result_callback_t &callback,
                                       Geofence::Result result);

    void receive_command_result(MAVLinkCommands::Result command_result,
                                const Geofence::result_callback_t &callback);

    Geofence::result_callback_t _result_callback = nullptr;

    std::vector<std::shared_ptr<mavlink_message_t>> _mavlink_geofence_item_messages;

    void *_timeout_cookie{nullptr};
    std::atomic<bool> _active{false};
};

} // namespace dronecode_sdk
