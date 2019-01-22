#include "mission_raw_impl.h"
#include "system.h"
#include "global_include.h"
#include <fstream> // for `std::ifstream`
#include <sstream> // for `std::stringstream`
#include <cmath>

namespace dronecode_sdk {

using namespace std::placeholders; // for `_1`

MissionRawImpl::MissionRawImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MissionRawImpl::~MissionRawImpl()
{
    _parent->unregister_plugin(this);
}

void MissionRawImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ACK,
        std::bind(&MissionRawImpl::process_mission_ack, this, _1),
        this);
}

void MissionRawImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MissionRawImpl::enable() {}

void MissionRawImpl::disable() {}

void MissionRawImpl::process_mission_ack(const mavlink_message_t &message)
{
    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    if (mission_ack.type != MAV_MISSION_ACCEPTED) {
        return;
    }

    // We assume that if the vehicle sends an ACCEPTED ack might have received
    // a new mission. In that case we need to notify our user.
    std::lock_guard<std::mutex> lock(_mission_changed.mutex);
    if (_mission_changed.callback) {
        // Local copy because we can't make a copy of member variable.
        auto callback = _mission_changed.callback;
        _parent->call_user_callback([callback]() { callback(); });
    }
}

void MissionRawImpl::download_mission_async(
    const MissionRaw::mission_items_and_result_callback_t &callback)
{
    UNUSED(callback);
}

void MissionRawImpl::download_mission_cancel() {}

void MissionRawImpl::subscribe_mission_changed(MissionRaw::mission_changed_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_mission_changed.mutex);
    _mission_changed.callback = callback;
}

} // namespace dronecode_sdk
