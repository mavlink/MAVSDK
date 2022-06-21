#include "mavlink_parameter_sender.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "system_impl.h"
#include <algorithm>
#include <cstring>
#include <future>

namespace mavsdk {

MavlinkParameterSender::MavlinkParameterSender(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    TimeoutSCallback timeout_s_callback) :
    _sender(sender),
    _message_handler(message_handler),
    _timeout_handler(timeout_handler),
    _timeout_s_callback(timeout_s_callback)
{
    if (const char* env_p = std::getenv("MAVSDK_PARAMETER_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Parameter debugging is on.";
            _parameter_debugging = true;
        }
    }

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_VALUE,
        [this](const mavlink_message_t& message) { process_param_value(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_EXT_VALUE,
        [this](const mavlink_message_t& message) { process_param_ext_value(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_PARAM_EXT_ACK,
        [this](const mavlink_message_t& message) { process_param_ext_ack(message); },
        this);
}

MavlinkParameterSender::~MavlinkParameterSender()
{
    _message_handler.unregister_all(this);
}

MavlinkParameterSender::Result MavlinkParameterSender::set_param(
    const std::string& name,
    ParamValue value,

    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    auto prom = std::promise<Result>();
    auto res = prom.get_future();

    set_param_async(
        name,
        value,
        [&prom](Result result) { prom.set_value(result); },
        this,
        maybe_component_id,
        extended);

    return res.get();
}

void MavlinkParameterSender::set_param_async(
    const std::string& name,
    ParamValue value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(Result::ParamNameTooLong);
        }
        return;
    }

    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());

    new_work->type = WorkItem::Type::Set;
    new_work->callback = callback;
    new_work->maybe_component_id = maybe_component_id;
    new_work->param_name = name;
    new_work->param_value = value;
    new_work->extended = extended;
    new_work->exact_type_known = true;
    new_work->cookie = cookie;
    _work_queue.push_back(new_work);
}

void MavlinkParameterSender::set_param_int_async(
    const std::string& name,
    int32_t value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(Result::ParamNameTooLong);
        }
        return;
    }

    // PX4 only uses int32_t, so we can be sure and don't need to check the exact type first
    // by getting the param, or checking the cache.
    const bool exact_int_type_known = (_sender.autopilot() == SystemImpl::Autopilot::Px4);

    const auto set_step = [=]() {
        auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());

        ParamValue value_to_set;
        value_to_set.set(value);

        new_work->type = WorkItem::Type::Set;
        new_work->callback = callback;
        new_work->param_name = name;
        new_work->param_value = value_to_set;
        new_work->extended = extended;
        new_work->exact_type_known = exact_int_type_known;
        new_work->cookie = cookie;
        _work_queue.push_back(new_work);
    };

    // We need to know the exact int type first.
    if (exact_int_type_known || _all_params.find(name) != _all_params.end()) {
        // We are sure about the type, or we have it cached, we'll be able to use it.
        set_step();
    } else {
        // We don't know the exact type, so we need to get it first.
        get_param_int_async(name, nullptr, cookie, maybe_component_id, extended);
        set_step();
    }
}

MavlinkParameterSender::Result MavlinkParameterSender::set_param_int(
    const std::string& name,
    int32_t value,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    auto prom = std::promise<Result>();
    auto res = prom.get_future();

    set_param_int_async(
        name,
        value,
        [&prom](Result result) { prom.set_value(result); },
        this,
        maybe_component_id,
        extended);

    return res.get();
}

void MavlinkParameterSender::set_param_float_async(
    const std::string& name,
    float value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(Result::ParamNameTooLong);
        }
        return;
    }

    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());

    ParamValue value_to_set;
    value_to_set.set_float(value);

    new_work->type = WorkItem::Type::Set;
    new_work->callback = callback;
    new_work->maybe_component_id = maybe_component_id;
    new_work->param_name = name;
    new_work->param_value = value_to_set;
    new_work->extended = extended;
    new_work->exact_type_known = true;
    new_work->cookie = cookie;
    _work_queue.push_back(new_work);
}

