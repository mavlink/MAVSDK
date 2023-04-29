#include "mavlink_parameter_server.h"
#include "mavlink_parameter_helper.h"
#include "plugin_base.h"
#include <cassert>

namespace mavsdk {

MavlinkParameterServer::MavlinkParameterServer(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    std::optional<std::map<std::string, ParamValue>> optional_param_values) :
    _sender(sender),
    _message_handler(message_handler)
{
    if (const char* env_p = std::getenv("MAVSDK_PARAMETER_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Parameter debugging is on.";
            _parameter_debugging = true;
        }
    }

    // Populate the parameter set before the first communication, if provided by the user.
    if (optional_param_values.has_value()) {
        const auto& param_values = optional_param_values.value();
        for (const auto& [key, value] : param_values) {
            const auto result = provide_server_param(key, value);
            if (result != Result::Success) {
                LogDebug() << "Cannot add parameter:" << key << ":" << value << " " << result;
            }
        }
    }

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_SET,
        [this](const mavlink_message_t& message) { process_param_set(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_EXT_SET,
        [this](const mavlink_message_t& message) { process_param_ext_set(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_REQUEST_READ,
        [this](const mavlink_message_t& message) { process_param_request_read(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_REQUEST_LIST,
        [this](const mavlink_message_t& message) { process_param_request_list(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_EXT_REQUEST_READ,
        [this](const mavlink_message_t& message) { process_param_ext_request_read(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_EXT_REQUEST_LIST,
        [this](const mavlink_message_t& message) { process_param_ext_request_list(message); },
        this);
}

MavlinkParameterServer::~MavlinkParameterServer()
{
    _message_handler.unregister_all(this);
}

MavlinkParameterServer::Result
MavlinkParameterServer::provide_server_param(const std::string& name, const ParamValue& param_value)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        return Result::ParamNameTooLong;
    }
    if (param_value.is<std::string>()) {
        const auto s = param_value.get<std::string>();
        if (s.size() > sizeof(mavlink_param_ext_set_t::param_value)) {
            LogErr() << "Error: param value too long";
            return Result::ParamValueTooLong;
        }
    }
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    // first we try to add it as a new parameter
    switch (_param_cache.add_new_param(name, param_value)) {
        case MavlinkParameterCache::AddNewParamResult::Ok:
            return Result::Success;
        case MavlinkParameterCache::AddNewParamResult::AlreadyExists:
            return Result::ParamExistsAlready;
        case MavlinkParameterCache::AddNewParamResult::TooManyParams:
            return Result::TooManyParams;
        default:
            LogErr() << "Unknown add_new_param result";
            assert(false);
    }

    // then, to not change the public api behaviour, try updating its value.
    switch (_param_cache.update_existing_param(name, param_value)) {
        case MavlinkParameterCache::UpdateExistingParamResult::Ok:
            return Result::Success;
        case MavlinkParameterCache::UpdateExistingParamResult::MissingParam:
            return Result::ParamNotFound;
        case MavlinkParameterCache::UpdateExistingParamResult::WrongType:
            return Result::WrongType;
        default:
            LogErr() << "Unknown update_existing_param result";
            assert(false);
    }

    return Result::Unknown;
}

MavlinkParameterServer::Result
MavlinkParameterServer::provide_server_param_float(const std::string& name, float value)
{
    ParamValue param_value;
    param_value.set(value);
    return provide_server_param(name, param_value);
}

MavlinkParameterServer::Result
MavlinkParameterServer::provide_server_param_int(const std::string& name, int32_t value)
{
    ParamValue param_value;
    param_value.set(value);
    return provide_server_param(name, param_value);
}

MavlinkParameterServer::Result MavlinkParameterServer::provide_server_param_custom(
    const std::string& name, const std::string& value)
{
    ParamValue param_value;
    param_value.set(value);
    return provide_server_param(name, param_value);
}

std::map<std::string, ParamValue> MavlinkParameterServer::retrieve_all_server_params()
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    std::map<std::string, ParamValue> map_copy;
    for (auto entry : _param_cache.all_parameters(true)) {
        map_copy[entry.id] = entry.value;
    }
    return map_copy;
}

template<class T>
std::pair<MavlinkParameterServer::Result, T>
MavlinkParameterServer::retrieve_server_param(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    const auto param_opt = _param_cache.param_by_id(name, true);
    if (!param_opt.has_value()) {
        return {Result::NotFound, {}};
    }
    // This parameter exists, check its type
    const auto& param = param_opt.value();
    if (param.value.is<T>()) {
        return {Result::Success, param.value.get<T>()};
    }
    return {Result::WrongType, {}};
}

std::pair<MavlinkParameterServer::Result, float>
MavlinkParameterServer::retrieve_server_param_float(const std::string& name)
{
    return retrieve_server_param<float>(name);
}

std::pair<MavlinkParameterServer::Result, std::string>
MavlinkParameterServer::retrieve_server_param_custom(const std::string& name)
{
    return retrieve_server_param<std::string>(name);
}

std::pair<MavlinkParameterServer::Result, int32_t>
MavlinkParameterServer::retrieve_server_param_int(const std::string& name)
{
    return retrieve_server_param<int32_t>(name);
}

void MavlinkParameterServer::process_param_set_internally(
    const std::string& param_id, const ParamValue& value_to_set, bool extended)
{
    // TODO: add debugging env
    LogDebug() << "Param set request" << (extended ? " extended" : "") << ": " << param_id
               << " with " << value_to_set;
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    // for checking if the update actually changed the value
    const auto opt_before_update = _param_cache.param_by_id(param_id, extended);
    const auto result = _param_cache.update_existing_param(param_id, value_to_set);
    const auto param_count = _param_cache.count(extended);

    switch (result) {
        case MavlinkParameterCache::UpdateExistingParamResult::MissingParam: {
            // We do not allow clients to add a new parameter to the parameter set, only to update
            // existing parameters. In this case, we cannot even respond with anything, since this
            // parameter simply does not exist.
            LogErr() << "Got param_set for non-existing parameter:" << param_id;
            return;
        }
        case MavlinkParameterCache::UpdateExistingParamResult::WrongType: {
            // Non-extended: we respond with the unchanged parameter.
            // Extended: we nack with failed.

            LogErr() << "Got param_set with wrong type for parameter: " << param_id;

            const auto curr_param = _param_cache.param_by_id(param_id, extended).value();

            if (extended) {
                auto new_work = std::make_shared<WorkItem>(
                    curr_param.id, curr_param.value, WorkItemAck{PARAM_ACK_FAILED});
                _work_queue.push_back(new_work);

            } else {
                auto new_work = std::make_shared<WorkItem>(
                    curr_param.id,
                    curr_param.value,
                    WorkItemValue{curr_param.index, param_count, extended});
                _work_queue.push_back(new_work);
            }
            return;
        }
        case MavlinkParameterCache::UpdateExistingParamResult::Ok: {
            const auto updated_parameter = _param_cache.param_by_id(param_id, extended).value();
            // The param set doesn't differentiate between an update that actually changed the value
            // e.g. 0 to 1 and an update that had no effect e.g. 0 to 0.
            if (opt_before_update.has_value() &&
                opt_before_update.value().value == updated_parameter.value) {
                LogDebug() << "Update had no effect: " << updated_parameter.value;
            } else {
                LogDebug() << "Updated param to :" << updated_parameter.value;
                find_and_call_subscriptions_value_changed(
                    updated_parameter.id, updated_parameter.value);
            }
            if (extended) {
                auto new_work = std::make_shared<WorkItem>(
                    updated_parameter.id, updated_parameter.value, WorkItemAck{PARAM_ACK_ACCEPTED});
                _work_queue.push_back(new_work);
            } else {
                auto new_work = std::make_shared<WorkItem>(
                    updated_parameter.id,
                    updated_parameter.value,
                    WorkItemValue{updated_parameter.index, param_count, extended});
                _work_queue.push_back(new_work);
            }
        } break;
    }
}

void MavlinkParameterServer::process_param_set(const mavlink_message_t& message)
{
    mavlink_param_set_t set_request{};
    mavlink_msg_param_set_decode(&message, &set_request);
    if (!target_matches(set_request.target_system, set_request.target_component, false)) {
        log_target_mismatch(set_request.target_system, set_request.target_component);
        return;
    }
    const std::string safe_param_id = extract_safe_param_id(set_request.param_id);

    if (safe_param_id.empty()) {
        LogWarn() << "Got ill-formed param_set message (param_id empty)";
        return;
    }

    ParamValue value_to_set;
    if (!value_to_set.set_from_mavlink_param_set_bytewise(set_request)) {
        // This should never happen, the type enum in the message is unknown.
        LogWarn() << "Invalid Param Set Request: " << safe_param_id;
        return;
    }
    process_param_set_internally(safe_param_id, value_to_set, false);
}

void MavlinkParameterServer::process_param_ext_set(const mavlink_message_t& message)
{
    mavlink_param_ext_set_t set_request{};
    mavlink_msg_param_ext_set_decode(&message, &set_request);
    if (!target_matches(set_request.target_system, set_request.target_component, false)) {
        log_target_mismatch(set_request.target_system, set_request.target_component);
        return;
    }
    const std::string safe_param_id = extract_safe_param_id(set_request.param_id);

    if (safe_param_id.empty()) {
        LogWarn() << "Got ill-formed param_ext_set message (param_id empty)";
        return;
    }

    ParamValue value_to_set;
    if (!value_to_set.set_from_mavlink_param_ext_set(set_request)) {
        // This should never happen, the type enum in the message is unknown.
        LogWarn() << "Invalid Param Set ext Request: " << safe_param_id;
        return;
    }
    process_param_set_internally(safe_param_id, value_to_set, true);
}

void MavlinkParameterServer::process_param_request_read(const mavlink_message_t& message)
{
    if (_parameter_debugging) {
        LogDebug() << "process param_request_read";
    }
    mavlink_param_request_read_t read_request{};
    mavlink_msg_param_request_read_decode(&message, &read_request);
    if (!target_matches(read_request.target_system, read_request.target_component, true)) {
        log_target_mismatch(read_request.target_system, read_request.target_component);
        return;
    }

    const auto param_id_or_index =
        extract_request_read_param_identifier(read_request.param_index, read_request.param_id);

    std::visit(
        overloaded{
            [&](std::monostate) { LogWarn() << "Ill-formed param_request_read message"; },
            [&](std::uint16_t index) {
                if (_parameter_debugging) {
                    LogDebug() << "found index: " << index;
                }
                internal_process_param_request_read_by_index(index, false);
            },
            [&](const std::string& id) {
                if (_parameter_debugging) {
                    LogDebug() << "found id: " << id;
                }
                internal_process_param_request_read_by_id(id, false);
            }},
        param_id_or_index);
}

void MavlinkParameterServer::process_param_ext_request_read(const mavlink_message_t& message)
{
    LogDebug() << "process param_ext_request_read";
    mavlink_param_ext_request_read_t read_request{};
    mavlink_msg_param_ext_request_read_decode(&message, &read_request);
    if (!target_matches(read_request.target_system, read_request.target_component, true)) {
        log_target_mismatch(read_request.target_system, read_request.target_component);
        return;
    }
    const auto param_id_or_index =
        extract_request_read_param_identifier(read_request.param_index, read_request.param_id);

    std::visit(
        overloaded{
            [&](std::monostate) { LogWarn() << "Ill-formed param_request_read message"; },
            [&](std::uint16_t index) {
                if (_parameter_debugging) {
                    LogDebug() << "found index: " << index;
                }
                internal_process_param_request_read_by_index(index, true);
            },
            [&](const std::string& id) {
                if (_parameter_debugging) {
                    LogDebug() << "found id: " << id;
                }
                internal_process_param_request_read_by_id(id, true);
            }},
        param_id_or_index);
}

void MavlinkParameterServer::internal_process_param_request_read_by_id(
    const std::string& id, const bool extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    const auto param_opt = _param_cache.param_by_id(id, extended);

    if (!param_opt.has_value()) {
        LogWarn() << "Ignoring request_read message " << (extended ? "extended " : "")
                  << "- param name not found: " << id;
        return;
    }
    const auto& param = param_opt.value();
    const auto param_count = _param_cache.count(extended);
    assert(param.index < param_count);
    auto new_work = std::make_shared<WorkItem>(
        param.id, param.value, WorkItemValue{param.index, param_count, extended});
    _work_queue.push_back(new_work);
}

void MavlinkParameterServer::internal_process_param_request_read_by_index(
    std::uint16_t index, bool extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    const auto param_opt = _param_cache.param_by_index(index, extended);

    if (!param_opt.has_value()) {
        LogWarn() << "Ignoring request_read message " << (extended ? "extended " : "")
                  << "- param index not found: " << index;
        return;
    }
    const auto& param = param_opt.value();
    const auto param_count = _param_cache.count(extended);

    assert(param.index < param_count);
    auto new_work = std::make_shared<WorkItem>(
        param.id, param.value, WorkItemValue{param.index, param_count, extended});
    _work_queue.push_back(new_work);
}

void MavlinkParameterServer::process_param_request_list(const mavlink_message_t& message)
{
    mavlink_param_request_list_t list_request{};
    mavlink_msg_param_request_list_decode(&message, &list_request);
    if (!target_matches(list_request.target_system, list_request.target_component, true)) {
        log_target_mismatch(list_request.target_system, list_request.target_component);
        return;
    }
    broadcast_all_parameters(false);
}

void MavlinkParameterServer::process_param_ext_request_list(const mavlink_message_t& message)
{
    if (_parameter_debugging) {
        LogDebug() << "process param_ext_request_list";
    }

    mavlink_param_ext_request_list_t ext_list_request{};
    mavlink_msg_param_ext_request_list_decode(&message, &ext_list_request);
    if (!target_matches(ext_list_request.target_system, ext_list_request.target_component, true)) {
        log_target_mismatch(ext_list_request.target_system, ext_list_request.target_component);
        return;
    }
    broadcast_all_parameters(true);
}

void MavlinkParameterServer::broadcast_all_parameters(const bool extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    const auto all_params = _param_cache.all_parameters(extended);
    if (_parameter_debugging) {
        LogDebug() << "broadcast_all_parameters " << (extended ? "extended" : "") << ": "
                   << all_params.size();
    }
    for (const auto& parameter : all_params) {
        if (_parameter_debugging) {
            LogDebug() << "sending param:" << parameter.id;
        }
        auto new_work = std::make_shared<WorkItem>(
            parameter.id,
            parameter.value,
            WorkItemValue{parameter.index, static_cast<uint16_t>(all_params.size()), extended});
        _work_queue.push_back(new_work);
    }
}

void MavlinkParameterServer::do_work()
{
    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();
    if (!work) {
        return;
    }
    const auto param_id_message_buffer = param_id_to_message_buffer(work->param_id);

    mavlink_message_t mavlink_message;

    std::visit(
        overloaded{
            [&](const WorkItemValue& specific) {
                if (specific.extended) {
                    const auto buf = work->param_value.get_128_bytes();
                    mavlink_msg_param_ext_value_pack(
                        _sender.get_own_system_id(),
                        _sender.get_own_component_id(),
                        &mavlink_message,
                        param_id_message_buffer.data(),
                        buf.data(),
                        work->param_value.get_mav_param_ext_type(),
                        specific.param_count,
                        specific.param_index);
                } else {
                    float param_value;
                    if (_sender.autopilot() == Sender::Autopilot::ArduPilot) {
                        param_value = work->param_value.get_4_float_bytes_cast();
                    } else {
                        param_value = work->param_value.get_4_float_bytes_bytewise();
                    }
                    mavlink_msg_param_value_pack(
                        _sender.get_own_system_id(),
                        _sender.get_own_component_id(),
                        &mavlink_message,
                        param_id_message_buffer.data(),
                        param_value,
                        work->param_value.get_mav_param_type(),
                        specific.param_count,
                        specific.param_index);
                }
                if (!_sender.send_message(mavlink_message)) {
                    LogErr() << "Error: Send message failed";
                    work_queue_guard.pop_front();
                    return;
                }
                work_queue_guard.pop_front();
            },
            [&](const WorkItemAck& specific) {
                auto buf = work->param_value.get_128_bytes();
                mavlink_msg_param_ext_ack_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &mavlink_message,
                    param_id_message_buffer.data(),
                    buf.data(),
                    work->param_value.get_mav_param_ext_type(),
                    specific.param_ack);
                if (!_sender.send_message(mavlink_message)) {
                    LogErr() << "Error: Send message failed";
                    work_queue_guard.pop_front();
                    return;
                }
                work_queue_guard.pop_front();
            }},
        work->work_item_variant);
}

std::ostream& operator<<(std::ostream& str, const MavlinkParameterServer::Result& result)
{
    switch (result) {
        case MavlinkParameterServer::Result::Success:
            return str << "Success";
        case MavlinkParameterServer::Result::WrongType:
            return str << "WrongType";
        case MavlinkParameterServer::Result::ParamNameTooLong:
            return str << "ParamNameTooLong";
        case MavlinkParameterServer::Result::NotFound:
            return str << "NotFound";
        case MavlinkParameterServer::Result::ParamValueTooLong:
            return str << ":ParamValueTooLong";
        default:
            return str << "UnknownError";
    }
}

bool MavlinkParameterServer::target_matches(
    const uint16_t target_sys_id, const uint16_t target_comp_id, bool is_request)
{
    // See: https://mavlink.io/en/services/parameter.html#multi-system-and-multi-component-support

    if (target_sys_id != _sender.get_own_system_id()) {
        return false;
    }
    if (is_request) {
        return target_comp_id == _sender.get_own_component_id() ||
               target_comp_id == MAV_COMP_ID_ALL;
    }
    return target_comp_id == _sender.get_own_component_id();
}

void MavlinkParameterServer::log_target_mismatch(uint16_t target_sys_id, uint16_t target_comp_id)
{
    if (!_parameter_debugging) {
        return;
    }

    LogDebug() << "Ignoring message - wrong target id. Got:" << (int)target_sys_id << ":"
               << (int)target_comp_id << " Wanted:" << (int)_sender.get_own_system_id() << ":"
               << (int)_sender.get_own_component_id();
}

std::variant<std::monostate, std::string, std::uint16_t>
MavlinkParameterServer::extract_request_read_param_identifier(
    int16_t param_index, const char* param_id)
{
    // Helper for safely handling a request_read or ext_request_read message (which have the exact
    // same layout). returns the identifier that should be used or nothing if the message is
    // ill-formed. See https://mavlink.io/en/messages/common.html#PARAM_REQUEST_READ and
    // https://mavlink.io/en/messages/common.html#PARAM_EXT_REQUEST_READ

    if (param_index == -1) {
        // use param_id if index == -1
        const auto safe_param_id = extract_safe_param_id(param_id);
        if (safe_param_id.empty()) {
            LogErr() << "Message with param_index=-1 but no empty param id";
            return std::monostate{};
        }
        return {safe_param_id};
    } else {
        // if index is not -1, it should be a valid parameter index (>=0)
        if (param_index < 0) {
            LogErr() << "Param_index " << param_index << " is not a valid param index";
            return std::monostate{};
        }
        return {static_cast<std::uint16_t>(param_index)};
    }
}

} // namespace mavsdk
