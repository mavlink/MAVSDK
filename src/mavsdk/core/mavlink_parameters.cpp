#include "mavlink_parameters.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "system_impl.h"
#include <algorithm>
#include <cstring>
#include <future>

namespace mavsdk {

MAVLinkParameters::MAVLinkParameters(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    TimeoutSCallback timeout_s_callback,
    bool is_server) :
    _sender(sender),
    _message_handler(message_handler),
    _timeout_handler(timeout_handler),
    _timeout_s_callback(timeout_s_callback),
    _is_server(is_server)
{
    if (const char* env_p = std::getenv("MAVSDK_PARAMETER_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Parameter debugging is on.";
            _parameter_debugging = true;
        }
    }

    if (!_is_server) {
        _message_handler.register_one(
            MAVLINK_MSG_ID_PARAM_VALUE,
            [this](const mavlink_message_t& message) { process_param_value(message); },
            this);

        //_message_handler.register_one(
        //    MAVLINK_MSG_ID_PARAM_SET,
        //    [this](const mavlink_message_t& message) { process_param_set(message); },
        //    this);

        _message_handler.register_one(
            MAVLINK_MSG_ID_PARAM_EXT_VALUE,
            [this](const mavlink_message_t& message) { process_param_ext_value(message); },
            this);

        _message_handler.register_one(
            MAVLINK_MSG_ID_PARAM_EXT_ACK,
            [this](const mavlink_message_t& message) { process_param_ext_ack(message); },
            this);
    }

    if (_is_server) {
        _message_handler.register_one(
            MAVLINK_MSG_ID_PARAM_SET,
            [this](const mavlink_message_t& message) { process_param_set(message); },
            this);

        _message_handler.register_one(
            MAVLINK_MSG_ID_PARAM_EXT_SET,
            [this](const mavlink_message_t& message) { process_param_ext_set(message); },
            this);

        // Parameter Server Callbacks
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
    }
}

MAVLinkParameters::~MAVLinkParameters()
{
    _message_handler.unregister_all(this);
}

MAVLinkParameters::Result
MAVLinkParameters::provide_server_param_float(const std::string& name, float value)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        return Result::ParamNameTooLong;
    }

    ParamValue param_value;
    param_value.set(value);
    _all_params.insert_or_assign(name, param_value);
    return Result::Success;
}

MAVLinkParameters::Result
MAVLinkParameters::provide_server_param_int(const std::string& name, int value)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        return Result::ParamNameTooLong;
    }

    ParamValue param_value;
    param_value.set(value);
    _all_params.insert_or_assign(name, param_value);
    return Result::Success;
}

MAVLinkParameters::Result
MAVLinkParameters::provide_server_param_custom(const std::string& name, const std::string& value)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        return Result::ParamNameTooLong;
    }

    if (value.size() > sizeof(mavlink_param_ext_set_t::param_value)) {
        LogErr() << "Error: param value too long";
        return Result::ParamValueTooLong;
    }

    ParamValue param_value;
    param_value.set(value);
    _all_params.insert_or_assign(name, param_value);
    return Result::Success;
}

