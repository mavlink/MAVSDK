#include <functional>
#include "info_impl.h"
#include "system.h"
#include "mavlink_system.h"
#include "global_include.h"

namespace dronecore {

InfoImpl::InfoImpl(System &system) :
    PluginImplBase(system),
    _version_mutex(),
    _version()
{
    _parent->register_plugin(this);
}

InfoImpl::~InfoImpl()
{
    _parent->unregister_plugin(this);
}

void InfoImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&InfoImpl::process_heartbeat, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        std::bind(&InfoImpl::process_autopilot_version, this, _1), this);
}

void InfoImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void InfoImpl::enable() {}

void InfoImpl::disable() {}

void InfoImpl::process_heartbeat(const mavlink_message_t &message)
{
    UNUSED(message);

    if (!is_complete()) {
        // We try to request more info if not all info is available.
        // We can't rely on System to request the autopilot_version,
        // so we do it here, anyway.
        _parent->request_autopilot_version();
    }
}

void InfoImpl::process_autopilot_version(const mavlink_message_t &message)
{
    mavlink_autopilot_version_t autopilot_version;

    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    Info::Version version {};

    version.flight_sw_major = (autopilot_version.flight_sw_version >> (8 * 3)) & 0xFF;
    version.flight_sw_minor = (autopilot_version.flight_sw_version >> (8 * 2)) & 0xFF;
    version.flight_sw_patch = (autopilot_version.flight_sw_version >> (8 * 1)) & 0xFF;

    // first three bytes of flight_custon_version (little endian) describe vendor version
    translate_binary_to_str(autopilot_version.flight_custom_version + 3,
                            sizeof(autopilot_version.flight_custom_version) - 3,
                            version.flight_sw_git_hash,
                            Info::GIT_HASH_STR_LEN);

    version.flight_sw_vendor_major = autopilot_version.flight_custom_version[2];
    version.flight_sw_vendor_minor = autopilot_version.flight_custom_version[1];
    version.flight_sw_vendor_patch = autopilot_version.flight_custom_version[0];

    version.os_sw_major = (autopilot_version.os_sw_version >> (8 * 3)) & 0xFF;
    version.os_sw_minor = (autopilot_version.os_sw_version >> (8 * 2)) & 0xFF;
    version.os_sw_patch = (autopilot_version.os_sw_version >> (8 * 1)) & 0xFF;

    // Debug() << "flight version: "
    //     << version.flight_sw_major
    //     << "."
    //     << version.flight_sw_minor
    //     << "."
    //     << version.flight_sw_patch;

    // Debug() << "os version: "
    //     << version.os_sw_major
    //     << "."
    //     << version.os_sw_minor
    //     << "."
    //     << version.os_sw_patch;

    translate_binary_to_str(autopilot_version.os_custom_version,
                            sizeof(autopilot_version.os_custom_version),
                            version.os_sw_git_hash,
                            Info::GIT_HASH_STR_LEN);

    set_version(version);

    Info::Product product {};

    product.vendor_id = autopilot_version.vendor_id;
    const char *vendor_name = vendor_id_str(autopilot_version.vendor_id);
    STRNCPY(product.vendor_name, vendor_name, sizeof(product.vendor_name));

    product.product_id = autopilot_version.product_id;
    const char *product_name = product_id_str(autopilot_version.product_id);
    STRNCPY(product.product_name, product_name, sizeof(product.product_name));

    set_product(product);
}

void InfoImpl::translate_binary_to_str(uint8_t *binary, unsigned binary_len,
                                       char *str, unsigned str_len)
{
    for (unsigned i = 0; i < binary_len; ++i) {
        // One hex number occupies 2 chars.
        // The binary is in little endian, therefore we need to swap the bytes for us to read.
        snprintf(&str[i * 2], str_len - i * 2,
                 "%02x",
                 binary[binary_len - 1 - i]);
    }
}

uint64_t InfoImpl::get_uuid() const
{
    return _parent->get_uuid();
}

bool InfoImpl::is_complete() const
{
    {
        std::lock_guard<std::mutex> lock(_version_mutex);

        // TODO: check OS version, it's currently just 0 for SITL
        if (_version.flight_sw_major == 0/* || _version.os_sw_major == 0*/) {
            return false;
        }
    }

    return true;
}

Info::Version InfoImpl::get_version() const
{
    std::lock_guard<std::mutex> lock(_version_mutex);
    return _version;
}

Info::Product InfoImpl::get_product() const
{
    std::lock_guard<std::mutex> lock(_product_mutex);
    return _product;
}

void InfoImpl::set_version(Info::Version version)
{
    std::lock_guard<std::mutex> lock(_version_mutex);
    _version = version;
}

void InfoImpl::set_product(Info::Product product)
{
    std::lock_guard<std::mutex> lock(_product_mutex);
    _product = product;
}

const char *InfoImpl::vendor_id_str(uint16_t vendor_id)
{
    switch (vendor_id) {
        case 0x26ac:
            return "Yuneec";
        default:
            return "undefined";
    }
}

const char *InfoImpl::product_id_str(uint16_t product_id)
{
    switch (product_id) {
        case 0x0010:
            return "H520";
        default:
            return "undefined";
    }
}



} // namespace dronecore
