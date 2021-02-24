#pragma once

#include <mutex>

#include "global_include.h"
#include "mavlink_include.h"
#include "plugins/info/info.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class InfoImpl : public PluginImplBase {
public:
    explicit InfoImpl(System& system);
    explicit InfoImpl(std::shared_ptr<System> system);
    explicit InfoImpl(SystemImpl* system_impl);
    ~InfoImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<Info::Result, Info::Identification> get_identification() const;
    std::pair<Info::Result, Info::Version> get_version() const;
    std::pair<Info::Result, Info::Product> get_product() const;
    std::pair<Info::Result, Info::FlightInfo> get_flight_information() const;
    std::pair<Info::Result, double> get_speed_factor() const;

    InfoImpl(const InfoImpl&) = delete;
    InfoImpl& operator=(const InfoImpl&) = delete;

private:
    void request_version_again();
    void request_flight_information();
    void process_heartbeat(const mavlink_message_t& message);
    void process_autopilot_version(const mavlink_message_t& message);
    void process_flight_information(const mavlink_message_t& message);
    void process_attitude(const mavlink_message_t& message);

    mutable std::mutex _mutex{};

    Info::Version _version{};
    Info::Product _product{};
    Info::Identification _identification{};
    Info::FlightInfo _flight_info{};
    bool _information_received{false};
    bool _flight_information_received{false};
    bool _was_armed{false};

    void* _call_every_cookie{nullptr};
    void* _flight_info_call_every_cookie{nullptr};

    double _speed_factor{double(NAN)};
    Time _time{};
    dl_time_t _last_time_attitude_arrived{};
    uint32_t _last_time_boot_ms{0};

    static const std::string vendor_id_str(uint16_t vendor_id);
    static const std::string product_id_str(uint16_t product_id);

    static std::string swap_and_translate_binary_to_str(uint8_t* binary, unsigned binary_len);
    static std::string translate_binary_to_str(uint8_t* binary, unsigned binary_len);
};

} // namespace mavsdk
