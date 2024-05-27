#include <cstring>
#include <functional>
#include <future>
#include <numeric>
#include "info_impl.h"
#include "system.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<Info::FlightInfo>;

InfoImpl::InfoImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

InfoImpl::InfoImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

InfoImpl::~InfoImpl()
{
    _system_impl->unregister_plugin(this);
}

void InfoImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        [this](const mavlink_message_t& message) { process_autopilot_version(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_FLIGHT_INFORMATION,
        [this](const mavlink_message_t& message) { process_flight_information(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ATTITUDE,
        [this](const mavlink_message_t& message) { process_attitude(message); },
        this);
}

void InfoImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers(this);
}

void InfoImpl::enable()
{
    // We're going to retry until we have the version.
    _call_every_cookie = _system_impl->add_call_every(
        [this]() { _system_impl->send_autopilot_version_request(); }, 2.0f);

    if (!_flight_info_subscriptions.empty()) {
        // We're hoping to get flight information regularly to update flight time.
        _system_impl->set_msg_rate_async(MAVLINK_MSG_ID_FLIGHT_INFORMATION, 1.0, nullptr);
    }
}

void InfoImpl::disable()
{
    _system_impl->remove_call_every(_call_every_cookie);

    std::lock_guard<std::mutex> lock(_mutex);
    _identification_received = false;
}

void InfoImpl::process_autopilot_version(const mavlink_message_t& message)
{
    _system_impl->remove_call_every(_call_every_cookie);

    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    _version.flight_sw_major = (autopilot_version.flight_sw_version >> (8 * 3)) & 0xFF;
    _version.flight_sw_minor = (autopilot_version.flight_sw_version >> (8 * 2)) & 0xFF;
    _version.flight_sw_patch = (autopilot_version.flight_sw_version >> (8 * 1)) & 0xFF;
    _version.flight_sw_version_type =
        get_flight_software_version_type(static_cast<FIRMWARE_VERSION_TYPE>(
            (autopilot_version.flight_sw_version >> (8 * 0)) & 0xFF));

    // first three bytes of flight_custom_version (little endian) describe vendor version
    _version.flight_sw_git_hash = swap_and_translate_binary_to_str(
        autopilot_version.flight_custom_version + 3,
        sizeof(autopilot_version.flight_custom_version) - 3);

    _version.flight_sw_vendor_major = autopilot_version.flight_custom_version[2];
    _version.flight_sw_vendor_minor = autopilot_version.flight_custom_version[1];
    _version.flight_sw_vendor_patch = autopilot_version.flight_custom_version[0];

    _version.os_sw_major = (autopilot_version.os_sw_version >> (8 * 3)) & 0xFF;
    _version.os_sw_minor = (autopilot_version.os_sw_version >> (8 * 2)) & 0xFF;
    _version.os_sw_patch = (autopilot_version.os_sw_version >> (8 * 1)) & 0xFF;

    _version.os_sw_git_hash = swap_and_translate_binary_to_str(
        autopilot_version.os_custom_version, sizeof(autopilot_version.os_custom_version));

    _product.vendor_id = autopilot_version.vendor_id;
    _product.vendor_name = vendor_id_str(autopilot_version.vendor_id);

    _product.product_id = autopilot_version.product_id;
    _product.product_name = product_id_str(autopilot_version.product_id);

    _identification.hardware_uid =
        translate_binary_to_str(autopilot_version.uid2, sizeof(autopilot_version.uid2));

    _identification.legacy_uid = autopilot_version.uid;

    _identification_received = true;
}

Info::Version::FlightSoftwareVersionType
InfoImpl::get_flight_software_version_type(FIRMWARE_VERSION_TYPE firmwareVersionType)
{
    switch (firmwareVersionType) {
        case FIRMWARE_VERSION_TYPE_DEV:
            return Info::Version::FlightSoftwareVersionType::Dev;

        case FIRMWARE_VERSION_TYPE_ALPHA:
            return Info::Version::FlightSoftwareVersionType::Alpha;

        case FIRMWARE_VERSION_TYPE_BETA:
            return Info::Version::FlightSoftwareVersionType::Beta;

        case FIRMWARE_VERSION_TYPE_RC:
            return Info::Version::FlightSoftwareVersionType::Rc;

        case FIRMWARE_VERSION_TYPE_OFFICIAL:
            return Info::Version::FlightSoftwareVersionType::Release;

        default:
            return Info::Version::FlightSoftwareVersionType::Unknown;
    }
}

void InfoImpl::process_flight_information(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_flight_information_t flight_information;
    mavlink_msg_flight_information_decode(&message, &flight_information);

    _flight_info.time_boot_ms = flight_information.time_boot_ms;
    _flight_info.flight_uid = flight_information.flight_uuid;
    // The fields are called UTC but are actually since boot
    const auto arming_time_ms = flight_information.arming_time_utc / 1000;
    const auto takeoff_time_ms = flight_information.takeoff_time_utc / 1000;

    if (arming_time_ms > 0 && arming_time_ms < flight_information.time_boot_ms) {
        _flight_info.duration_since_arming_ms = flight_information.time_boot_ms - arming_time_ms;
    } else {
        _flight_info.duration_since_arming_ms = 0;
    }

    if (takeoff_time_ms > 0 && takeoff_time_ms < flight_information.time_boot_ms) {
        _flight_info.duration_since_takeoff_ms = flight_information.time_boot_ms - takeoff_time_ms;
    } else {
        _flight_info.duration_since_takeoff_ms = 0;
    }

    _flight_info_subscriptions.queue(
        _flight_info, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

std::string InfoImpl::swap_and_translate_binary_to_str(uint8_t* binary, unsigned binary_len)
{
    std::string str(binary_len * 2, '0');

    for (unsigned i = 0; i < binary_len; ++i) {
        // One hex number occupies 2 chars.
        // The binary is in little endian, therefore we need to swap the bytes for us to read.
        snprintf(&str[i * 2], str.length() - i * 2, "%02x", binary[binary_len - 1 - i]);
    }

    return str;
}

std::string InfoImpl::translate_binary_to_str(uint8_t* binary, unsigned binary_len)
{
    std::string str(binary_len * 2 + 1, '0');

    for (unsigned i = 0; i < binary_len; ++i) {
        // One hex number occupies 2 chars.
        snprintf(&str[i * 2], str.length() - i * 2, "%02x", binary[i]);
    }

    return str;
}

std::pair<Info::Result, Info::Identification> InfoImpl::get_identification() const
{
    wait_for_identification();

    std::lock_guard<std::mutex> lock(_mutex);
    return std::make_pair<>(
        (_identification_received ? Info::Result::Success :
                                    Info::Result::InformationNotReceivedYet),
        _identification);
}

std::pair<Info::Result, Info::Version> InfoImpl::get_version() const
{
    wait_for_identification();

    std::lock_guard<std::mutex> lock(_mutex);

    return std::make_pair<>(
        (_identification_received ? Info::Result::Success :
                                    Info::Result::InformationNotReceivedYet),
        _version);
}

std::pair<Info::Result, Info::Product> InfoImpl::get_product() const
{
    wait_for_identification();
    std::lock_guard<std::mutex> lock(_mutex);

    return std::make_pair<>(
        (_identification_received ? Info::Result::Success :
                                    Info::Result::InformationNotReceivedYet),
        _product);
}

std::pair<Info::Result, Info::FlightInfo> InfoImpl::get_flight_information()
{
    std::promise<std::pair<Info::Result, Info::FlightInfo>> prom;
    auto fut = prom.get_future();
    _system_impl->request_message().request(
        MAVLINK_MSG_ID_FLIGHT_INFORMATION,
        MAV_COMP_ID_AUTOPILOT1,
        [&](MavlinkCommandSender::Result result, const mavlink_message_t& message) {
            if (result == MavlinkCommandSender::Result::Success) {
                // This call might happen twice but that's ok.
                process_flight_information(message);
                prom.set_value({Info::Result::Success, _flight_info});
            } else {
                prom.set_value({Info::Result::InformationNotReceivedYet, Info::FlightInfo{}});
            }
        });
    return fut.get();
}

const std::string InfoImpl::vendor_id_str(uint16_t vendor_id)
{
    switch (vendor_id) {
        case 0x26ac:
            return "3D Robotics Inc.";
        default:
            return "undefined";
    }
}

const std::string InfoImpl::product_id_str(uint16_t product_id)
{
    switch (product_id) {
        case 0x0010:
            return "H520";
        default:
            return "undefined";
    }
}

void InfoImpl::process_attitude(const mavlink_message_t& message)
{
    // We use the attitude message to estimate the lockstep speed factor
    // because it's common to be sent, arrives at high rate, and contains
    // the timestamp field.
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&message, &attitude);

    std::lock_guard<std::mutex> lock(_mutex);

    if (_last_time_boot_ms != 0) {
        _speed_factor_measurements.push(SpeedFactorMeasurement{
            static_cast<double>(attitude.time_boot_ms - _last_time_boot_ms) * 1e-3,
            _time.elapsed_since_s(_last_time_attitude_arrived)});
    }

    _last_time_boot_ms = attitude.time_boot_ms;
    _last_time_attitude_arrived = _time.steady_time();
}

std::pair<Info::Result, double> InfoImpl::get_speed_factor() const
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_speed_factor_measurements.size() == 0) {
        return std::make_pair<>(Info::Result::InformationNotReceivedYet, NAN);
    }

    const auto sum = std::accumulate(
        _speed_factor_measurements.begin(),
        _speed_factor_measurements.end(),
        SpeedFactorMeasurement{},
        [](SpeedFactorMeasurement a, SpeedFactorMeasurement b) { return a + b; });

    const double speed_factor = sum.simulated_duration_s / sum.real_time_s;

    return std::make_pair<>(Info::Result::Success, speed_factor);
}

void InfoImpl::wait_for_identification() const
{
    // Wait 0.5 seconds max
    for (unsigned i = 0; i < 50; ++i) {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_identification_received) {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

Info::FlightInformationHandle
InfoImpl::subscribe_flight_information(const Info::FlightInformationCallback& callback)
{
    // Make sure we get the message regularly.
    _system_impl->set_msg_rate_async(MAVLINK_MSG_ID_FLIGHT_INFORMATION, 1.0, nullptr);

    std::lock_guard<std::mutex> lock(_mutex);

    return _flight_info_subscriptions.subscribe(callback);
}

void InfoImpl::unsubscribe_flight_information(Info::FlightInformationHandle handle)
{
    // Reset message to default
    _system_impl->set_msg_rate_async(MAVLINK_MSG_ID_FLIGHT_INFORMATION, 0.0, nullptr);

    std::lock_guard<std::mutex> lock(_mutex);

    _flight_info_subscriptions.unsubscribe(handle);
}

} // namespace mavsdk
