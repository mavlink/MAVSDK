#include "geofence_impl.h"
#include "global_include.h"
#include "log.h"
#include <cmath>

namespace mavsdk {

GeofenceImpl::GeofenceImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

GeofenceImpl::~GeofenceImpl()
{
    _parent->unregister_plugin(this);
}

void GeofenceImpl::init() {}

void GeofenceImpl::deinit() {}

void GeofenceImpl::enable() {}

void GeofenceImpl::disable() {}

Geofence::Result GeofenceImpl::upload_geofence(const std::vector<Geofence::Polygon>& polygons)
{
    auto prom = std::promise<Geofence::Result>();
    auto fut = prom.get_future();

    upload_geofence_async(polygons, [&prom](Geofence::Result result) { prom.set_value(result); });
    return fut.get();
}

void GeofenceImpl::upload_geofence_async(
    const std::vector<Geofence::Polygon>& polygons, const Geofence::result_callback_t& callback)
{
    // We can just create these items on the stack because they get copied
    // later in the MAVLinkMissionTransfer constructor.
    const auto items = assemble_items(polygons);

    _parent->mission_transfer().upload_items_async(
        MAV_MISSION_TYPE_FENCE, items, [this, callback](MAVLinkMissionTransfer::Result result) {
            auto converted_result = convert_result(result);
            _parent->call_user_callback(
                [callback, converted_result]() { callback(converted_result); });
        });
}

std::vector<MAVLinkMissionTransfer::ItemInt>
GeofenceImpl::assemble_items(const std::vector<Geofence::Polygon>& polygons)
{
    std::vector<MAVLinkMissionTransfer::ItemInt> items;

    uint16_t sequence = 0;
    for (auto& polygon : polygons) {
        uint16_t command;
        switch (polygon.type) {
            case Geofence::Polygon::Type::Inclusion:
                command = MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION;
                break;
            case Geofence::Polygon::Type::Exclusion:
                command = MAV_CMD_NAV_FENCE_POLYGON_VERTEX_EXCLUSION;
                break;
            default:
                LogErr() << "Unknown type";
                continue;
        }

        for (auto& point : polygon.points) {
            // FIXME: check if these two  make sense.
            const uint8_t current = (sequence == 0 ? 1 : 0);
            const uint8_t autocontinue = 0;
            const float param1 = float(polygon.points.size());

            items.push_back(
                MAVLinkMissionTransfer::ItemInt{sequence,
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
    return items;
}

Geofence::Result GeofenceImpl::convert_result(MAVLinkMissionTransfer::Result result)
{
    switch (result) {
        case MAVLinkMissionTransfer::Result::Success:
            return Geofence::Result::Success;
        case MAVLinkMissionTransfer::Result::ConnectionError:
            return Geofence::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::Denied:
            return Geofence::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::TooManyMissionItems:
            return Geofence::Result::TooManyGeofenceItems;
        case MAVLinkMissionTransfer::Result::Timeout:
            return Geofence::Result::Timeout;
        case MAVLinkMissionTransfer::Result::Unsupported:
            return Geofence::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::UnsupportedFrame:
            return Geofence::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::NoMissionAvailable:
            return Geofence::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::Cancelled:
            return Geofence::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::MissionTypeNotConsistent:
            return Geofence::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidSequence:
            return Geofence::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::CurrentInvalid:
            return Geofence::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::ProtocolError:
            return Geofence::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidParam:
            return Geofence::Result::InvalidArgument; // FIXME
        default:
            return Geofence::Result::Unknown;
    }
}

} // namespace mavsdk
