#include "geofence_impl.h"
#include "log.h"
#include <cmath>

namespace mavsdk {

GeofenceImpl::GeofenceImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

GeofenceImpl::GeofenceImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

GeofenceImpl::~GeofenceImpl()
{
    _system_impl->unregister_plugin(this);
}

void GeofenceImpl::init() {}

void GeofenceImpl::deinit() {}

void GeofenceImpl::enable() {}

void GeofenceImpl::disable() {}

Geofence::Result GeofenceImpl::upload_geofence(const Geofence::GeofenceData& geofence_data)
{
    auto prom = std::promise<Geofence::Result>();
    auto fut = prom.get_future();

    upload_geofence_async(
        geofence_data, [&prom](Geofence::Result result) { prom.set_value(result); });
    return fut.get();
}

void GeofenceImpl::upload_geofence_async(
    const Geofence::GeofenceData& geofence_data, const Geofence::ResultCallback& callback)
{
    // We can just create these items on the stack because they get copied
    // later in the MavlinkMissionTransferClient constructor.
    const auto items = assemble_items(geofence_data);

    _system_impl->mission_transfer_client().upload_items_async(
        MAV_MISSION_TYPE_FENCE,
        _system_impl->get_system_id(),
        items,
        [this, callback](MavlinkMissionTransferClient::Result result) {
            auto converted_result = convert_result(result);
            _system_impl->get_system_id(),
                _system_impl->call_user_callback(
                    [callback, converted_result]() { callback(converted_result); });
        });
}

Geofence::Result GeofenceImpl::clear_geofence()
{
    auto prom = std::promise<Geofence::Result>();
    auto fut = prom.get_future();

    clear_geofence_async([&prom](Geofence::Result result) { prom.set_value(result); });
    return fut.get();
}

void GeofenceImpl::clear_geofence_async(const Geofence::ResultCallback& callback)
{
    _system_impl->mission_transfer_client().clear_items_async(
        MAV_MISSION_TYPE_FENCE,
        _system_impl->get_system_id(),
        [this, callback](MavlinkMissionTransferClient::Result result) {
            auto converted_result = convert_result(result);
            _system_impl->call_user_callback([callback, converted_result]() {
                if (callback) {
                    callback(converted_result);
                }
            });
        });
}

std::vector<MavlinkMissionTransferClient::ItemInt>
GeofenceImpl::assemble_items(const Geofence::GeofenceData& geofence_data)
{
    std::vector<MavlinkMissionTransferClient::ItemInt> items;

    uint16_t sequence = 0;

    for (auto& polygon : geofence_data.polygons) {
        const uint16_t command =
            (polygon.fence_type == Geofence::FenceType::Inclusion ?
                 MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION :
                 MAV_CMD_NAV_FENCE_POLYGON_VERTEX_EXCLUSION);

        for (auto& point : polygon.points) {
            const uint8_t current = (sequence == 0 ? 1 : 0);
            const uint8_t autocontinue = 0;
            const float param1 = float(polygon.points.size());

            items.push_back(MavlinkMissionTransferClient::ItemInt{
                sequence,
                MAV_FRAME_GLOBAL_INT,
                command,
                current,
                autocontinue,
                param1,
                0.0f,
                0.0f,
                0.0f,
                int32_t(std::round(point.latitude_deg * 1e7)),
                int32_t(std::round(point.longitude_deg * 1e7)),
                0.0f,
                MAV_MISSION_TYPE_FENCE});
            ++sequence;
        }
    }

    for (auto& circle : geofence_data.circles) {
        const uint8_t current = (sequence == 0 ? 1 : 0);
        const uint8_t autocontinue = 0;
        const uint16_t command =
            (circle.fence_type == Geofence::FenceType::Inclusion ?
                 MAV_CMD_NAV_FENCE_CIRCLE_INCLUSION :
                 MAV_CMD_NAV_FENCE_CIRCLE_EXCLUSION);

        items.push_back(MavlinkMissionTransferClient::ItemInt{
            sequence,
            MAV_FRAME_GLOBAL_INT,
            command,
            current,
            autocontinue,
            circle.radius,
            0.0f,
            0.0f,
            0.0f,
            int32_t(std::round(circle.point.latitude_deg * 1e7)),
            int32_t(std::round(circle.point.longitude_deg * 1e7)),
            0.0f,
            MAV_MISSION_TYPE_FENCE});
        ++sequence;
    }

    return items;
}

Geofence::Result GeofenceImpl::convert_result(MavlinkMissionTransferClient::Result result)
{
    switch (result) {
        case MavlinkMissionTransferClient::Result::Success:
            return Geofence::Result::Success;
        case MavlinkMissionTransferClient::Result::ConnectionError:
            return Geofence::Result::Error; // FIXME
        case MavlinkMissionTransferClient::Result::Denied:
            return Geofence::Result::Error; // FIXME
        case MavlinkMissionTransferClient::Result::TooManyMissionItems:
            return Geofence::Result::TooManyGeofenceItems;
        case MavlinkMissionTransferClient::Result::Timeout:
            return Geofence::Result::Timeout;
        case MavlinkMissionTransferClient::Result::Unsupported:
            return Geofence::Result::Error; // FIXME
        case MavlinkMissionTransferClient::Result::UnsupportedFrame:
            return Geofence::Result::Error; // FIXME
        case MavlinkMissionTransferClient::Result::NoMissionAvailable:
            return Geofence::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferClient::Result::Cancelled:
            return Geofence::Result::Error; // FIXME
        case MavlinkMissionTransferClient::Result::MissionTypeNotConsistent:
            return Geofence::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferClient::Result::InvalidSequence:
            return Geofence::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferClient::Result::CurrentInvalid:
            return Geofence::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferClient::Result::ProtocolError:
            return Geofence::Result::Error; // FIXME
        case MavlinkMissionTransferClient::Result::InvalidParam:
            return Geofence::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferClient::Result::IntMessagesNotSupported:
            return Geofence::Result::Error; // FIXME
        default:
            return Geofence::Result::Unknown;
    }
}

} // namespace mavsdk