MavlinkParameterSender::Result MavlinkParameterSender::set_param_float(
    const std::string& name, float value, std::optional<uint8_t> maybe_component_id, bool extended)
{
    auto prom = std::promise<Result>();
    auto res = prom.get_future();

    set_param_float_async(
        name,
        value,
        [&prom](Result result) { prom.set_value(result); },
        this,
        maybe_component_id,
        extended);

    return res.get();
}

void MavlinkParameterSender::get_param_float_async(
    const std::string& name,
    const GetParamFloatCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    if (_parameter_debugging) {
        LogDebug() << "getting param " << name << ", extended: " << (extended ? "yes" : "no");
    }

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(MavlinkParameterSender::Result::ParamNameTooLong, NAN);
        }
        return;
    }

    ParamValue value_type;
    value_type.set(NAN);

    // Otherwise, push work onto queue.
    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
    new_work->type = WorkItem::Type::Get;
    new_work->callback = callback;
    new_work->maybe_component_id = maybe_component_id;
    new_work->param_name = name;
    new_work->param_value = value_type;
    new_work->extended = extended;
    new_work->cookie = cookie;

    _work_queue.push_back(new_work);
}

void MavlinkParameterSender::get_param_async(
    const std::string& name,
    ParamValue value,
    const GetParamAnyCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    if (_parameter_debugging) {
        LogDebug() << "getting param " << name << ", extended: " << (extended ? "yes" : "no");
    }

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(Result::ParamNameTooLong, ParamValue{});
        }
        return;
    }

    // Otherwise, push work onto queue.
    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
    new_work->type = WorkItem::Type::Get;
    new_work->callback = callback;
    new_work->maybe_component_id = maybe_component_id;
    new_work->param_name = name;
    new_work->param_value = value;
    new_work->extended = extended;
    new_work->exact_type_known = true;
    new_work->cookie = cookie;

    _work_queue.push_back(new_work);
}

void MavlinkParameterSender::get_param_int_async(
    const std::string& name,
    const GetParamIntCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    if (_parameter_debugging) {
        LogDebug() << "getting param " << name << ", extended: " << (extended ? "yes" : "no");
    }

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(MavlinkParameterSender::Result::ParamNameTooLong, 0);
        }
        return;
    }

    // Otherwise, push work onto queue.
    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
    new_work->type = WorkItem::Type::Get;
    new_work->callback = callback;
    new_work->maybe_component_id = maybe_component_id;
    new_work->param_name = name;
    new_work->param_value = {};
    new_work->extended = extended;
    new_work->cookie = cookie;

    _work_queue.push_back(new_work);
}

MavlinkParameterSender::Result
MavlinkParameterSender::set_param_custom(const std::string& name, const std::string& value)
{
    auto prom = std::promise<Result>();
    auto res = prom.get_future();

    set_param_custom_async(
        name, value, [&prom](Result result) { prom.set_value(result); }, this);

    return res.get();
}

void MavlinkParameterSender::get_param_custom_async(
    const std::string& name, const GetParamCustomCallback& callback, const void* cookie)
{
    if (_parameter_debugging) {
        LogDebug() << "getting param " << name;
    }

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(MavlinkParameterSender::Result::ParamNameTooLong, 0);
        }
        return;
    }

    // Otherwise, push work onto queue.
    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
    new_work->type = WorkItem::Type::Get;
    new_work->callback = callback;
    new_work->param_name = name;
    new_work->param_value.set_custom(std::string());
    new_work->extended = true;
    new_work->cookie = cookie;

    _work_queue.push_back(new_work);
}

void MavlinkParameterSender::set_param_custom_async(
    const std::string& name,
    const std::string& value,
    const SetParamCallback& callback,
    const void* cookie)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(Result::ParamNameTooLong);
        }
        return;
    }

    if (value.size() > sizeof(mavlink_param_ext_set_t::param_value)) {
        LogErr() << "Error: param value too long";
        if (callback) {
            callback(Result::ParamValueTooLong);
        }
        return;
    }

    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());

    ParamValue value_to_set;
    value_to_set.set_custom(value);

    new_work->type = WorkItem::Type::Set;
    new_work->callback = callback;
    new_work->param_name = name;
    new_work->param_value = value_to_set;
    new_work->extended = true;
    new_work->exact_type_known = true;
    new_work->cookie = cookie;
    _work_queue.push_back(new_work);
}

