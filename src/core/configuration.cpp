#include "configuration.h"

namespace mavsdk {

Configuration::Configuration(uint8_t system_id, uint8_t component_id) :
    _address{system_id, component_id},
    _usage_type(Configuration::UsageType::Custom) {
}

Configuration::Configuration(UsageType usage_type) :
    _address{245, 1},
    _usage_type(usage_type) {
}

Configuration::~Configuration() {}

uint8_t Configuration::get_system_id() const {
    return _address.system_id;
}

uint8_t Configuration::get_component_id() const {
    return _address.component_id;
}

Configuration::UsageType Configuration::get_usage_type() const {
    return _usage_type;
}

} // namespace mavsdk
