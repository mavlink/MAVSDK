#include "server_utility_impl.h"
#include <limits>

namespace mavsdk {

ServerUtilityImpl::ServerUtilityImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

ServerUtilityImpl::ServerUtilityImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

ServerUtilityImpl::~ServerUtilityImpl()
{
    _system_impl->unregister_plugin(this);
}

void ServerUtilityImpl::init() {}

void ServerUtilityImpl::deinit() {}

void ServerUtilityImpl::enable() {}

void ServerUtilityImpl::disable() {}

std::optional<MAV_SEVERITY>
ServerUtilityImpl::mav_severity_from_status_text_type(ServerUtility::StatusTextType type)
{
    switch (type) {
        case ServerUtility::StatusTextType::Debug:
            return {MAV_SEVERITY_DEBUG};
        case ServerUtility::StatusTextType::Info:
            return {MAV_SEVERITY_INFO};
        case ServerUtility::StatusTextType::Notice:
            return {MAV_SEVERITY_NOTICE};
        case ServerUtility::StatusTextType::Warning:
            return {MAV_SEVERITY_WARNING};
        case ServerUtility::StatusTextType::Error:
            return {MAV_SEVERITY_ERROR};
        case ServerUtility::StatusTextType::Critical:
            return {MAV_SEVERITY_CRITICAL};
        case ServerUtility::StatusTextType::Alert:
            return {MAV_SEVERITY_ALERT};
        case ServerUtility::StatusTextType::Emergency:
            return {MAV_SEVERITY_EMERGENCY};
    }

    return {};
}

ServerUtility::Result
ServerUtilityImpl::send_status_text(ServerUtility::StatusTextType type, std::string text)
{
    constexpr unsigned chunk_size =
        sizeof(mavlink_statustext_t::text) / sizeof(mavlink_statustext_t::text[0]);
    constexpr unsigned max_len = chunk_size * std::numeric_limits<uint8_t>::max();

    // -1 for 0 termination.
    if (text.size() > max_len - 1) {
        return ServerUtility::Result::InvalidArgument;
    }

    const auto maybe_mav_severity = mav_severity_from_status_text_type(type);
    if (!maybe_mav_severity.has_value()) {
        return ServerUtility::Result::InvalidArgument;
    }

    // If the text size is a multiple of the chunk_size, we still need to add
    // one chunk for the 0 termination.
    const auto chunks_required = text.size() / chunk_size + 1;

    // The id starts from 1 if there are multiple chunks, it's 0 otherwise.
    uint16_t id = 0;
    if (chunks_required > 1) {
        id = _unique_send_id++;
    }

    for (unsigned chunk_seq = 0; chunk_seq < chunks_required; ++chunk_seq) {
        const char* pos = &text.c_str()[chunk_seq * chunk_size];
        char tmp_buf[chunk_size]{};
        const unsigned copy_len = std::min(chunk_size, static_cast<unsigned>(strlen(pos)));
        memcpy(tmp_buf, pos, copy_len);

        mavlink_message_t message;
        mavlink_msg_statustext_pack(
            _system_impl->get_own_system_id(),
            _system_impl->get_own_component_id(),
            &message,
            maybe_mav_severity.value(),
            tmp_buf,
            id,
            chunk_seq);

        if (!_system_impl->send_message(message)) {
            return ServerUtility::Result::ConnectionError;
        }
    }

    return ServerUtility::Result::Success;
}

} // namespace mavsdk