MAVLinkParameters::Result MAVLinkParameters::set_param(
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

void MAVLinkParameters::set_param_async(
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

void MAVLinkParameters::set_param_int_async(
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

        MAVLinkParameters::ParamValue value_to_set;
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

MAVLinkParameters::Result MAVLinkParameters::set_param_int(
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

void MAVLinkParameters::set_param_float_async(
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

    MAVLinkParameters::ParamValue value_to_set;
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

MAVLinkParameters::Result MAVLinkParameters::set_param_float(
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

void MAVLinkParameters::get_param_float_async(
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
            callback(MAVLinkParameters::Result::ParamNameTooLong, NAN);
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

void MAVLinkParameters::get_param_async(
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

void MAVLinkParameters::get_param_int_async(
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
            callback(MAVLinkParameters::Result::ParamNameTooLong, 0);
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

MAVLinkParameters::Result
MAVLinkParameters::set_param_custom(const std::string& name, const std::string& value)
{
    auto prom = std::promise<Result>();
    auto res = prom.get_future();

    set_param_custom_async(
        name, value, [&prom](Result result) { prom.set_value(result); }, this);

    return res.get();
}

void MAVLinkParameters::get_param_custom_async(
    const std::string& name, const GetParamCustomCallback& callback, const void* cookie)
{
    if (_parameter_debugging) {
        LogDebug() << "getting param " << name;
    }

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(MAVLinkParameters::Result::ParamNameTooLong, 0);
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

void MAVLinkParameters::set_param_custom_async(
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

    MAVLinkParameters::ParamValue value_to_set;
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

std::map<std::string, MAVLinkParameters::ParamValue> MAVLinkParameters::retrieve_all_server_params()
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    return _all_params;
}

std::pair<MAVLinkParameters::Result, MAVLinkParameters::ParamValue>
MAVLinkParameters::retrieve_server_param(const std::string& name, ParamValue value_type)
{
    if (_all_params.find(name) != _all_params.end()) {
        auto value = _all_params.at(name);
        if (value.is_same_type(value_type))
            return {MAVLinkParameters::Result::Success, value};
        else
            return {MAVLinkParameters::Result::WrongType, {}};
    }
    return {MAVLinkParameters::Result::NotFound, {}};
}

std::pair<MAVLinkParameters::Result, float>
MAVLinkParameters::retrieve_server_param_float(const std::string& name)
{
    if (_all_params.find(name) != _all_params.end()) {
        const auto maybe_value = _all_params.at(name).get_float();
        if (maybe_value)
            return {MAVLinkParameters::Result::Success, maybe_value.value()};
        else
            return {MAVLinkParameters::Result::WrongType, {}};
    }
    return {MAVLinkParameters::Result::NotFound, {}};
}

std::pair<MAVLinkParameters::Result, std::string>
MAVLinkParameters::retrieve_server_param_custom(const std::string& name)
{
    if (_all_params.find(name) != _all_params.end()) {
        const auto maybe_value = _all_params.at(name).get_custom();
        if (maybe_value)
            return {MAVLinkParameters::Result::Success, maybe_value.value()};
        else
            return {MAVLinkParameters::Result::WrongType, {}};
    }
    return {MAVLinkParameters::Result::NotFound, {}};
}

std::pair<MAVLinkParameters::Result, int>
MAVLinkParameters::retrieve_server_param_int(const std::string& name)
{
    if (_all_params.find(name) != _all_params.end()) {
        const auto maybe_value = _all_params.at(name).get_int();
        if (maybe_value)
            return {MAVLinkParameters::Result::Success, maybe_value.value()};
        else
            return {MAVLinkParameters::Result::WrongType, {}};
    }
    return {MAVLinkParameters::Result::NotFound, {}};
}

std::pair<MAVLinkParameters::Result, MAVLinkParameters::ParamValue> MAVLinkParameters::get_param(
    const std::string& name, MAVLinkParameters::ParamValue value, bool extended)
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

std::pair<MAVLinkParameters::Result, int32_t> MAVLinkParameters::get_param_int(
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

std::pair<MAVLinkParameters::Result, float> MAVLinkParameters::get_param_float(
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

std::pair<MAVLinkParameters::Result, std::string>
MAVLinkParameters::get_param_custom(const std::string& name)
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

void MAVLinkParameters::get_all_params_async(const GetAllParamsCallback& callback)
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

std::map<std::string, MAVLinkParameters::ParamValue> MAVLinkParameters::get_all_params()
{
    std::promise<std::map<std::string, ParamValue>> prom;
    auto res = prom.get_future();

    get_all_params_async(
        [&prom](const std::map<std::string, MAVLinkParameters::ParamValue>& all_params) {
            prom.set_value(all_params);
        });

    return res.get();
}

void MAVLinkParameters::cancel_all_param(const void* cookie)
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

void MAVLinkParameters::subscribe_param_changed(
    const std::string& name,
    const MAVLinkParameters::ParamChangedCallback& callback,
    const void* cookie)
{
    std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);

    if (callback != nullptr) {
        ParamChangedSubscription subscription{};
        subscription.param_name = name;
        subscription.callback = callback;
        subscription.cookie = cookie;
        subscription.any_type = true;
        _param_changed_subscriptions.push_back(subscription);

    } else {
        for (auto it = _param_changed_subscriptions.begin();
             it != _param_changed_subscriptions.end();
             /* ++it */) {
            if (it->param_name == name && it->cookie == cookie) {
                it = _param_changed_subscriptions.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void MAVLinkParameters::subscribe_param_changed(
    const std::string& name,
    ParamValue value_type,
    const MAVLinkParameters::ParamChangedCallback& callback,
    const void* cookie)
{
    std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);

    if (callback != nullptr) {
        ParamChangedSubscription subscription{};
        subscription.param_name = name;
        subscription.callback = callback;
        subscription.cookie = cookie;
        subscription.value_type = value_type;
        _param_changed_subscriptions.push_back(subscription);

    } else {
        for (auto it = _param_changed_subscriptions.begin();
             it != _param_changed_subscriptions.end();
             /* ++it */) {
            if (it->param_name == name && it->cookie == cookie) {
                it = _param_changed_subscriptions.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void MAVLinkParameters::do_work()
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
                            callback(MAVLinkParameters::Result::Failed);
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
                        callback(MAVLinkParameters::Result::ConnectionError);
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

        case WorkItem::Type::Value: {
            if (work->extended) {
                auto buf = work->param_value.get_128_bytes();
                mavlink_msg_param_ext_value_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &work->mavlink_message,
                    param_id,
                    buf.data(),
                    work->param_value.get_mav_param_ext_type(),
                    work->param_count,
                    work->param_index);
            } else {
                float param_value;
                if (_sender.autopilot() == SystemImpl::Autopilot::ArduPilot) {
                    param_value = work->param_value.get_4_float_bytes_cast();
                } else {
                    param_value = work->param_value.get_4_float_bytes_bytewise();
                }
                mavlink_msg_param_value_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &work->mavlink_message,
                    param_id,
                    param_value,
                    work->param_value.get_mav_param_type(),
                    work->param_count,
                    work->param_index);
            }

            if (!_sender.send_message(work->mavlink_message)) {
                LogErr() << "Error: Send message failed";
                work_queue_guard.pop_front();
                return;
            }

            // As we're a server in this case we don't need any response
            work_queue_guard.pop_front();
        } break;

        case WorkItem::Type::Ack: {
            if (work->extended) {
                auto buf = work->param_value.get_128_bytes();
                mavlink_msg_param_ext_ack_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &work->mavlink_message,
                    param_id,
                    buf.data(),
                    work->param_value.get_mav_param_ext_type(),
                    PARAM_ACK_ACCEPTED);
            }

            if (!work->extended || !_sender.send_message(work->mavlink_message)) {
                LogErr() << "Error: Send message failed";
                work_queue_guard.pop_front();
                return;
            }

            // As we're a server in this case we don't need any response
            work_queue_guard.pop_front();
        } break;
    }
}

void MAVLinkParameters::process_param_value(const mavlink_message_t& message)
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

    notify_param_subscriptions(param_value);

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
                    callback(MAVLinkParameters::Result::Success);
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

void MAVLinkParameters::notify_param_subscriptions(const mavlink_param_value_t& param_value)
{
    std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);

    for (const auto& subscription : _param_changed_subscriptions) {
        if (subscription.param_name != extract_safe_param_id(param_value.param_id)) {
            continue;
        }

        ParamValue value;

        if (_sender.autopilot() == SystemImpl::Autopilot::ArduPilot) {
            value.set_from_mavlink_param_value_cast(param_value);
        } else {
            value.set_from_mavlink_param_value_bytewise(param_value);
        }

        if (!subscription.any_type && !subscription.value_type.is_same_type(value)) {
            LogErr() << "Received wrong param type in subscription for " << subscription.param_name;
            continue;
        }

        subscription.callback(value);
    }
}

void MAVLinkParameters::process_param_ext_value(const mavlink_message_t& message)
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
                            callback(MAVLinkParameters::Result::Success, value.get_float().value());
                        } else {
                            callback(MAVLinkParameters::Result::WrongType, NAN);
                        }
                    }
                } else if (std::get_if<GetParamIntCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamIntCallback>(work->callback);
                    if (callback) {
                        if (value.get_int()) {
                            callback(MAVLinkParameters::Result::Success, value.get_int().value());
                        } else {
                            callback(MAVLinkParameters::Result::WrongType, 0);
                        }
                    }
                } else if (std::get_if<GetParamCustomCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamCustomCallback>(work->callback);
                    if (callback) {
                        if (value.get_custom()) {
                            callback(
                                MAVLinkParameters::Result::Success, value.get_custom().value());
                        } else {
                            callback(MAVLinkParameters::Result::WrongType, 0);
                        }
                    }
                } else if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                    if (callback) {
                        callback(MAVLinkParameters::Result::Success, value);
                    }
                }
            } else {
                LogErr() << "Param types don't match for " << work->param_name;

                if (std::get_if<GetParamFloatCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamFloatCallback>(work->callback);
                    if (callback) {
                        callback(MAVLinkParameters::Result::WrongType, NAN);
                    }
                } else if (std::get_if<GetParamIntCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamIntCallback>(work->callback);
                    if (callback) {
                        callback(MAVLinkParameters::Result::WrongType, 0);
                    }
                } else if (std::get_if<GetParamCustomCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamCustomCallback>(work->callback);
                    if (callback) {
                        callback(MAVLinkParameters::Result::WrongType, {});
                    }
                } else if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                    if (callback) {
                        callback(MAVLinkParameters::Result::WrongType, ParamValue{});
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

void MAVLinkParameters::process_param_ext_ack(const mavlink_message_t& message)
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
                        callback(MAVLinkParameters::Result::Success);
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
                                    return MAVLinkParameters::Result::Failed;
                                case PARAM_ACK_VALUE_UNSUPPORTED:
                                    return MAVLinkParameters::Result::ValueUnsupported;
                                default:
                                    return MAVLinkParameters::Result::UnknownError;
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

void MAVLinkParameters::process_param_ext_set(const mavlink_message_t& message)
{
    mavlink_param_ext_set_t set_request{};
    mavlink_msg_param_ext_set_decode(&message, &set_request);

    std::string safe_param_id = extract_safe_param_id(set_request.param_id);
    if (!safe_param_id.empty()) {
        LogDebug() << "Set Param Request: " << safe_param_id;
        {
            // Use the ID
            std::lock_guard<std::mutex> lock(_all_params_mutex);
            ParamValue value{};
            if (!value.set_from_mavlink_param_ext_set(set_request)) {
                LogWarn() << "Invalid Param Ext Set Request: " << safe_param_id;
                return;
            }
            _all_params[safe_param_id] = value;
        }

        auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
        new_work->type = WorkItem::Type::Ack;
        new_work->param_name = safe_param_id;
        new_work->param_value = _all_params[safe_param_id];
        new_work->extended = true;
        _work_queue.push_back(new_work);
        std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);

        for (const auto& subscription : _param_changed_subscriptions) {
            if (subscription.param_name != safe_param_id) {
                continue;
            }

            if (!subscription.any_type &&
                !subscription.value_type.is_same_type(new_work->param_value)) {
                LogErr() << "Received wrong param type in subscription for "
                         << subscription.param_name;
                continue;
            }

            subscription.callback(new_work->param_value);
        }
    } else {
        LogWarn() << "Invalid Param Ext Set ID Request: " << safe_param_id;
    }
}
void MAVLinkParameters::receive_timeout()
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
                        callback(MAVLinkParameters::Result::Timeout, NAN);
                    }
                } else if (std::get_if<GetParamIntCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamIntCallback>(work->callback);
                    if (callback) {
                        callback(MAVLinkParameters::Result::Timeout, 0);
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
                            callback(MAVLinkParameters::Result::ConnectionError);
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
                        callback(MAVLinkParameters::Result::Timeout);
                    }
                }
            }
        } break;
        default:
            break;
    }
}

std::string MAVLinkParameters::extract_safe_param_id(const char param_id[])
{
    // The param_id field of the MAVLink struct has length 16 and is not 0 terminated.
    // Therefore, we make a 0 terminated copy first.
    char param_id_long_enough[PARAM_ID_LEN + 1] = {};
    std::memcpy(param_id_long_enough, param_id, PARAM_ID_LEN);
    return {param_id_long_enough};
}

std::ostream& operator<<(std::ostream& strm, const MAVLinkParameters::ParamValue& obj)
{
    strm << obj.get_string();
    return strm;
}

void MAVLinkParameters::process_param_set(const mavlink_message_t& message)
{
    mavlink_param_set_t set_request{};
    mavlink_msg_param_set_decode(&message, &set_request);

    std::string safe_param_id = extract_safe_param_id(set_request.param_id);
    if (!safe_param_id.empty()) {
        LogDebug() << "Set Param Request: " << safe_param_id << " with value "
                   << *(int32_t*)(&set_request.param_value);

        // Use the ID
        if (_all_params.find(safe_param_id) != _all_params.end()) {
            ParamValue value{};
            if (!value.set_from_mavlink_param_set_bytewise(set_request)) {
                LogWarn() << "Invalid Param Set Request: " << safe_param_id;
                return;
            }

            LogDebug() << "Changing param from " << _all_params[safe_param_id] << " to " << value;
            _all_params[safe_param_id] = value;

            auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
            new_work->type = WorkItem::Type::Value;
            new_work->param_name = safe_param_id;
            new_work->param_value = _all_params.at(safe_param_id);
            new_work->extended = false;
            _work_queue.push_back(new_work);
            std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);
            for (const auto& subscription : _param_changed_subscriptions) {
                if (subscription.param_name != safe_param_id) {
                    continue;
                }
                if (!subscription.any_type && !subscription.value_type.is_same_type(value)) {
                    LogErr() << "Received wrong param type in subscription for "
                             << subscription.param_name;
                    continue;
                }
                subscription.callback(value);
            }
        } else {
            LogDebug() << "Missing Param: " << safe_param_id << "(this: " << this << ")";
        }
    } else {
        LogWarn() << "Invalid Param Set ID Request: " << safe_param_id;
    }
}

void MAVLinkParameters::process_param_request_read(const mavlink_message_t& message)
{
    mavlink_param_request_read_t read_request{};
    mavlink_msg_param_request_read_decode(&message, &read_request);

    std::string param_id = extract_safe_param_id(read_request.param_id);

    if (read_request.param_index == -1) {
        auto safe_param_id = extract_safe_param_id(read_request.param_id);
        LogDebug() << "Request Param " << safe_param_id;

        // Use the ID
        if (_all_params.find(safe_param_id) != _all_params.end()) {
            auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
            new_work->type = WorkItem::Type::Value;
            new_work->param_name = safe_param_id;
            new_work->param_value = _all_params.at(safe_param_id);
            new_work->extended = false;
            _work_queue.push_back(new_work);
        } else {
            LogDebug() << "Missing Param " << safe_param_id;
        }
    }
}

void MAVLinkParameters::process_param_request_list(const mavlink_message_t& message)
{
    mavlink_param_request_list_t list_request{};
    mavlink_msg_param_request_list_decode(&message, &list_request);

    auto idx = 0;
    for (const auto& pair : _all_params) {
        auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
        new_work->type = WorkItem::Type::Value;
        new_work->param_name = pair.first;
        new_work->param_value = pair.second;
        new_work->extended = false;
        new_work->param_count = static_cast<int>(_all_params.size());
        new_work->param_index = idx++;
        _work_queue.push_back(new_work);
    }
}

void MAVLinkParameters::process_param_ext_request_read(const mavlink_message_t& message)
{
    mavlink_param_request_read_t read_request{};
    mavlink_msg_param_request_read_decode(&message, &read_request);

    std::string param_id = extract_safe_param_id(read_request.param_id);

    if (read_request.param_index == -1) {
        auto safe_param_id = extract_safe_param_id(read_request.param_id);
        LogDebug() << "Request Param " << safe_param_id;
        // Use the ID
        if (_all_params.find(safe_param_id) != _all_params.end()) {
            auto new_work = std::make_shared<WorkItem>(_timeout_s_callback());
            new_work->type = WorkItem::Type::Value;
            new_work->param_name = safe_param_id;
            new_work->param_value = _all_params.at(safe_param_id);
            new_work->extended = true;
            _work_queue.push_back(new_work);
        } else {
            LogDebug() << "Missing Param " << safe_param_id;
        }
    }
}

bool MAVLinkParameters::ParamValue::set_from_mavlink_param_value_bytewise(
    const mavlink_param_value_t& mavlink_value)
{
    switch (mavlink_value.param_type) {
        case MAV_PARAM_TYPE_UINT8: {
            uint8_t temp;
            memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
            _value = temp;
        } break;

        case MAV_PARAM_TYPE_INT8: {
            int8_t temp;
            memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
            _value = temp;
        } break;

        case MAV_PARAM_TYPE_UINT16: {
            uint16_t temp;
            memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
            _value = temp;
        } break;

        case MAV_PARAM_TYPE_INT16: {
            int16_t temp;
            memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
            _value = temp;
        } break;

        case MAV_PARAM_TYPE_UINT32: {
            uint32_t temp;
            memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
            _value = temp;
        } break;

        case MAV_PARAM_TYPE_INT32: {
            int32_t temp;
            memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
            _value = temp;
        } break;

        case MAV_PARAM_TYPE_REAL32: {
            _value = mavlink_value.param_value;
        } break;

        default:
            // This would be worrying
            LogErr() << "Error: unknown mavlink param type: "
                     << std::to_string(mavlink_value.param_type);
            return false;
    }
    return true;
}

bool MAVLinkParameters::ParamValue::set_from_mavlink_param_value_cast(
    const mavlink_param_value_t& mavlink_value)
{
    switch (mavlink_value.param_type) {
        case MAV_PARAM_TYPE_UINT8: {
            _value = static_cast<uint8_t>(mavlink_value.param_value);
        } break;

        case MAV_PARAM_TYPE_INT8: {
            _value = static_cast<int8_t>(mavlink_value.param_value);
        } break;

        case MAV_PARAM_TYPE_UINT16: {
            _value = static_cast<uint16_t>(mavlink_value.param_value);
        } break;

        case MAV_PARAM_TYPE_INT16: {
            _value = static_cast<int16_t>(mavlink_value.param_value);
        } break;

        case MAV_PARAM_TYPE_UINT32: {
            _value = static_cast<uint32_t>(mavlink_value.param_value);
        } break;

        case MAV_PARAM_TYPE_INT32: {
            _value = static_cast<int32_t>(mavlink_value.param_value);
        } break;

        case MAV_PARAM_TYPE_REAL32: {
            float temp = mavlink_value.param_value;
            _value = temp;
        } break;

        default:
            // This would be worrying
            LogErr() << "Error: unknown mavlink param type: "
                     << std::to_string(mavlink_value.param_type);
            return false;
    }
    return true;
}

bool MAVLinkParameters::ParamValue::set_from_mavlink_param_set_bytewise(
    const mavlink_param_set_t& mavlink_set)
{
    mavlink_param_value_t mavlink_value{};
    mavlink_value.param_value = mavlink_set.param_value;
    mavlink_value.param_type = mavlink_set.param_type;

    return set_from_mavlink_param_value_bytewise(mavlink_value);
}

bool MAVLinkParameters::ParamValue::set_from_mavlink_param_ext_set(
    const mavlink_param_ext_set_t& mavlink_ext_set)
{
    switch (mavlink_ext_set.param_type) {
        case MAV_PARAM_EXT_TYPE_UINT8: {
            uint8_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT8: {
            int8_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_UINT16: {
            uint16_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT16: {
            int16_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_UINT32: {
            uint32_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT32: {
            int32_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_UINT64: {
            uint64_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT64: {
            int64_t temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_REAL32: {
            float temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_REAL64: {
            double temp;
            memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_CUSTOM: {
            std::size_t len = std::min(std::size_t(128), strlen(mavlink_ext_set.param_value));
            _value = std::string(mavlink_ext_set.param_value, mavlink_ext_set.param_value + len);
        } break;
        default:
            // This would be worrying
            LogErr() << "Error: unknown mavlink ext param type";
            assert(false);
            return false;
    }
    return true;
}

// FIXME: this function can collapse with the one above.
bool MAVLinkParameters::ParamValue::set_from_mavlink_param_ext_value(
    const mavlink_param_ext_value_t& mavlink_ext_value)
{
    switch (mavlink_ext_value.param_type) {
        case MAV_PARAM_EXT_TYPE_UINT8: {
            uint8_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT8: {
            int8_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_UINT16: {
            uint16_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT16: {
            int16_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_UINT32: {
            uint32_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT32: {
            int32_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_UINT64: {
            uint64_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_INT64: {
            int64_t temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_REAL32: {
            float temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_REAL64: {
            double temp;
            memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
            _value = temp;
        } break;
        case MAV_PARAM_EXT_TYPE_CUSTOM: {
            std::size_t len = std::min(std::size_t(128), strlen(mavlink_ext_value.param_value));
            _value =
                std::string(mavlink_ext_value.param_value, mavlink_ext_value.param_value + len);
        } break;
        default:
            // This would be worrying
            LogErr() << "Error: unknown mavlink ext param type";
            assert(false);
            return false;
    }
    return true;
}

bool MAVLinkParameters::ParamValue::set_from_xml(
    const std::string& type_str, const std::string& value_str)
{
    if (strcmp(type_str.c_str(), "uint8") == 0) {
        _value = static_cast<uint8_t>(std::stoi(value_str));
    } else if (strcmp(type_str.c_str(), "int8") == 0) {
        _value = static_cast<int8_t>(std::stoi(value_str));
    } else if (strcmp(type_str.c_str(), "uint16") == 0) {
        _value = static_cast<uint16_t>(std::stoi(value_str));
    } else if (strcmp(type_str.c_str(), "int16") == 0) {
        _value = static_cast<int16_t>(std::stoi(value_str));
    } else if (strcmp(type_str.c_str(), "uint32") == 0) {
        _value = static_cast<uint32_t>(std::stoi(value_str));
    } else if (strcmp(type_str.c_str(), "int32") == 0) {
        _value = static_cast<int32_t>(std::stoi(value_str));
    } else if (strcmp(type_str.c_str(), "uint64") == 0) {
        _value = static_cast<uint64_t>(std::stoll(value_str));
    } else if (strcmp(type_str.c_str(), "int64") == 0) {
        _value = static_cast<int64_t>(std::stoll(value_str));
    } else if (strcmp(type_str.c_str(), "float") == 0) {
        _value = static_cast<float>(std::stof(value_str));
    } else if (strcmp(type_str.c_str(), "double") == 0) {
        _value = static_cast<double>(std::stod(value_str));
    } else {
        LogErr() << "Unknown type: " << type_str;
        return false;
    }
    return true;
}

bool MAVLinkParameters::ParamValue::set_empty_type_from_xml(const std::string& type_str)
{
    if (strcmp(type_str.c_str(), "uint8") == 0) {
        _value = uint8_t(0);
    } else if (strcmp(type_str.c_str(), "int8") == 0) {
        _value = int8_t(0);
    } else if (strcmp(type_str.c_str(), "uint16") == 0) {
        _value = uint16_t(0);
    } else if (strcmp(type_str.c_str(), "int16") == 0) {
        _value = int16_t(0);
    } else if (strcmp(type_str.c_str(), "uint32") == 0) {
        _value = uint32_t(0);
    } else if (strcmp(type_str.c_str(), "int32") == 0) {
        _value = int32_t(0);
    } else if (strcmp(type_str.c_str(), "uint64") == 0) {
        _value = uint64_t(0);
    } else if (strcmp(type_str.c_str(), "int64") == 0) {
        _value = int64_t(0);
    } else if (strcmp(type_str.c_str(), "float") == 0) {
        _value = 0.0f;
    } else if (strcmp(type_str.c_str(), "double") == 0) {
        _value = 0.0;
    } else {
        LogErr() << "Unknown type: " << type_str;
        return false;
    }
    return true;
}

[[nodiscard]] MAV_PARAM_TYPE MAVLinkParameters::ParamValue::get_mav_param_type() const
{
    if (std::get_if<uint8_t>(&_value)) {
        return MAV_PARAM_TYPE_UINT8;
    } else if (std::get_if<int8_t>(&_value)) {
        return MAV_PARAM_TYPE_INT8;
    } else if (std::get_if<uint16_t>(&_value)) {
        return MAV_PARAM_TYPE_UINT16;
    } else if (std::get_if<int16_t>(&_value)) {
        return MAV_PARAM_TYPE_INT16;
    } else if (std::get_if<uint32_t>(&_value)) {
        return MAV_PARAM_TYPE_UINT32;
    } else if (std::get_if<int32_t>(&_value)) {
        return MAV_PARAM_TYPE_INT32;
    } else if (std::get_if<uint64_t>(&_value)) {
        return MAV_PARAM_TYPE_UINT64;
    } else if (std::get_if<int64_t>(&_value)) {
        return MAV_PARAM_TYPE_INT64;
    } else if (std::get_if<float>(&_value)) {
        return MAV_PARAM_TYPE_REAL32;
    } else if (std::get_if<double>(&_value)) {
        return MAV_PARAM_TYPE_REAL64;
    } else {
        LogErr() << "Unknown data type for param.";
        assert(false);
        return MAV_PARAM_TYPE_INT32;
    }
}

[[nodiscard]] MAV_PARAM_EXT_TYPE MAVLinkParameters::ParamValue::get_mav_param_ext_type() const
{
    if (std::get_if<uint8_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_UINT8;
    } else if (std::get_if<int8_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_INT8;
    } else if (std::get_if<uint16_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_UINT16;
    } else if (std::get_if<int16_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_INT16;
    } else if (std::get_if<uint32_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_UINT32;
    } else if (std::get_if<int32_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_INT32;
    } else if (std::get_if<uint64_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_UINT64;
    } else if (std::get_if<int64_t>(&_value)) {
        return MAV_PARAM_EXT_TYPE_INT64;
    } else if (std::get_if<float>(&_value)) {
        return MAV_PARAM_EXT_TYPE_REAL32;
    } else if (std::get_if<double>(&_value)) {
        return MAV_PARAM_EXT_TYPE_REAL64;
    } else if (std::get_if<std::string>(&_value)) {
        return MAV_PARAM_EXT_TYPE_CUSTOM;
    } else {
        LogErr() << "Unknown data type for param.";
        assert(false);
        return MAV_PARAM_EXT_TYPE_INT32;
    }
}

bool MAVLinkParameters::ParamValue::set_as_same_type(const std::string& value_str)
{
    if (std::get_if<uint8_t>(&_value)) {
        _value = uint8_t(std::stoi(value_str));
    } else if (std::get_if<int8_t>(&_value)) {
        _value = int8_t(std::stoi(value_str));
    } else if (std::get_if<uint16_t>(&_value)) {
        _value = uint16_t(std::stoi(value_str));
    } else if (std::get_if<int16_t>(&_value)) {
        _value = int16_t(std::stoi(value_str));
    } else if (std::get_if<uint32_t>(&_value)) {
        _value = uint32_t(std::stoi(value_str));
    } else if (std::get_if<int32_t>(&_value)) {
        _value = int32_t(std::stoi(value_str));
    } else if (std::get_if<uint64_t>(&_value)) {
        _value = uint64_t(std::stoll(value_str));
    } else if (std::get_if<int64_t>(&_value)) {
        _value = int64_t(std::stoll(value_str));
    } else if (std::get_if<float>(&_value)) {
        _value = float(std::stof(value_str));
    } else if (std::get_if<double>(&_value)) {
        _value = double(std::stod(value_str));
    } else {
        LogErr() << "Unknown type";
        return false;
    }
    return true;
}

[[nodiscard]] float MAVLinkParameters::ParamValue::get_4_float_bytes_bytewise() const
{
    if (std::get_if<float>(&_value)) {
        return std::get<float>(_value);
    } else if (std::get_if<int32_t>(&_value)) {
        return *(reinterpret_cast<const float*>(&std::get<int32_t>(_value)));
    } else {
        LogErr() << "Unknown type";
        assert(false);
        return NAN;
    }
}

[[nodiscard]] float MAVLinkParameters::ParamValue::get_4_float_bytes_cast() const
{
    if (std::get_if<float>(&_value)) {
        return std::get<float>(_value);
    } else if (std::get_if<uint32_t>(&_value)) {
        return static_cast<float>(std::get<uint32_t>(_value));
    } else if (std::get_if<int32_t>(&_value)) {
        return static_cast<float>(std::get<int32_t>(_value));
    } else if (std::get_if<uint16_t>(&_value)) {
        return static_cast<float>(std::get<uint16_t>(_value));
    } else if (std::get_if<int16_t>(&_value)) {
        return static_cast<float>(std::get<int16_t>(_value));
    } else if (std::get_if<uint8_t>(&_value)) {
        return static_cast<float>(std::get<uint8_t>(_value));
    } else if (std::get_if<int8_t>(&_value)) {
        return static_cast<float>(std::get<int8_t>(_value));
    } else {
        LogErr() << "Unknown type";
        assert(false);
        return NAN;
    }
}

[[nodiscard]] std::optional<int> MAVLinkParameters::ParamValue::get_int() const
{
    if (std::get_if<uint32_t>(&_value)) {
        return static_cast<int>(std::get<uint32_t>(_value));
    } else if (std::get_if<int32_t>(&_value)) {
        return static_cast<int>(std::get<int32_t>(_value));
    } else if (std::get_if<uint16_t>(&_value)) {
        return static_cast<int>(std::get<uint16_t>(_value));
    } else if (std::get_if<int16_t>(&_value)) {
        return static_cast<int>(std::get<int16_t>(_value));
    } else if (std::get_if<uint8_t>(&_value)) {
        return static_cast<int>(std::get<uint8_t>(_value));
    } else if (std::get_if<int8_t>(&_value)) {
        return static_cast<int>(std::get<int8_t>(_value));
    } else {
        LogErr() << "Not int type";
        return {};
    }
}

bool MAVLinkParameters::ParamValue::set_int(int new_value)
{
    if (std::get_if<uint8_t>(&_value)) {
        _value = static_cast<uint8_t>(new_value);
        return true;
    } else if (std::get_if<int8_t>(&_value)) {
        _value = static_cast<int8_t>(new_value);
        return true;
    } else if (std::get_if<uint16_t>(&_value)) {
        _value = static_cast<uint16_t>(new_value);
        return true;
    } else if (std::get_if<int16_t>(&_value)) {
        _value = static_cast<int16_t>(new_value);
        return true;
    } else if (std::get_if<uint32_t>(&_value)) {
        _value = static_cast<uint32_t>(new_value);
        return true;
    } else if (std::get_if<int32_t>(&_value)) {
        _value = static_cast<int32_t>(new_value);
        return true;
    } else {
        return false;
    }
}

void MAVLinkParameters::ParamValue::set_float(float new_value)
{
    _value = new_value;
}

void MAVLinkParameters::ParamValue::set_custom(const std::string& new_value)
{
    _value = new_value;
}

[[nodiscard]] std::optional<float> MAVLinkParameters::ParamValue::get_float() const
{
    if (std::get_if<float>(&_value)) {
        return std::get<float>(_value);
    } else {
        LogErr() << "Not float type";
        return {};
    }
}

[[nodiscard]] std::optional<std::string> MAVLinkParameters::ParamValue::get_custom() const
{
    if (std::get_if<std::string>(&_value)) {
        return std::get<std::string>(_value);
    } else {
        LogErr() << "Not custom type";
        return {};
    }
}

std::array<char, 128> MAVLinkParameters::ParamValue::get_128_bytes() const
{
    std::array<char, 128> bytes{};

    if (std::get_if<uint8_t>(&_value)) {
        memcpy(bytes.data(), &std::get<uint8_t>(_value), sizeof(uint8_t));
    } else if (std::get_if<int8_t>(&_value)) {
        memcpy(bytes.data(), &std::get<int8_t>(_value), sizeof(int8_t));
    } else if (std::get_if<uint16_t>(&_value)) {
        memcpy(bytes.data(), &std::get<uint16_t>(_value), sizeof(uint16_t));
    } else if (std::get_if<int16_t>(&_value)) {
        memcpy(bytes.data(), &std::get<int16_t>(_value), sizeof(int16_t));
    } else if (std::get_if<uint32_t>(&_value)) {
        memcpy(bytes.data(), &std::get<uint32_t>(_value), sizeof(uint32_t));
    } else if (std::get_if<int32_t>(&_value)) {
        memcpy(bytes.data(), &std::get<int32_t>(_value), sizeof(int32_t));
    } else if (std::get_if<uint64_t>(&_value)) {
        memcpy(bytes.data(), &std::get<uint64_t>(_value), sizeof(uint64_t));
    } else if (std::get_if<int64_t>(&_value)) {
        memcpy(bytes.data(), &std::get<int64_t>(_value), sizeof(int64_t));
    } else if (std::get_if<float>(&_value)) {
        memcpy(bytes.data(), &std::get<float>(_value), sizeof(float));
    } else if (std::get_if<double>(&_value)) {
        memcpy(bytes.data(), &std::get<double>(_value), sizeof(double));
    } else if (std::get_if<std::string>(&_value)) {
        auto str_ptr = &std::get<std::string>(_value);
        // Copy all data in string, max 128 bytes
        memcpy(bytes.data(), str_ptr->data(), std::min(bytes.size(), str_ptr->size()));
    } else {
        LogErr() << "Unknown type";
        assert(false);
    }

    return bytes;
}

[[nodiscard]] std::string MAVLinkParameters::ParamValue::get_string() const
{
    return std::visit([](auto value) { return to_string(value); }, _value);
}

[[nodiscard]] bool MAVLinkParameters::ParamValue::is_same_type(const ParamValue& rhs) const
{
    if ((std::get_if<uint8_t>(&_value) && std::get_if<uint8_t>(&rhs._value)) ||
        (std::get_if<int8_t>(&_value) && std::get_if<int8_t>(&rhs._value)) ||
        (std::get_if<uint16_t>(&_value) && std::get_if<uint16_t>(&rhs._value)) ||
        (std::get_if<int16_t>(&_value) && std::get_if<int16_t>(&rhs._value)) ||
        (std::get_if<uint32_t>(&_value) && std::get_if<uint32_t>(&rhs._value)) ||
        (std::get_if<int32_t>(&_value) && std::get_if<int32_t>(&rhs._value)) ||
        (std::get_if<uint64_t>(&_value) && std::get_if<uint64_t>(&rhs._value)) ||
        (std::get_if<int64_t>(&_value) && std::get_if<int64_t>(&rhs._value)) ||
        (std::get_if<float>(&_value) && std::get_if<float>(&rhs._value)) ||
        (std::get_if<double>(&_value) && std::get_if<double>(&rhs._value)) ||
        (std::get_if<std::string>(&_value) && std::get_if<std::string>(&rhs._value))) {
        return true;
    } else {
        LogWarn() << "Comparison type mismatch between " << typestr() << " and " << rhs.typestr();
        return false;
    }
}

bool MAVLinkParameters::ParamValue::operator==(const std::string& value_str) const
{
    // LogDebug() << "Compare " << value_str() << " and " << rhs.value_str();
    if (std::get_if<uint8_t>(&_value)) {
        return std::get<uint8_t>(_value) == std::stoi(value_str);
    } else if (std::get_if<int8_t>(&_value)) {
        return std::get<int8_t>(_value) == std::stoi(value_str);
    } else if (std::get_if<uint16_t>(&_value)) {
        return std::get<uint16_t>(_value) == std::stoi(value_str);
    } else if (std::get_if<int16_t>(&_value)) {
        return std::get<int16_t>(_value) == std::stoi(value_str);
    } else if (std::get_if<uint32_t>(&_value)) {
        return std::get<uint32_t>(_value) == std::stoul(value_str);
    } else if (std::get_if<int32_t>(&_value)) {
        return std::get<int32_t>(_value) == std::stol(value_str);
    } else if (std::get_if<uint64_t>(&_value)) {
        return std::get<uint64_t>(_value) == std::stoull(value_str);
    } else if (std::get_if<int64_t>(&_value)) {
        return std::get<int64_t>(_value) == std::stoll(value_str);
    } else if (std::get_if<float>(&_value)) {
        return std::get<float>(_value) == std::stof(value_str);
    } else if (std::get_if<double>(&_value)) {
        return std::get<double>(_value) == std::stod(value_str);
    } else {
        // This also covers custom_type_t
        return false;
    }
}

[[nodiscard]] std::string MAVLinkParameters::ParamValue::typestr() const
{
    if (std::get_if<uint8_t>(&_value)) {
        return "uint8_t";
    } else if (std::get_if<int8_t>(&_value)) {
        return "int8_t";
    } else if (std::get_if<uint16_t>(&_value)) {
        return "uint16_t";
    } else if (std::get_if<int16_t>(&_value)) {
        return "int16_t";
    } else if (std::get_if<uint32_t>(&_value)) {
        return "uint32_t";
    } else if (std::get_if<int32_t>(&_value)) {
        return "int32_t";
    } else if (std::get_if<uint64_t>(&_value)) {
        return "uint64_t";
    } else if (std::get_if<int64_t>(&_value)) {
        return "int64_t";
    } else if (std::get_if<float>(&_value)) {
        return "float";
    } else if (std::get_if<double>(&_value)) {
        return "double";
    } else if (std::get_if<std::string>(&_value)) {
        return "custom";
    }
    // FIXME: Added to fix CI error (control reading end of non-void function)
    return "unknown";
}

std::ostream& operator<<(std::ostream& str, const MAVLinkParameters::Result& result)
{
    switch (result) {
        case MAVLinkParameters::Result::Success:
            return str << "Success";
        case MAVLinkParameters::Result::Timeout:
            return str << "Timeout";
        case MAVLinkParameters::Result::ConnectionError:
            return str << "ConnectionError";
        case MAVLinkParameters::Result::WrongType:
            return str << "WrongType";
        case MAVLinkParameters::Result::ParamNameTooLong:
            return str << "ParamNameTooLong";
        case MAVLinkParameters::Result::NotFound:
            return str << "NotFound";
        case MAVLinkParameters::Result::ValueUnsupported:
            return str << "ValueUnsupported";
        case MAVLinkParameters::Result::Failed:
            return str << "Failed";
        case MAVLinkParameters::Result::UnknownError:
            // Fallthrough
        default:
            return str << "UnknownError";
    }
}

} // namespace mavsdk
