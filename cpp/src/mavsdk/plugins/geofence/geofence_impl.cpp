#include "geofence_impl.hpp"
#include "log.hpp"
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

std::pair<Geofence::Result, Geofence::GeofenceData> GeofenceImpl::download_geofence()
{
    auto prom = std::promise<std::pair<Geofence::Result, Geofence::GeofenceData>>();
    auto fut = prom.get_future();

    download_geofence_async([&prom](Geofence::Result result, Geofence::GeofenceData geofence_data) {
        prom.set_value(std::make_pair(result, geofence_data));
    });
    return fut.get();
}

void GeofenceImpl::download_geofence_async(const Geofence::DownloadGeofenceCallback& callback)
{
    if (_last_download.lock()) {
        _system_impl->call_user_callback([callback]() {
            if (callback) {
                callback(Geofence::Result::Busy, Geofence::GeofenceData{});
            }
        });
        return;
    }

    _last_download = _system_impl->mission_transfer_client().download_items_async(
        MAV_MISSION_TYPE_FENCE,
        _system_impl->get_system_id(),
        [this, callback](
            MavlinkMissionTransferClient::Result result,
            std::vector<MavlinkMissionTransferClient::ItemInt> items) {
            auto result_pair = (result == MavlinkMissionTransferClient::Result::Success) ?
                                   disassemble_items(items) :
                                   std::make_pair(convert_result(result), Geofence::GeofenceData{});
            _system_impl->call_user_callback([callback, result_pair]() {
                if (callback) {
                    callback(result_pair.first, result_pair.second);
                }
            });
        });
}

std::pair<Geofence::Result, Geofence::GeofenceData>
GeofenceImpl::disassemble_items(const std::vector<MavlinkMissionTransferClient::ItemInt>& items)
{
    Geofence::GeofenceData geofence_data;

    size_t i = 0;
    while (i < items.size()) {
        const auto& item = items[i];

        if (item.command == MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION ||
            item.command == MAV_CMD_NAV_FENCE_POLYGON_VERTEX_EXCLUSION) {
            Geofence::Polygon polygon;
            polygon.fence_type = (item.command == MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION) ?
                                     Geofence::FenceType::Inclusion :
                                     Geofence::FenceType::Exclusion;

            const auto vertex_count = static_cast<size_t>(item.param1);
            if (vertex_count == 0 || i + vertex_count > items.size()) {
                LogErr("Invalid polygon vertex count: {}", vertex_count);
                return {Geofence::Result::InvalidArgument, {}};
            }

            for (size_t j = 0; j < vertex_count; ++j) {
                const auto& vertex_item = items[i + j];
                Geofence::Point point;
                point.latitude_deg = vertex_item.x * 1e-7;
                point.longitude_deg = vertex_item.y * 1e-7;
                polygon.points.push_back(point);
            }

            geofence_data.polygons.push_back(polygon);
            i += vertex_count;

        } else if (
            item.command == MAV_CMD_NAV_FENCE_CIRCLE_INCLUSION ||
            item.command == MAV_CMD_NAV_FENCE_CIRCLE_EXCLUSION) {
            Geofence::Circle circle;
            circle.fence_type = (item.command == MAV_CMD_NAV_FENCE_CIRCLE_INCLUSION) ?
                                    Geofence::FenceType::Inclusion :
                                    Geofence::FenceType::Exclusion;
            circle.point.latitude_deg = item.x * 1e-7;
            circle.point.longitude_deg = item.y * 1e-7;
            circle.radius = item.param1;

            geofence_data.circles.push_back(circle);
            ++i;

        } else {
            LogWarn("Unknown geofence item command: {}", item.command);
            ++i;
        }
    }

    return {Geofence::Result::Success, geofence_data};
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