std::pair<MavlinkParameterSender::Result, ParamValue>
MavlinkParameterSender::get_param(const std::string& name, ParamValue value, bool extended)
{
    auto prom = std::promise<std::pair<Result, ParamValue>>();
    auto res = prom.get_future();

    get_param_async(
        name,
        value,
        [&prom](Result result, ParamValue new_value) {
            prom.set_value(std::make_pair<>(result, new_value));
        },
        this,
        extended);

    return res.get();
}

std::pair<MavlinkParameterSender::Result, int32_t> MavlinkParameterSender::get_param_int(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    auto prom = std::promise<std::pair<Result, int32_t>>();
    auto res = prom.get_future();

    get_param_int_async(
        name,
        [&prom](Result result, int32_t value) { prom.set_value(std::make_pair<>(result, value)); },
        this,
        maybe_component_id,
        extended);

    return res.get();
}

std::pair<MavlinkParameterSender::Result, float> MavlinkParameterSender::get_param_float(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    auto prom = std::promise<std::pair<Result, float>>();
    auto res = prom.get_future();

    get_param_float_async(
        name,
        [&prom](Result result, float value) { prom.set_value(std::make_pair<>(result, value)); },
        this,
        maybe_component_id,
        extended);

    return res.get();
}

std::pair<MavlinkParameterSender::Result, std::string>
MavlinkParameterSender::get_param_custom(const std::string& name)
{
    auto prom = std::promise<std::pair<Result, std::string>>();
    auto res = prom.get_future();

    get_param_custom_async(
        name,
        [&prom](Result result, const std::string& value) {
            prom.set_value(std::make_pair<>(result, value));
        },
        this);

    return res.get();
}

void MavlinkParameterSender::get_all_params_async(const GetAllParamsCallback& callback)
{
    std::unique_lock<std::mutex> lock(_all_params_mutex);

    _all_params_callback = callback;

    mavlink_message_t msg;

    mavlink_msg_param_request_list_pack(
        _sender.get_own_system_id(),
        _sender.get_own_component_id(),
        &msg,
        _sender.get_system_id(),
        MAV_COMP_ID_AUTOPILOT1); // FIXME: what should the component be?

    if (!_sender.send_message(msg)) {
        LogErr() << "Failed to send param list request!";
        lock.unlock();
        callback(std::map<std::string, ParamValue>{});
    }

    _timeout_handler.add(
        [this] { receive_timeout(); }, _timeout_s_callback(), &_all_params_timeout_cookie);
}

std::map<std::string, ParamValue> MavlinkParameterSender::get_all_params()
{
    std::promise<std::map<std::string, ParamValue>> prom;
    auto res = prom.get_future();

    get_all_params_async([&prom](const std::map<std::string, ParamValue>& all_params) {
        prom.set_value(all_params);
    });

    return res.get();
}

void MavlinkParameterSender::cancel_all_param(const void* cookie)
{
    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);

    for (auto item = _work_queue.begin(); item != _work_queue.end(); /* manual incrementation */) {
        if ((*item)->cookie == cookie) {
            item = _work_queue.erase(item);
        } else {
            ++item;
        }
    }
}

