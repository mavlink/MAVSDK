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
    explicit GeofenceImpl(System& system);
    explicit GeofenceImpl(std::shared_ptr<System> system);
    ~GeofenceImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Geofence::Result upload_geofence(const Geofence::GeofenceData& geofence_data);

    void upload_geofence_async(
        const Geofence::GeofenceData& geofence_data, const Geofence::ResultCallback& callback);

    Geofence::Result clear_geofence();

    void clear_geofence_async(const Geofence::ResultCallback& callback);

    // Non-copyable
    GeofenceImpl(const GeofenceImpl&) = delete;
    const GeofenceImpl& operator=(const GeofenceImpl&) = delete;

private:
    std::vector<MavlinkMissionTransferClient::ItemInt>
    assemble_items(const Geofence::GeofenceData& geofence_data);

    static Geofence::Result convert_result(MavlinkMissionTransferClient::Result result);
};

} // namespace mavsdk
