#include "remote_id_impl.h"
#include "mavlink_address.h"
#include "callback_list.tpp"

#include <cstring>

namespace mavsdk {

RemoteIdImpl::RemoteIdImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

RemoteIdImpl::RemoteIdImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

RemoteIdImpl::~RemoteIdImpl()
{
    _system_impl->unregister_plugin(this);
}

void RemoteIdImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_OPEN_DRONE_ID_ARM_STATUS,
        [this](const mavlink_message_t& message) { process_arm_status(message); },
        this);
}

void RemoteIdImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers_blocking(this);
}

void RemoteIdImpl::enable() {}

void RemoteIdImpl::disable() {}

void RemoteIdImpl::process_arm_status(const mavlink_message_t& message)
{
    mavlink_open_drone_id_arm_status_t arm_status_msg;
    mavlink_msg_open_drone_id_arm_status_decode(&message, &arm_status_msg);

    RemoteId::ArmStatus new_status;
    new_status.status = (arm_status_msg.status == MAV_ODID_ARM_STATUS_GOOD_TO_ARM) ?
                            RemoteId::ArmStatus::Status::GoodToArm :
                            RemoteId::ArmStatus::Status::PreArmFailGeneric;
    new_status.error = std::string(
        arm_status_msg.error, strnlen(arm_status_msg.error, sizeof(arm_status_msg.error)));

    {
        std::lock_guard<std::mutex> lock(_arm_status_mutex);
        _arm_status = new_status;
    }

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _arm_status_subscriptions.queue(
        new_status, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

RemoteId::ArmStatusHandle
RemoteIdImpl::subscribe_arm_status(const RemoteId::ArmStatusCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    return _arm_status_subscriptions.subscribe(callback);
}

void RemoteIdImpl::unsubscribe_arm_status(RemoteId::ArmStatusHandle handle)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _arm_status_subscriptions.unsubscribe(handle);
}

RemoteId::ArmStatus RemoteIdImpl::arm_status() const
{
    std::lock_guard<std::mutex> lock(_arm_status_mutex);
    return _arm_status;
}

RemoteId::Result RemoteIdImpl::set_basic_id(RemoteId::BasicId basic_id)
{
    uint8_t id_type = static_cast<uint8_t>(basic_id.id_type);
    uint8_t ua_type = static_cast<uint8_t>(basic_id.ua_type);

    // Prepare uas_id (max 20 bytes, null-padded)
    uint8_t uas_id[20] = {};
    const size_t copy_len = std::min(basic_id.uas_id.size(), sizeof(uas_id));
    std::memcpy(uas_id, basic_id.uas_id.data(), copy_len);

    // id_or_mac is only used for received data, set to zero
    uint8_t id_or_mac[20] = {};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_open_drone_id_basic_id_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            _system_impl->get_autopilot_id(),
            id_or_mac,
            id_type,
            ua_type,
            uas_id);
        return message;
    }) ?
               RemoteId::Result::Success :
               RemoteId::Result::Error;
}

RemoteId::Result RemoteIdImpl::set_location(RemoteId::Location location)
{
    uint8_t status = static_cast<uint8_t>(location.status);
    uint8_t height_reference = static_cast<uint8_t>(location.height_reference);
    uint8_t horizontal_accuracy = static_cast<uint8_t>(location.horizontal_accuracy);
    uint8_t vertical_accuracy = static_cast<uint8_t>(location.vertical_accuracy);
    uint8_t barometer_accuracy = static_cast<uint8_t>(location.barometer_accuracy);
    uint8_t speed_accuracy = static_cast<uint8_t>(location.speed_accuracy);
    uint8_t timestamp_accuracy = static_cast<uint8_t>(location.timestamp_accuracy);

    // Convert units
    int32_t latitude = static_cast<int32_t>(location.latitude_deg * 1e7);
    int32_t longitude = static_cast<int32_t>(location.longitude_deg * 1e7);
    uint16_t direction = static_cast<uint16_t>(location.direction_deg * 100);
    uint16_t speed_horizontal = static_cast<uint16_t>(location.speed_horizontal_m_s * 100.0f);
    int16_t speed_vertical = static_cast<int16_t>(location.speed_vertical_m_s * 100.0f);

    // Convert timestamp from UTC microseconds to seconds after the full hour
    // timestamp = ((time_week_ms % (60*60*1000))) / 1000
    uint64_t time_us = location.time_utc_us;
    uint64_t time_ms = time_us / 1000;
    float timestamp = static_cast<float>((time_ms % (60ULL * 60ULL * 1000ULL))) / 1000.0f;

    // id_or_mac is only used for received data, set to zero
    uint8_t id_or_mac[20] = {};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_open_drone_id_location_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            _system_impl->get_autopilot_id(),
            id_or_mac,
            status,
            direction,
            speed_horizontal,
            speed_vertical,
            latitude,
            longitude,
            location.altitude_barometric_m,
            location.altitude_geodetic_m,
            height_reference,
            location.height_m,
            horizontal_accuracy,
            vertical_accuracy,
            barometer_accuracy,
            speed_accuracy,
            timestamp,
            timestamp_accuracy);
        return message;
    }) ?
               RemoteId::Result::Success :
               RemoteId::Result::Error;
}

