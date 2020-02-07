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

void GeofenceImpl::send_geofence_async(
    const std::vector<std::shared_ptr<Geofence::Polygon>>& polygons,
    const Geofence::result_callback_t& callback)
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
GeofenceImpl::assemble_items(const std::vector<std::shared_ptr<Geofence::Polygon>>& polygons)
{
    std::vector<MAVLinkMissionTransfer::ItemInt> items;

    uint16_t sequence = 0;
    for (auto& polygon : polygons) {
        uint16_t command;
        switch (polygon->type) {
            case Geofence::Polygon::Type::INCLUSION:
                command = MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION;
                break;
            case Geofence::Polygon::Type::EXCLUSION:
                command = MAV_CMD_NAV_FENCE_POLYGON_VERTEX_EXCLUSION;
                break;
            default:
                LogErr() << "Unknown type";
                continue;
        }

        for (auto& point : polygon->points) {
            // FIXME: check if these two  make sense.
            const uint8_t current = (sequence == 0 ? 1 : 0);
            const uint8_t autocontinue = 0;
            const float param1 = float(polygon->points.size());

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
            return Geofence::Result::SUCCESS;
        case MAVLinkMissionTransfer::Result::ConnectionError:
            return Geofence::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::Denied:
            return Geofence::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::TooManyMissionItems:
            return Geofence::Result::TOO_MANY_GEOFENCE_ITEMS;
        case MAVLinkMissionTransfer::Result::Timeout:
            return Geofence::Result::TIMEOUT;
        case MAVLinkMissionTransfer::Result::Unsupported:
            return Geofence::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::UnsupportedFrame:
            return Geofence::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::NoMissionAvailable:
            return Geofence::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::Cancelled:
            return Geofence::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::MissionTypeNotConsistent:
            return Geofence::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidSequence:
            return Geofence::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::CurrentInvalid:
            return Geofence::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::ProtocolError:
            return Geofence::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidParam:
            return Geofence::Result::INVALID_ARGUMENT; // FIXME
        default:
            return Geofence::Result::UNKNOWN;
    }
}

} // namespace mavsdk
