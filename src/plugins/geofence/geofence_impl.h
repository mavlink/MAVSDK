#pragma once

#include <memory>
#include <map>
#include <atomic>

#include "mavlink_include.h"
#include "plugins/geofence/geofence.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class GeofenceImpl : public PluginImplBase {
public:
    GeofenceImpl(System& system);
    ~GeofenceImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void send_geofence_async(
        const std::vector<std::shared_ptr<Geofence::Polygon>>& polygons,
        const Geofence::result_callback_t& callback);

    // Non-copyable
    GeofenceImpl(const GeofenceImpl&) = delete;
    const GeofenceImpl& operator=(const GeofenceImpl&) = delete;

private:
    std::vector<MAVLinkMissionTransfer::ItemInt>
    assemble_items(const std::vector<std::shared_ptr<Geofence::Polygon>>& polygons);

    static Geofence::Result convert_result(MAVLinkMissionTransfer::Result result);
};

} // namespace mavsdk
