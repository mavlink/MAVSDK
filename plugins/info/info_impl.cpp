#include <functional>
#include "info_impl.h"
#include "system.h"
#include "global_include.h"

namespace dronecode_sdk {

InfoImpl::InfoImpl(System &system) : PluginImplBase(system)
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
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        std::bind(&InfoImpl::process_autopilot_version, this, _1),
        this);
}

void InfoImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void InfoImpl::enable()
{
    // We can't rely on System to request the autopilot_version,
    // so we do it here, anyway.
    _parent->send_autopilot_version_request();

    // We're going to retry until we have the version.
    _parent->add_call_every(
        std::bind(&InfoImpl::request_version_again, this), 1.0f, &_call_every_cookie);
}

void InfoImpl::disable()
{
    _parent->remove_call_every(_call_every_cookie);

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _information_received = false;
    }
}

void InfoImpl::request_version_again()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_information_received) {
            _parent->remove_call_every(_call_every_cookie);
            return;
        }
    }

    _parent->send_autopilot_version_request();
}

void InfoImpl::process_autopilot_version(const mavlink_message_t &message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    _version.flight_sw_major = (autopilot_version.flight_sw_version >> (8 * 3)) & 0xFF;
    _version.flight_sw_minor = (autopilot_version.flight_sw_version >> (8 * 2)) & 0xFF;
    _version.flight_sw_patch = (autopilot_version.flight_sw_version >> (8 * 1)) & 0xFF;

    // first three bytes of flight_custon_version (little endian) describe vendor version
    translate_binary_to_str(autopilot_version.flight_custom_version + 3,
                            sizeof(autopilot_version.flight_custom_version) - 3,
                            _version.flight_sw_git_hash,
                            Info::GIT_HASH_STR_LEN);

    _version.flight_sw_vendor_major = autopilot_version.flight_custom_version[2];
    _version.flight_sw_vendor_minor = autopilot_version.flight_custom_version[1];
    _version.flight_sw_vendor_patch = autopilot_version.flight_custom_version[0];

    _version.os_sw_major = (autopilot_version.os_sw_version >> (8 * 3)) & 0xFF;
    _version.os_sw_minor = (autopilot_version.os_sw_version >> (8 * 2)) & 0xFF;
    _version.os_sw_patch = (autopilot_version.os_sw_version >> (8 * 1)) & 0xFF;

    // Debug() << "flight version: "
    //     << _version.flight_sw_major
    //     << "."
    //     << _version.flight_sw_minor
    //     << "."
    //     << _version.flight_sw_patch;

    // Debug() << "os version: "
    //     << _version.os_sw_major
    //     << "."
    //     << _version.os_sw_minor
    //     << "."
    //     << _version.os_sw_patch;

    translate_binary_to_str(autopilot_version.os_custom_version,
                            sizeof(autopilot_version.os_custom_version),
                            _version.os_sw_git_hash,
                            Info::GIT_HASH_STR_LEN);

    _product.vendor_id = autopilot_version.vendor_id;
    const char *vendor_name = vendor_id_str(autopilot_version.vendor_id);
    STRNCPY(_product.vendor_name, vendor_name, sizeof(_product.vendor_name) - 1);

    _product.product_id = autopilot_version.product_id;
    const char *product_name = product_id_str(autopilot_version.product_id);
    STRNCPY(_product.product_name, product_name, sizeof(_product.product_name) - 1);

    _information_received = true;
}

void InfoImpl::translate_binary_to_str(uint8_t *binary,
                                       unsigned binary_len,
                                       char *str,
                                       unsigned str_len)
{
    for (unsigned i = 0; i < binary_len; ++i) {
        // One hex number occupies 2 chars.
        // The binary is in little endian, therefore we need to swap the bytes for us to read.
        snprintf(&str[i * 2], str_len - i * 2, "%02x", binary[binary_len - 1 - i]);
    }
}

std::pair<Info::Result, uint64_t> InfoImpl::get_uuid() const
{
    // TODO: this should be processed in this plugin
    return std::make_pair<>(Info::Result::SUCCESS, _parent->get_uuid());
}

std::pair<Info::Result, Info::Version> InfoImpl::get_version() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return std::make_pair<>((_information_received ? Info::Result::SUCCESS :
                                                     Info::Result::INFORMATION_NOT_RECEIVED_YET),
                            _version);
}

std::pair<Info::Result, Info::Product> InfoImpl::get_product() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return std::make_pair<>((_information_received ? Info::Result::SUCCESS :
                                                     Info::Result::INFORMATION_NOT_RECEIVED_YET),
                            _product);
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

} // namespace dronecode_sdk