void MavlinkParameterSender::do_work()
{
    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (work->already_requested) {
        return;
    }

    char param_id[PARAM_ID_LEN + 1] = {};
    strncpy(param_id, work->param_name.c_str(), sizeof(param_id) - 1);

    uint8_t component_id = [&]() {
        if (work->maybe_component_id) {
            return work->maybe_component_id.value();
        } else {
            if (work->extended) {
                return static_cast<uint8_t>(MAV_COMP_ID_CAMERA);
            } else {
                return static_cast<uint8_t>(MAV_COMP_ID_AUTOPILOT1);
            }
        }
    }();

    switch (work->type) {
        case WorkItem::Type::Set: {
            if (!work->exact_type_known) {
                std::unique_lock<std::mutex> lock(_all_params_mutex);
                const auto it = _all_params.find(work->param_name);
                if (it == _all_params.end()) {
                    LogErr() << "Don't know the type of param_set";
                    lock.unlock();
                    if (std::get_if<SetParamCallback>(&work->callback)) {
                        const auto& callback = std::get<SetParamCallback>(work->callback);
                        if (callback) {
                            callback(MavlinkParameterSender::Result::Failed);
                        }
                    }
                    work_queue_guard.pop_front();
                    return;
                }

                auto maybe_temp_int = work->param_value.get_int();
                if (!maybe_temp_int) {
                    // Not sure what to think when this happens.
                    LogErr() << "Error: this should definitely be an int";
                } else {
                    // First we copy over the type
                    work->param_value = it->second;
                    // And then fill in the value.
                    work->param_value.set_int(maybe_temp_int.value());
                }
            }

            if (work->extended) {
                auto param_value_buf = work->param_value.get_128_bytes();

                // FIXME: extended currently always go to the camera component
                mavlink_msg_param_ext_set_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &work->mavlink_message,
                    _sender.get_system_id(),
                    component_id,
                    param_id,
                    param_value_buf.data(),
                    work->param_value.get_mav_param_ext_type());
            } else {
                float value_set = (_sender.autopilot() == SystemImpl::Autopilot::ArduPilot) ?
                                      work->param_value.get_4_float_bytes_cast() :
                                      work->param_value.get_4_float_bytes_bytewise();

                mavlink_msg_param_set_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &work->mavlink_message,
                    _sender.get_system_id(),
                    component_id,
                    param_id,
                    value_set,
                    work->param_value.get_mav_param_type());
            }

            if (!_sender.send_message(work->mavlink_message)) {
                LogErr() << "Error: Send message failed";
                if (std::get_if<SetParamCallback>(&work->callback)) {
                    const auto& callback = std::get<SetParamCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::ConnectionError);
                    }
                }
                work_queue_guard.pop_front();
                return;
            }

            work->already_requested = true;
            // _last_request_time = _sender.get_time().steady_time();

            // We want to get notified if a timeout happens
            _timeout_handler.add([this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);

        } break;

        case WorkItem::Type::Get: {
            // LogDebug() << "now getting: " << work->param_name;
            if (work->extended) {
                mavlink_msg_param_ext_request_read_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &work->mavlink_message,
                    _sender.get_system_id(),
                    component_id,
                    param_id,
                    -1);

            } else {
                // LogDebug() << "request read: "
                //    << (int)_sender.get_own_system_id() << ":"
                //    << (int)_sender.get_own_component_id() <<
                //    " to "
                //    << (int)_sender.get_system_id() << ":"
                //    << (int)_sender.get_autopilot_id();

                mavlink_msg_param_request_read_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &work->mavlink_message,
                    _sender.get_system_id(),
                    component_id,
                    param_id,
                    -1);
            }

            if (!_sender.send_message(work->mavlink_message)) {
                LogErr() << "Error: Send message failed";

                if (std::get_if<GetParamIntCallback>(&work->callback)) {
                    if (const auto& callback = std::get<GetParamIntCallback>(work->callback)) {
                        callback(Result::ConnectionError, 0);
                    }
                } else if (std::get_if<GetParamFloatCallback>(&work->callback)) {
                    if (const auto& callback = std::get<GetParamFloatCallback>(work->callback)) {
                        (callback)(Result::ConnectionError, NAN);
                    }
                } else if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                    if (const auto& callback = std::get<GetParamAnyCallback>(work->callback)) {
                        (callback)(Result::ConnectionError, ParamValue{});
                    }
                }
                work_queue_guard.pop_front();
                return;
            }

            work->already_requested = true;

            // _last_request_time = _sender.get_time().steady_time();

            // We want to get notified if a timeout happens
            _timeout_handler.add([this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);

        } break;
    }
}

