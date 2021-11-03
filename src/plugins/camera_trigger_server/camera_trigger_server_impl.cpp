#include "camera_trigger_server_impl.h"

namespace mavsdk {

CameraTriggerServerImpl::CameraTriggerServerImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

CameraTriggerServerImpl::CameraTriggerServerImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

CameraTriggerServerImpl::~CameraTriggerServerImpl()
{
    _parent->unregister_plugin(this);
}

void CameraTriggerServerImpl::init() {
    _parent->register_mavlink_command_handler(
        MAV_CMD_IMAGE_START_CAPTURE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            auto interval = command.params.param2;
            auto total = static_cast<uint32_t>(command.params.param3);
            auto seq = static_cast<uint32_t>(command.params.param4);

            std::lock_guard<std::mutex> lock(_capture.mutex);
            // If sequence number is non-zero, it means a single capture has been requested.
            // Duplicate callbacks are avoided by keeping track of the previous sequence number.
            if (interval == 0  && total == 1 && seq > _capture.prev_sequence_num && _capture.callback) {
                _capture.prev_sequence_num = seq;
                _parent->call_user_callback(
                    [this, seq]() { _capture.callback(CameraTriggerServer::Result::Success, seq); });
            }

            mavlink_message_t msg;
            mavlink_msg_command_ack_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                &msg,
                command.command,
                MAV_RESULT::MAV_RESULT_ACCEPTED,
                std::numeric_limits<uint8_t>::max(),
                0,
                command.origin_system_id,
                command.origin_component_id);
            return msg;
        },
        this);
}

void CameraTriggerServerImpl::deinit() {}

void CameraTriggerServerImpl::enable() {}

void CameraTriggerServerImpl::disable() {}



void CameraTriggerServerImpl::subscribe_capture(CameraTriggerServer::CaptureCallback callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);
    _capture.callback = callback;
}

} // namespace mavsdk