#include "geofence_impl.h"
#include "global_include.h"
#include "log.h"

namespace dronecode_sdk {

GeofenceImpl::GeofenceImpl(System &system) :
    PluginImplBase(system),
    _mavlink_geofence_item_messages()
{
    _parent->register_plugin(this);
}

GeofenceImpl::~GeofenceImpl()
{
    _parent->unregister_plugin(this);
}

void GeofenceImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_REQUEST_INT,
        std::bind(&GeofenceImpl::process_mission_request_int, this, _1),
        (void *)this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ACK,
        std::bind(&GeofenceImpl::process_mission_ack, this, _1),
        (void *)this);
}

void GeofenceImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void GeofenceImpl::enable() {}

void GeofenceImpl::disable() {}

void GeofenceImpl::send_geofence_async(
    const std::vector<std::shared_ptr<Geofence::Polygon>> &polygons,
    const Geofence::result_callback_t &callback)
{
    if (_active) {
        report_geofence_result(callback, Geofence::Result::BUSY);
        return;
    }

    if (!_parent->does_support_mission_int()) {
        LogDebug() << "Mission int messages not supported";
        report_geofence_result(callback, Geofence::Result::ERROR);
        return;
    }

    _active = true;

    assemble_mavlink_messages(polygons);

    LogDebug() << "size afterwards is: " << _mavlink_geofence_item_messages.size();

    mavlink_message_t message;
    mavlink_msg_mission_count_pack(_parent->get_own_system_id(),
                                   _parent->get_own_component_id(),
                                   &message,
                                   _parent->get_system_id(),
                                   _parent->get_autopilot_id(),
                                   _mavlink_geofence_item_messages.size(),
                                   MAV_MISSION_TYPE_FENCE);

    LogDebug() << "About to send " << _mavlink_geofence_item_messages.size() << " geofence items";

    if (!_parent->send_message(message)) {
        report_geofence_result(callback, Geofence::Result::ERROR);
    }

    _result_callback = callback;

    _parent->register_timeout_handler(
        std::bind(&GeofenceImpl::timeout_happened, this), 2.0, &_timeout_cookie);
}

void GeofenceImpl::process_mission_request_int(const mavlink_message_t &message)
{
    if (!_active) {
        LogDebug() << "Ignore geofence request, currently inactive";
        return;
    }

    mavlink_mission_request_int_t mission_request_int;
    mavlink_msg_mission_request_int_decode(&message, &mission_request_int);

    if (mission_request_int.target_system != _parent->get_own_system_id() &&
        mission_request_int.target_component != _parent->get_own_component_id()) {
        LogDebug() << "Ignore geofence request int that is not for us";
        return;
    }

    if (mission_request_int.mission_type != MAV_MISSION_TYPE_FENCE) {
        LogDebug() << "Ignore mission request that is not a geofence";
        return;
    }

    send_geofence_item(mission_request_int.seq);

    // Reset the timeout because we're still communicating.
    _parent->refresh_timeout_handler(this);
}

void GeofenceImpl::process_mission_ack(const mavlink_message_t &message)
{
    if (!_active) {
        LogDebug() << "Ignore mission ack because not active";
        return;
    }

    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    if (mission_ack.target_system != _parent->get_own_system_id() &&
        mission_ack.target_component != _parent->get_own_component_id()) {
        LogDebug() << "Ignore geofence ack that is not for us";
        return;
    }

    // We got some response, so it wasn't a timeout and we can remove it.
    _parent->unregister_timeout_handler(this);

    if (mission_ack.type == MAV_MISSION_ACCEPTED) {
        report_geofence_result(_result_callback, Geofence::Result::SUCCESS);
        LogDebug() << "Sucess, done";

    } else if (mission_ack.type == MAV_MISSION_NO_SPACE) {
        LogDebug() << "Error: too many geofence items: " << int(mission_ack.type);
        report_geofence_result(_result_callback, Geofence::Result::TOO_MANY_GEOFENCE_ITEMS);
    } else {
        LogDebug() << "Error: unknown geofence ack: " << int(mission_ack.type);
        report_geofence_result(_result_callback, Geofence::Result::ERROR);
    }
}

void GeofenceImpl::assemble_mavlink_messages(
    const std::vector<std::shared_ptr<Geofence::Polygon>> &polygons)
{
    // TODO: delete all entries first
    _mavlink_geofence_item_messages.clear();

    for (auto &polygon : polygons) {
        u_int16_t command;
        switch (polygon->type) {
            case Geofence::Polygon::Type::INCLUSION:
                command = MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION;
                break;
            case Geofence::Polygon::Type::EXCLUSION:
                command = MAV_CMD_NAV_FENCE_POLYGON_VERTEX_EXCLUSION;
                break;
            default:
                LogDebug() << "Unknown type";
                return;
        }

        for (auto &point : polygon->points) {
            std::shared_ptr<mavlink_message_t> message(new mavlink_message_t());
            mavlink_msg_mission_item_int_pack(_parent->get_own_system_id(),
                                              _parent->get_own_component_id(),
                                              message.get(),
                                              _parent->get_system_id(),
                                              _parent->get_autopilot_id(),
                                              _mavlink_geofence_item_messages.size(),
                                              MAV_FRAME_GLOBAL_INT,
                                              command,
                                              0, // current
                                              0, // autocontinue
                                              polygon->points.size(), // vertex count
                                              0.0f,
                                              0.0f,
                                              0.0f,
                                              point.latitude_deg * 1e7,
                                              point.longitude_deg * 1e7,
                                              0.0f,
                                              MAV_MISSION_TYPE_FENCE);
            _mavlink_geofence_item_messages.push_back(message);
        }
    }
}

void GeofenceImpl::timeout_happened()
{
    LogDebug() << "timeout happened";
    _active = false;

    report_geofence_result(_result_callback, Geofence::Result::TIMEOUT);
}

void GeofenceImpl::send_geofence_item(uint16_t seq)
{
    LogDebug() << "Send geofence item " << int(seq);
    if (seq >= _mavlink_geofence_item_messages.size()) {
        LogDebug() << "Geofence item requested out of bounds.";
        return;
    }

    _parent->send_message(*_mavlink_geofence_item_messages.at(seq));
}

void GeofenceImpl::report_geofence_result(const Geofence::result_callback_t &callback,
                                          Geofence::Result result)
{
    if (callback == nullptr) {
        LogDebug() << "Callback is not set";
        return;
    }

    callback(result);
}

} // namespace dronecode_sdk