void MavlinkParameterSender::process_param_value(const mavlink_message_t& message)
{
    mavlink_param_value_t param_value;
    mavlink_msg_param_value_decode(&message, &param_value);

    if (_parameter_debugging) {
        LogDebug() << "getting param value: " << extract_safe_param_id(param_value.param_id);
    }

    ParamValue received_value;
    if (_sender.autopilot() == SystemImpl::Autopilot::ArduPilot) {
        received_value.set_from_mavlink_param_value_cast(param_value);
    } else {
        received_value.set_from_mavlink_param_value_bytewise(param_value);
    }

    std::string param_id = extract_safe_param_id(param_value.param_id);

    {
        std::unique_lock<std::mutex> lock(_all_params_mutex);
        _all_params[param_id] = received_value;

        // check if we are looking for param list
        if (_all_params_callback) {
            if (param_value.param_index + 1 == param_value.param_count) {
                lock.unlock();
                _all_params_callback(_all_params);
            } else {
                _timeout_handler.remove(_all_params_timeout_cookie);

                _timeout_handler.add(
                    [this] { receive_timeout(); },
                    _timeout_s_callback(),
                    &_all_params_timeout_cookie);
            }

            return;
        }
    }

    // TODO check if value has changed ...
    find_and_call_subscriptions_value_changed(param_id,received_value);

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (!work->already_requested) {
        return;
    }

    if (work->param_name != extract_safe_param_id(param_value.param_id)) {
        // No match, let's just return the borrowed work item.
        return;
    }

    switch (work->type) {
        case WorkItem::Type::Get: {
            if (std::get_if<GetParamIntCallback>(&work->callback)) {
                const auto& callback = std::get<GetParamIntCallback>(work->callback);
                if (received_value.get_int()) {
                    if (callback) {
                        callback(Result::Success, received_value.get_int().value());
                    }
                } else {
                    LogErr() << "Wrong get callback type";
                    if (callback) {
                        callback(Result::WrongType, 0);
                    }
                }
            } else if (std::get<GetParamFloatCallback>(work->callback)) {
                const auto& callback = std::get<GetParamFloatCallback>(work->callback);
                if (received_value.get_float()) {
                    if (callback) {
                        callback(Result::Success, received_value.get_float().value());
                    }
                } else {
                    LogErr() << "Wrong get callback type";
                    if (callback) {
                        callback(Result::WrongType, NAN);
                    }
                }
            } else if (std::get<GetParamCustomCallback>(work->callback)) {
                const auto& callback = std::get<GetParamCustomCallback>(work->callback);
                if (received_value.get_custom()) {
                    if (callback) {
                        callback(Result::Success, received_value.get_custom().value());
                    }
                } else {
                    LogErr() << "Wrong get callback type";
                    if (callback) {
                        callback(Result::WrongType, {});
                    }
                }
            }
            _timeout_handler.remove(_timeout_cookie);
            // LogDebug() << "time taken: " <<
            // _sender.get_time().elapsed_since_s(_last_request_time);
            work_queue_guard.pop_front();
        } break;
        case WorkItem::Type::Set: {
            // We are done, inform caller and go back to idle
            if (std::get_if<SetParamCallback>(&work->callback)) {
                const auto& callback = std::get<SetParamCallback>(work->callback);
                if (callback) {
                    callback(MavlinkParameterSender::Result::Success);
                }
            }

            _timeout_handler.remove(_timeout_cookie);
            // LogDebug() << "time taken: " <<
            // _sender.get_time().elapsed_since_s(_last_request_time);
            work_queue_guard.pop_front();
        } break;
        default:
            break;
    }
}

