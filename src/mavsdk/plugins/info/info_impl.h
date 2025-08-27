#pragma once

#include <mutex>

#include "mavlink_include.h"
#include "plugins/info/info.h"
#include "plugin_impl_base.h"
#include "ringbuffer.h"
#include "callback_list.h"

namespace mavsdk {

class InfoImpl : public PluginImplBase {
public:
    explicit InfoImpl(System& system);
    explicit InfoImpl(std::shared_ptr<System> system);
    ~InfoImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<Info::Result, Info::Identification> get_identification() const;
    std::pair<Info::Result, Info::Version> get_version() const;
    std::pair<Info::Result, Info::Product> get_product() const;
    std::pair<Info::Result, Info::FlightInfo> get_flight_information();
    std::pair<Info::Result, double> get_speed_factor() const;

    Info::FlightInformationHandle
    subscribe_flight_information(const Info::FlightInformationCallback& callback);
    void unsubscribe_flight_information(Info::FlightInformationHandle handle);

    InfoImpl(const InfoImpl&) = delete;
    InfoImpl& operator=(const InfoImpl&) = delete;

private:
    void process_heartbeat(const mavlink_message_t& message);
    void process_autopilot_version(const mavlink_message_t& message);
    void process_flight_information(const mavlink_message_t& message);
    void process_attitude(const mavlink_message_t& message);

    Info::Version::FlightSoftwareVersionType
        get_flight_software_version_type(FIRMWARE_VERSION_TYPE);

    void wait_for_identification() const;

    mutable std::mutex _mutex{};

    Info::Version _version{};
    Info::Product _product{};
    Info::Identification _identification{};
    bool _identification_received{false};

    Info::FlightInfo _flight_info{};

    CallEveryHandler::Cookie _call_every_cookie{};

    struct SpeedFactorMeasurement {
        double simulated_duration_s{0.0};
        double real_time_s{0.0};

        SpeedFactorMeasurement operator+(SpeedFactorMeasurement& rhs) const
        {
            return {
                this->simulated_duration_s + rhs.simulated_duration_s,
                this->real_time_s + rhs.real_time_s};
        }
    };
    Ringbuffer<SpeedFactorMeasurement, 50> _speed_factor_measurements;

    Time _time{};
    SteadyTimePoint _last_time_attitude_arrived{};
    uint32_t _last_time_boot_ms{0};

    CallbackList<Info::FlightInfo> _flight_info_subscriptions{};

    static const std::string vendor_id_str(uint16_t vendor_id);
    static const std::string product_id_str(uint16_t product_id);

    static std::string swap_and_translate_binary_to_str(uint8_t* binary, unsigned binary_len);
    static std::string translate_binary_to_str(uint8_t* binary, unsigned binary_len);
};

} // namespace mavsdk
