#pragma once

#include "info.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <mutex>

namespace dronecore {

class InfoImpl : public PluginImplBase
{
public:
    InfoImpl(System &system);
    ~InfoImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    uint64_t get_uuid() const;
    bool is_complete() const;
    Info::Version get_version() const;
    Info::Product get_product() const;

private:
    void set_version(Info::Version version);
    void set_product(Info::Product product);

    void process_heartbeat(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);

    mutable std::mutex _version_mutex;
    Info::Version _version = {};

    mutable std::mutex _product_mutex;
    Info::Product _product = {};

    static const char *vendor_id_str(uint16_t vendor_id);
    static const char *product_id_str(uint16_t product_id);

    static void translate_binary_to_str(uint8_t *binary, unsigned binary_len,
                                        char *str, unsigned str_len);
};

} // namespace dronecore