void MavlinkParameterSender::process_param_ext_value(const mavlink_message_t& message)
{
    if (_parameter_debugging) {
        LogDebug() << "getting param ext value";
    }

    mavlink_param_ext_value_t param_ext_value{};
    mavlink_msg_param_ext_value_decode(&message, &param_ext_value);

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (!work->already_requested) {
        return;
    }

    if (work->param_name != extract_safe_param_id(param_ext_value.param_id)) {
        return;
    }

    switch (work->type) {
        case WorkItem::Type::Get: {
            ParamValue value;
            value.set_from_mavlink_param_ext_value(param_ext_value);

            if (value.is_same_type(work->param_value)) {
                if (std::get_if<GetParamFloatCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamFloatCallback>(work->callback);
                    if (callback) {
                        if (value.get_float()) {
                            callback(
                                MavlinkParameterSender::Result::Success, value.get_float().value());
                        } else {
                            callback(MavlinkParameterSender::Result::WrongType, NAN);
                        }
                    }
                } else if (std::get_if<GetParamIntCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamIntCallback>(work->callback);
                    if (callback) {
                        if (value.get_int()) {
                            callback(
                                MavlinkParameterSender::Result::Success, value.get_int().value());
                        } else {
                            callback(MavlinkParameterSender::Result::WrongType, 0);
                        }
                    }
                } else if (std::get_if<GetParamCustomCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamCustomCallback>(work->callback);
                    if (callback) {
                        if (value.get_custom()) {
                            callback(
                                MavlinkParameterSender::Result::Success,
                                value.get_custom().value());
                        } else {
                            callback(MavlinkParameterSender::Result::WrongType, 0);
                        }
                    }
                } else if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::Success, value);
                    }
                }
            } else {
                LogErr() << "Param types don't match for " << work->param_name;

                if (std::get_if<GetParamFloatCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamFloatCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::WrongType, NAN);
                    }
                } else if (std::get_if<GetParamIntCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamIntCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::WrongType, 0);
                    }
                } else if (std::get_if<GetParamCustomCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamCustomCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::WrongType, {});
                    }
                } else if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::WrongType, ParamValue{});
                    }
                }
            }

            _timeout_handler.remove(_timeout_cookie);
            // LogDebug() << "time taken: " <<
            // _sender.get_time().elapsed_since_s(_last_request_time);
            work_queue_guard.pop_front();
        } break;

        case WorkItem::Type::Set:
            LogWarn() << "Unexpected ParamExtValue response";
            break;
        default:
            break;
    }
}

void MavlinkParameterSender::process_param_ext_ack(const mavlink_message_t& message)
{
    // LogDebug() << "getting param ext ack";

    mavlink_param_ext_ack_t param_ext_ack;
    mavlink_msg_param_ext_ack_decode(&message, &param_ext_ack);

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (!work->already_requested) {
        return;
    }

    // Now it still needs to match the param name
    if (work->param_name != extract_safe_param_id(param_ext_ack.param_id)) {
        return;
    }

    switch (work->type) {
        case WorkItem::Type::Get: {
            LogWarn() << "Unexpected ParamExtAck response.";
        } break;

        case WorkItem::Type::Set: {
            if (param_ext_ack.param_result == PARAM_ACK_ACCEPTED) {
                // We are done, inform caller and go back to idle
                if (std::get_if<SetParamCallback>(&work->callback)) {
                    const auto& callback = std::get<SetParamCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::Success);
                    }
                }

                _timeout_handler.remove(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _sender.get_time().elapsed_since_s(_last_request_time);
                work_queue_guard.pop_front();

            } else if (param_ext_ack.param_result == PARAM_ACK_IN_PROGRESS) {
                // Reset timeout and wait again.
                _timeout_handler.refresh(_timeout_cookie);

            } else {
                LogErr() << "Somehow we did not get an ack, we got: "
                         << int(param_ext_ack.param_result);

                if (std::get_if<SetParamCallback>(&work->callback)) {
                    const auto& callback = std::get<SetParamCallback>(work->callback);
                    if (callback) {
                        auto result = [&]() {
                            switch (param_ext_ack.param_result) {
                                case PARAM_ACK_FAILED:
                                    return MavlinkParameterSender::Result::Failed;
                                case PARAM_ACK_VALUE_UNSUPPORTED:
                                    return MavlinkParameterSender::Result::ValueUnsupported;
                                default:
                                    return MavlinkParameterSender::Result::UnknownError;
                            }
                        }();
                        callback(result);
                    }
                }

                _timeout_handler.remove(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _sender.get_time().elapsed_since_s(_last_request_time);
                work_queue_guard.pop_front();
            }
        } break;
        default:
            break;
    }
}