RemoteId::Result RemoteIdImpl::set_system(RemoteId::SystemId system)
{
    uint8_t operator_location_type = static_cast<uint8_t>(system.operator_location_type);
    uint8_t classification_type = static_cast<uint8_t>(system.classification_type);
    uint8_t category_eu = static_cast<uint8_t>(system.category_eu);
    uint8_t class_eu = static_cast<uint8_t>(system.class_eu);

    // Convert units
    int32_t operator_latitude = static_cast<int32_t>(system.operator_latitude_deg * 1e7);
    int32_t operator_longitude = static_cast<int32_t>(system.operator_longitude_deg * 1e7);
    uint16_t area_count = static_cast<uint16_t>(system.area_count);
    uint16_t area_radius = static_cast<uint16_t>(system.area_radius_m);

    // Convert timestamp from UTC microseconds to Unix seconds since 2019-01-01
    // Unix epoch for 2019-01-01 00:00:00 UTC is 1546300800
    constexpr uint64_t EPOCH_2019 = 1546300800ULL;
    uint64_t time_us = system.time_utc_us;
    uint64_t unix_seconds = time_us / 1000000ULL;
    uint32_t timestamp = 0;
    if (unix_seconds >= EPOCH_2019) {
        timestamp = static_cast<uint32_t>(unix_seconds - EPOCH_2019);
    }

    // id_or_mac is only used for received data, set to zero
    uint8_t id_or_mac[20] = {};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_open_drone_id_system_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            _system_impl->get_autopilot_id(),
            id_or_mac,
            operator_location_type,
            classification_type,
            operator_latitude,
            operator_longitude,
            area_count,
            area_radius,
            system.area_ceiling_m,
            system.area_floor_m,
            category_eu,
            class_eu,
            system.operator_altitude_geo_m,
            timestamp);
        return message;
    }) ?
               RemoteId::Result::Success :
               RemoteId::Result::Error;
}

RemoteId::Result RemoteIdImpl::set_operator_id(RemoteId::OperatorId operator_id)
{
    uint8_t operator_id_type = static_cast<uint8_t>(operator_id.operator_id_type);

    // Prepare operator_id string (max 20 bytes, null-padded)
    char operator_id_str[20] = {};
    const size_t copy_len = std::min(operator_id.operator_id.size(), sizeof(operator_id_str));
    std::memcpy(operator_id_str, operator_id.operator_id.data(), copy_len);

    // id_or_mac is only used for received data, set to zero
    uint8_t id_or_mac[20] = {};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_open_drone_id_operator_id_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            _system_impl->get_autopilot_id(),
            id_or_mac,
            operator_id_type,
            operator_id_str);
        return message;
    }) ?
               RemoteId::Result::Success :
               RemoteId::Result::Error;
}

RemoteId::Result RemoteIdImpl::set_self_id(RemoteId::SelfId self_id)
{
    uint8_t description_type = static_cast<uint8_t>(self_id.description_type);

    // Prepare description string (max 23 bytes, null-padded)
    char description[23] = {};
    const size_t copy_len = std::min(self_id.description.size(), sizeof(description));
    std::memcpy(description, self_id.description.data(), copy_len);

    // id_or_mac is only used for received data, set to zero
    uint8_t id_or_mac[20] = {};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_open_drone_id_self_id_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            _system_impl->get_autopilot_id(),
            id_or_mac,
            description_type,
            description);
        return message;
    }) ?
               RemoteId::Result::Success :
               RemoteId::Result::Error;
}

} // namespace mavsdk
