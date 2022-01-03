#include "component_information_server_impl.h"
#include "mavlink_request_message_handler.h"
#include "fs.h"
#include "crc32.h"
#include <fstream>
#include <string>

namespace mavsdk {

ComponentInformationServerImpl::ComponentInformationServerImpl(System& system) :
    PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ComponentInformationServerImpl::ComponentInformationServerImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _parent->register_plugin(this);
}

ComponentInformationServerImpl::~ComponentInformationServerImpl()
{
    _parent->unregister_plugin(this);
}

void ComponentInformationServerImpl::init() {}

void ComponentInformationServerImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void ComponentInformationServerImpl::enable() {}

void ComponentInformationServerImpl::disable() {}

ComponentInformationServer::Result
ComponentInformationServerImpl::provide_peripheral_file(const std::string& path)
{
    if (!fs_exists(path)) {
        return ComponentInformationServer::Result::NotFound;
    }

    _path = path;

    std::ifstream file(path, std::ios::binary);

    if (!file) {
        return ComponentInformationServer::Result::OpenFailure;
    }

    file.seekg(0, std::ifstream::end);
    auto length = file.tellg();
    file.seekg(0, std::ifstream::beg);

    std::string buf;
    buf.reserve(length);
    file.read(buf.data(), length);

    if (!file) {
        return ComponentInformationServer::Result::ReadFailure;
    }

    Crc32 crc32_calculator;
    crc32_calculator.add(reinterpret_cast<uint8_t*>(buf.data()), buf.length());
    _crc32 = crc32_calculator.get();

    _parent->register_mavlink_request_message_handler(
        MAVLINK_MSG_ID_COMPONENT_INFORMATION,
        [this](MavlinkRequestMessageHandler::Params) {
            return process_component_information_requested();
        },
        this);

    return ComponentInformationServer::Result::Success;
}

std::optional<MAV_RESULT> ComponentInformationServerImpl::process_component_information_requested()
{
    mavlink_message_t message;
    mavlink_msg_component_information_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_time().elapsed_ms(),
        0,
        "",
        _crc32,
        _path.c_str());
    _parent->send_message(message);

    return MAV_RESULT_ACCEPTED;
}

} // namespace mavsdk