void MavlinkParameterSender::receive_timeout()
{
    {
        std::unique_lock<std::mutex> lock(_all_params_mutex);
        // first check if we are waiting for param list response
        if (_all_params_callback) {
            lock.unlock();
            _all_params_callback({});
            return;
        }
    }

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        LogErr() << "Received timeout without work";
        return;
    }

    if (!work->already_requested) {
        return;
    }

    switch (work->type) {
        case WorkItem::Type::Get: {
            ParamValue empty_value;
            if (work->retries_to_do > 0) {
                // We're not sure the command arrived, let's retransmit.
                LogWarn() << "sending again, retries to do: " << work->retries_to_do << "  ("
                          << work->param_name << ").";
                if (!_sender.send_message(work->mavlink_message)) {
                    LogErr() << "connection send error in retransmit (" << work->param_name << ").";
                    work_queue_guard.pop_front();

                    if (std::get_if<GetParamFloatCallback>(&work->callback)) {
                        const auto& callback = std::get<GetParamFloatCallback>(work->callback);
                        if (callback) {
                            callback(Result::ConnectionError, NAN);
                        }
                    } else if (std::get_if<GetParamIntCallback>(&work->callback)) {
                        const auto& callback = std::get<GetParamIntCallback>(work->callback);
                        if (callback) {
                            callback(Result::ConnectionError, 0);
                        }
                    } else if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                        const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                        if (callback) {
                            callback(Result::ConnectionError, ParamValue{});
                        }
                    }
                } else {
                    --work->retries_to_do;
                    _timeout_handler.add(
                        [this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);
                }
            } else {
                // We have tried retransmitting, giving up now.
                LogErr() << "Error: Retrying failed get param busy timeout: " << work->param_name;

                work_queue_guard.pop_front();

                if (std::get_if<GetParamFloatCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamFloatCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::Timeout, NAN);
                    }
                } else if (std::get_if<GetParamIntCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamIntCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::Timeout, 0);
                    }
                }
            }
        } break;
        case WorkItem::Type::Set: {
            if (work->retries_to_do > 0) {
                // We're not sure the command arrived, let's retransmit.
                LogWarn() << "sending again, retries to do: " << work->retries_to_do << "  ("
                          << work->param_name << ").";
                if (!_sender.send_message(work->mavlink_message)) {
                    LogErr() << "connection send error in retransmit (" << work->param_name << ").";
                    work_queue_guard.pop_front();
                    if (std::get_if<SetParamCallback>(&work->callback)) {
                        const auto& callback = std::get<SetParamCallback>(work->callback);
                        if (callback) {
                            callback(MavlinkParameterSender::Result::ConnectionError);
                        }
                    }
                } else {
                    --work->retries_to_do;
                    _timeout_handler.add(
                        [this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);
                }
            } else {
                // We have tried retransmitting, giving up now.
                LogErr() << "Error: Retrying failed get param busy timeout: " << work->param_name;

                work_queue_guard.pop_front();
                if (std::get_if<SetParamCallback>(&work->callback)) {
                    const auto& callback = std::get<SetParamCallback>(work->callback);
                    if (callback) {
                        callback(MavlinkParameterSender::Result::Timeout);
                    }
                }
            }
        } break;
        default:
            break;
    }
}

std::string MavlinkParameterSender::extract_safe_param_id(const char param_id[])
{
    // The param_id field of the MAVLink struct has length 16 and is not 0 terminated.
    // Therefore, we make a 0 terminated copy first.
    char param_id_long_enough[PARAM_ID_LEN + 1] = {};
    std::memcpy(param_id_long_enough, param_id, PARAM_ID_LEN);
    return {param_id_long_enough};
}

std::ostream& operator<<(std::ostream& str, const MavlinkParameterSender::Result& result)
{
    switch (result) {
        case MavlinkParameterSender::Result::Success:
            return str << "Success";
        case MavlinkParameterSender::Result::Timeout:
            return str << "Timeout";
        case MavlinkParameterSender::Result::ConnectionError:
            return str << "ConnectionError";
        case MavlinkParameterSender::Result::WrongType:
            return str << "WrongType";
        case MavlinkParameterSender::Result::ParamNameTooLong:
            return str << "ParamNameTooLong";
        case MavlinkParameterSender::Result::NotFound:
            return str << "NotFound";
        case MavlinkParameterSender::Result::ValueUnsupported:
            return str << "ValueUnsupported";
        case MavlinkParameterSender::Result::Failed:
            return str << "Failed";
        case MavlinkParameterSender::Result::UnknownError:
            // Fallthrough
        default:
            return str << "UnknownError";
    }
}

} // namespace mavsdk
