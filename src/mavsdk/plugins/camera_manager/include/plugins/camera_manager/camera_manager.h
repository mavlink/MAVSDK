#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "plugin_base.h"
#include "mavlink_include.h"

namespace mavsdk {

class CameraManagerPluginImpl : public PluginImplBase {
public:
    explicit CameraManagerPluginImpl(System& system) : PluginImplBase(system)
    {
        _parent->register_plugin(this);
    }
    explicit CameraManagerPluginImpl(std::shared_ptr<System> system) : PluginImplBase(system)
    {
        _parent->register_plugin(this);
    }
    ~CameraManagerPluginImpl() { _parent->unregister_plugin(this); }

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void provide_camera_information(
        const std::function<(mavlink_camera_information_t & camera_information)>& callback)
    {
        callback(camera_information);
        camera_information_valid = true;
    }

    template<class T>
    void add_param(
        const std::string& name, T default_value, const typename std::function<void(T t)>& callback)
    {
        _parent->provide_server_param_int("CAM_ISO", default_value);
        _parent->subscribe_param(
            name,
            overloaded{
                [&name](auto value) {
                    std::cerr << "invalid parameter type for " << name << std::endl;
                },
                callback},
            nullptr);
    }

private:
    mavlink_camera_information_t camera_information;
    bool camera_information_valid{false};
};

void CameraManagerPluginImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_LONG,
        [this](const mavlink_message_t& message) {
            mavlink_command_long_t cmd;
            mavlink_msg_command_long_decode(&msg, &cmd);
            switch (cmd.command) {
                case MAV_CMD_REQUEST_CAMERA_INFORMATION:
                    if (camera_information_valid) {
                        mavlink_message_t msg;
                        mavlink_msg_camera_information_encode(
                            _parent->get_own_system_id(),
                            _parent->get_own_component_id(),
                            &msg,
                            &camera_information);
                        _parent->send_message(msg);
                    }
                    break;
                default:
                    break;
            }
        },
        this);
}
void CameraManagerPluginImpl::deinit() {}
void CameraManagerPluginImpl::enable() {}
void CameraManagerPluginImpl::disable() {}

} // namespace mavsdk
