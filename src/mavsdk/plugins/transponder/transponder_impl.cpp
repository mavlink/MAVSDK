#include "transponder_impl.h"

namespace mavsdk {

TransponderImpl::TransponderImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

TransponderImpl::TransponderImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _parent->register_plugin(this);
}

TransponderImpl::~TransponderImpl()
{
    _parent->unregister_plugin(this);
}

void TransponderImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ADSB_VEHICLE,
        [this](const mavlink_message_t& message) { process_transponder(message); },
        this);
}

void TransponderImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void TransponderImpl::enable() {}

void TransponderImpl::disable() {}

Transponder::Result TransponderImpl::set_rate_transponder(double rate_hz)
{
    return transponder_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_ADSB_VEHICLE, rate_hz));
}

void TransponderImpl::set_rate_transponder_async(
    double rate_hz, const Transponder::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_ADSB_VEHICLE,
        rate_hz,
        [callback](MavlinkCommandSender::Result command_result, float) {
            command_result_callback(command_result, callback);
        });
}

Transponder::AdsbVehicle TransponderImpl::transponder() const
{
    std::lock_guard<std::mutex> lock(_transponder_mutex);
    return _transponder;
}

void TransponderImpl::subscribe_transponder(Transponder::TransponderCallback callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _transponder_subscription = callback;
}

void TransponderImpl::set_transponder(Transponder::AdsbVehicle transponder)
{
    std::lock_guard<std::mutex> lock(_transponder_mutex);
    _transponder = transponder;
}

void TransponderImpl::process_transponder(const mavlink_message_t& message)
{
    mavlink_adsb_vehicle_t local_adsb_vehicle;
    mavlink_msg_adsb_vehicle_decode(&message, &local_adsb_vehicle);

    Transponder::AdsbVehicle adsbVehicle;
    adsbVehicle.icao_address = local_adsb_vehicle.ICAO_address;
    adsbVehicle.latitude_deg = local_adsb_vehicle.lat * 1e-7;
    adsbVehicle.longitude_deg = local_adsb_vehicle.lon * 1e-7;
    adsbVehicle.absolute_altitude_m = local_adsb_vehicle.altitude * 1e-3f;
    adsbVehicle.heading_deg = local_adsb_vehicle.heading * 1e-2f;
    adsbVehicle.horizontal_velocity_m_s = local_adsb_vehicle.hor_velocity * 1e-2f;
    adsbVehicle.vertical_velocity_m_s = local_adsb_vehicle.ver_velocity * 1e-2f;
    adsbVehicle.callsign = local_adsb_vehicle.callsign;
    adsbVehicle.emitter_type = Transponder::AdsbEmitterType(local_adsb_vehicle.emitter_type);
    adsbVehicle.squawk = local_adsb_vehicle.squawk;
    adsbVehicle.tslc_s = local_adsb_vehicle.tslc;

    set_transponder(adsbVehicle);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_transponder_subscription) {
        auto callback = _transponder_subscription;
        auto arg = transponder();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

Transponder::Result
TransponderImpl::transponder_result_from_command_result(MavlinkCommandSender::Result command_result)
{
    switch (command_result) {
        case MavlinkCommandSender::Result::Success:
            return Transponder::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return Transponder::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return Transponder::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
            return Transponder::Result::Busy;
        case MavlinkCommandSender::Result::CommandDenied:
            return Transponder::Result::CommandDenied;
        case MavlinkCommandSender::Result::Timeout:
            return Transponder::Result::Timeout;
        default:
            return Transponder::Result::Unknown;
    }
}

void TransponderImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, const Transponder::ResultCallback& callback)
{
    Transponder::Result action_result = transponder_result_from_command_result(command_result);

    callback(action_result);
}

} // namespace mavsdk
