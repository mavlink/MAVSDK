#include "mavlink_parameters.h"
#include "system_impl.h"
#include <cstring>
#include <future>

namespace mavsdk {

MAVLinkParameters::MAVLinkParameters(SystemImpl& parent) : _parent(parent)
{
    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_VALUE,
        [this](const mavlink_message_t& message) { process_param_value(message); },
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_SET,
        [this](const mavlink_message_t& message) { process_param_set(message); },
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_EXT_VALUE,
        [this](const mavlink_message_t& message) { process_param_ext_value(message); },
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_EXT_ACK,
        [this](const mavlink_message_t& message) { process_param_ext_ack(message); },
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_EXT_SET,
        [this](const mavlink_message_t& message) { process_param_ext_set(message); },
        this);

    // Parameter Server Callbacks
    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_REQUEST_READ,
        [this](const mavlink_message_t& message) { process_param_request_read(message); },
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_REQUEST_LIST,
        [this](const mavlink_message_t& message) { process_param_request_list(message); },
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_EXT_REQUEST_READ,
        [this](const mavlink_message_t& message) { process_param_ext_request_read(message); },
        this);
}

MAVLinkParameters::~MAVLinkParameters()
{
    _parent.unregister_all_mavlink_message_handlers(this);
}

void MAVLinkParameters::provide_server_param(const std::string& name, const ParamValue& value)
{
    _param_server_store.insert_or_assign(name, value);
}

void MAVLinkParameters::set_param_async(
    const std::string& name,
    const ParamValue& value,
    const set_param_callback_t& callback,
    const void* cookie,
    bool extended)
{
    // if (value.is_float()) {
    //     LogDebug() << "setting param " << name << " to " << value.get_float();
    // } else {
    //     LogDebug() << "setting param " << name << " to " << value.get_int();
    // }

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(Result::ParamNameTooLong);
        }
        return;
    }

    auto new_work = std::make_shared<WorkItem>(_parent.timeout_s());
    new_work->type = WorkItem::Type::Set;
    new_work->set_param_callback = callback;
    new_work->param_name = name;
    new_work->param_value = value;
    new_work->extended = extended;
    new_work->cookie = cookie;

    _work_queue.push_back(new_work);
}

MAVLinkParameters::Result
MAVLinkParameters::set_param(const std::string& name, const ParamValue& value, bool extended)
{
    auto prom = std::promise<Result>();
    auto res = prom.get_future();

    set_param_async(
        name, value, [&prom](Result result) { prom.set_value(result); }, this, extended);

    return res.get();
}

void MAVLinkParameters::get_param_async(
    const std::string& name,
    ParamValue value_type,
    const get_param_callback_t& callback,
    const void* cookie,
    bool extended)
{
    // LogDebug() << "getting param " << name << ", extended: " << (extended ? "yes" : "no");

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            ParamValue empty_param;
            callback(MAVLinkParameters::Result::ParamNameTooLong, empty_param);
        }
        return;
    }

    // Otherwise, push work onto queue.
    auto new_work = std::make_shared<WorkItem>(_parent.timeout_s());
    new_work->type = WorkItem::Type::Get;
    new_work->get_param_callback = callback;
    new_work->param_name = name;
    new_work->param_value = value_type;
    new_work->extended = extended;
    new_work->cookie = cookie;

    _work_queue.push_back(new_work);
}

std::map<std::string, MAVLinkParameters::ParamValue> MAVLinkParameters::retrieve_all_server_params()
{
    return _param_server_store;
}

std::pair<MAVLinkParameters::Result, MAVLinkParameters::ParamValue>
MAVLinkParameters::retrieve_server_param(const std::string& name, ParamValue value_type)
{
    if (_param_server_store.find(name) != _param_server_store.end()) {
        auto value = _param_server_store.at(name);
        if (value.is_same_type(value_type))
            return {MAVLinkParameters::Result::Success, value};
        else
            return {MAVLinkParameters::Result::WrongType, {}};
    }
    return {MAVLinkParameters::Result::NotFound, {}};
}

std::pair<MAVLinkParameters::Result, MAVLinkParameters::ParamValue>
MAVLinkParameters::get_param(const std::string& name, ParamValue value_type, bool extended)
{
    auto prom = std::promise<std::pair<Result, MAVLinkParameters::ParamValue>>();
    auto res = prom.get_future();

    get_param_async(
        name,
        value_type,
        [&prom](Result result, ParamValue value) {
            prom.set_value(std::make_pair<>(result, value));
        },
        this,
        extended);

    return res.get();
}

void MAVLinkParameters::get_all_params_async(const get_all_params_callback_t& callback)
{
    _all_param_store = std::make_shared<AllParameters>();

    _all_param_store->callback = callback;

    mavlink_message_t msg;

    mavlink_msg_param_request_list_pack(
        _parent.get_own_system_id(),
        _parent.get_own_component_id(),
        &msg,
        _parent.get_system_id(),
        _parent.get_autopilot_id());

    if (!_parent.send_message(msg)) {
        LogErr() << "Failed to send param list request!";
        callback(std::map<std::string, ParamValue>{});
        _all_param_store = nullptr;
    }

    _parent.register_timeout_handler(
        [this] { receive_timeout(); }, _parent.timeout_s(), &_all_param_store->timeout_cookie);
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

    switch (work->type) {
        case WorkItem::Type::Set: {
            if (work->extended) {
                char param_value_buf[128] = {};
                work->param_value.get_128_bytes(param_value_buf);

                // FIXME: extended currently always go to the camera component
                mavlink_msg_param_ext_set_pack(
                    _parent.get_own_system_id(),
                    _parent.get_own_component_id(),
                    &work->mavlink_message,
                    _parent.get_system_id(),
                    MAV_COMP_ID_CAMERA,
                    param_id,
                    param_value_buf,
                    work->param_value.get_mav_param_ext_type());
            } else {
                // Param set is intended for Autopilot only.
                mavlink_msg_param_set_pack(
                    _parent.get_own_system_id(),
                    _parent.get_own_component_id(),
                    &work->mavlink_message,
                    _parent.get_system_id(),
                    _parent.get_autopilot_id(),
                    param_id,
                    work->param_value.get_4_float_bytes(),
                    work->param_value.get_mav_param_type());
            }

            if (!_parent.send_message(work->mavlink_message)) {
                LogErr() << "Error: Send message failed";
                if (work->set_param_callback) {
                    work->set_param_callback(MAVLinkParameters::Result::ConnectionError);
                }
                work_queue_guard.pop_front();
                return;
            }

            work->already_requested = true;
            // _last_request_time = _parent.get_time().steady_time();

            // We want to get notified if a timeout happens
            _parent.register_timeout_handler(
                [this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);

        } break;

        case WorkItem::Type::Get: {
            // LogDebug() << "now getting: " << work->param_name;
            if (work->extended) {
                mavlink_msg_param_ext_request_read_pack(
                    _parent.get_own_system_id(),
                    _parent.get_own_component_id(),
                    &work->mavlink_message,
                    _parent.get_system_id(),
                    MAV_COMP_ID_CAMERA,
                    param_id,
                    -1);

            } else {
                // LogDebug() << "request read: "
                //    << (int)_parent.get_own_system_id() << ":"
                //    << (int)_parent.get_own_component_id() <<
                //    " to "
                //    << (int)_parent.get_system_id() << ":"
                //    << (int)_parent.get_autopilot_id();

                mavlink_msg_param_request_read_pack(
                    _parent.get_own_system_id(),
                    _parent.get_own_component_id(),
                    &work->mavlink_message,
                    _parent.get_system_id(),
                    _parent.get_autopilot_id(),
                    param_id,
                    -1);
            }

            if (!_parent.send_message(work->mavlink_message)) {
                LogErr() << "Error: Send message failed";
                if (work->get_param_callback) {
                    ParamValue empty_param;
                    work->get_param_callback(
                        MAVLinkParameters::Result::ConnectionError, empty_param);
                }
                work_queue_guard.pop_front();
                return;
            }

            work->already_requested = true;

            // _last_request_time = _parent.get_time().steady_time();

            // We want to get notified if a timeout happens
            _parent.register_timeout_handler(
                [this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);

        } break;

        case WorkItem::Type::Value: {
            if (work->extended) {
                std::array<char, 128> buf;
                work->param_value.get_128_bytes(buf.data());
                mavlink_msg_param_ext_value_pack(
                    _parent.get_own_system_id(),
                    _parent.get_own_component_id(),
                    &work->mavlink_message,
                    param_id,
                    buf.data(),
                    work->param_value.get_mav_param_ext_type(),
                    work->param_count,
                    work->param_index);
            } else {
                mavlink_msg_param_value_pack(
                    _parent.get_own_system_id(),
                    _parent.get_own_component_id(),
                    &work->mavlink_message,
                    param_id,
                    work->param_value.get_4_float_bytes(),
                    work->param_value.get_mav_param_type(),
                    work->param_count,
                    work->param_index);
            }

            if (!_parent.send_message(work->mavlink_message)) {
                LogErr() << "Error: Send message failed";
                work_queue_guard.pop_front();
                return;
            }

            // As we're a server in this case we don't need any response
            work_queue_guard.pop_front();
        } break;

        case WorkItem::Type::Ack: {
            if (work->extended) {
                std::array<char, 128> buf;
                work->param_value.get_128_bytes(buf.data());
                mavlink_msg_param_ext_ack_pack(
                    _parent.get_own_system_id(),
                    _parent.get_own_component_id(),
                    &work->mavlink_message,
                    param_id,
                    buf.data(),
                    work->param_value.get_mav_param_ext_type(),
                    PARAM_ACK_ACCEPTED);
            }

            if (!work->extended || !_parent.send_message(work->mavlink_message)) {
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

    // LogDebug() << "getting param value: " << extract_safe_param_id(param_value.param_id);

    // check if we are looking for param list
    if (_all_param_store) {
        ParamValue value;
        value.set_from_mavlink_param_value(param_value);

        std::string param_id = extract_safe_param_id(param_value.param_id);

        _all_param_store->all_params.insert(std::pair<std::string, ParamValue>(param_id, value));

        if (param_value.param_index + 1 == param_value.param_count) {
            _all_param_store->callback(_all_param_store->all_params);
            _all_param_store = nullptr;
        } else {
            _parent.unregister_timeout_handler(_all_param_store->timeout_cookie);

            _parent.register_timeout_handler(
                [this] { receive_timeout(); },
                _parent.timeout_s(),
                &_all_param_store->timeout_cookie);
        }

        return;
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
            ParamValue value;
            value.set_from_mavlink_param_value(param_value);
            if (value.is_same_type(work->param_value)) {
                if (work->get_param_callback) {
                    work->get_param_callback(MAVLinkParameters::Result::Success, value);
                }
            } else {
                LogErr() << "Param types don't match";
                ParamValue no_value;
                if (work->get_param_callback) {
                    work->get_param_callback(MAVLinkParameters::Result::WrongType, no_value);
                }
            }
            _parent.unregister_timeout_handler(_timeout_cookie);
            // LogDebug() << "time taken: " <<
            // _parent.get_time().elapsed_since_s(_last_request_time);
            work_queue_guard.pop_front();
        } break;
        case WorkItem::Type::Set: {
            // We are done, inform caller and go back to idle
            if (work->set_param_callback) {
                work->set_param_callback(MAVLinkParameters::Result::Success);
            }

            _parent.unregister_timeout_handler(_timeout_cookie);
            // LogDebug() << "time taken: " <<
            // _parent.get_time().elapsed_since_s(_last_request_time);
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
        value.set_from_mavlink_param_value(param_value);
        if (!subscription.any_type && !subscription.value_type.is_same_type(value)) {
            LogErr() << "Received wrong param type in subscription for " << subscription.param_name;
            continue;
        }

        subscription.callback(value);
    }
}

void MAVLinkParameters::process_param_ext_value(const mavlink_message_t& message)
{
    // LogDebug() << "getting param ext value";

    mavlink_param_ext_value_t param_ext_value;
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
                if (work->get_param_callback) {
                    work->get_param_callback(MAVLinkParameters::Result::Success, value);
                }
            } else {
                LogErr() << "Param types don't match for " << work->param_name;
                ParamValue no_value;
                if (work->get_param_callback) {
                    work->get_param_callback(MAVLinkParameters::Result::WrongType, no_value);
                }
            }

            _parent.unregister_timeout_handler(_timeout_cookie);
            // LogDebug() << "time taken: " <<
            // _parent.get_time().elapsed_since_s(_last_request_time);
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
                if (work->set_param_callback) {
                    work->set_param_callback(MAVLinkParameters::Result::Success);
                }

                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                work_queue_guard.pop_front();

            } else if (param_ext_ack.param_result == PARAM_ACK_IN_PROGRESS) {
                // Reset timeout and wait again.
                _parent.refresh_timeout_handler(_timeout_cookie);

            } else {
                LogErr() << "Somehow we did not get an ack, we got: "
                         << int(param_ext_ack.param_result);

                if (work->set_param_callback) {
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
                    work->set_param_callback(result);
                }

                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
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
        // Use the ID
        if (_param_server_store.find(safe_param_id) != _param_server_store.end()) {
            ParamValue value{};
            if (!value.set_from_mavlink_param_ext_set(set_request)) {
                LogWarn() << "Invalid Param Ext Set Request: " << safe_param_id;
                return;
            }
            _param_server_store.at(safe_param_id) = value;
            auto new_work = std::make_shared<WorkItem>(_parent.timeout_s());
            new_work->type = WorkItem::Type::Ack;
            new_work->param_name = safe_param_id;
            new_work->param_value = _param_server_store.at(safe_param_id);
            new_work->extended = true;
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
            LogDebug() << "Missing Param: " << safe_param_id;
        }
    } else {
        LogWarn() << "Invalid Param Ext Set ID Request: " << safe_param_id;
    }
}
void MAVLinkParameters::receive_timeout()
{
    // first check if we are waiting for param list response
    if (_all_param_store) {
        std::lock_guard<std::mutex> lock(_all_param_mutex);
        _all_param_store->callback(std::map<std::string, ParamValue>{});
        _all_param_store = nullptr; // stop waiting, failed!
        return;
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
                if (!_parent.send_message(work->mavlink_message)) {
                    LogErr() << "connection send error in retransmit (" << work->param_name << ").";
                    work_queue_guard.pop_front();
                    work->get_param_callback(
                        MAVLinkParameters::Result::ConnectionError, empty_value);
                } else {
                    --work->retries_to_do;
                    _parent.register_timeout_handler(
                        [this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);
                }
            } else {
                // We have tried retransmitting, giving up now.
                LogErr() << "Error: Retrying failed get param busy timeout: " << work->param_name;

                work_queue_guard.pop_front();

                work->get_param_callback(MAVLinkParameters::Result::Timeout, empty_value);
            }
        } break;
        case WorkItem::Type::Set: {
            if (work->retries_to_do > 0) {
                // We're not sure the command arrived, let's retransmit.
                LogWarn() << "sending again, retries to do: " << work->retries_to_do << "  ("
                          << work->param_name << ").";
                if (!_parent.send_message(work->mavlink_message)) {
                    LogErr() << "connection send error in retransmit (" << work->param_name << ").";
                    work_queue_guard.pop_front();
                    work->set_param_callback(MAVLinkParameters::Result::ConnectionError);
                } else {
                    --work->retries_to_do;
                    _parent.register_timeout_handler(
                        [this] { receive_timeout(); }, work->timeout_s, &_timeout_cookie);
                }
            } else {
                // We have tried retransmitting, giving up now.
                LogErr() << "Error: Retrying failed get param busy timeout: " << work->param_name;

                work_queue_guard.pop_front();
                work->set_param_callback(MAVLinkParameters::Result::Timeout);
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
        LogDebug() << "Set Param Request: " << safe_param_id;
        // Use the ID
        if (_param_server_store.find(safe_param_id) != _param_server_store.end()) {
            ParamValue value{};
            if (!value.set_from_mavlink_param_set(set_request)) {
                LogWarn() << "Invalid Param Set Request: " << safe_param_id;
                return;
            }
            _param_server_store.at(safe_param_id) = value;
            auto new_work = std::make_shared<WorkItem>(_parent.timeout_s());
            new_work->type = WorkItem::Type::Value;
            new_work->param_name = safe_param_id;
            new_work->param_value = _param_server_store.at(safe_param_id);
            new_work->extended = false;
            _work_queue.push_back(new_work);
        } else {
            LogDebug() << "Missing Param: " << safe_param_id;
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
        if (_param_server_store.find(safe_param_id) != _param_server_store.end()) {
            auto new_work = std::make_shared<WorkItem>(_parent.timeout_s());
            new_work->type = WorkItem::Type::Value;
            new_work->param_name = safe_param_id;
            new_work->param_value = _param_server_store.at(safe_param_id);
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
    for (const auto& pair : _param_server_store) {
        auto new_work = std::make_shared<WorkItem>(_parent.timeout_s());
        new_work->type = WorkItem::Type::Value;
        new_work->param_name = pair.first;
        new_work->param_value = pair.second;
        new_work->extended = false;
        new_work->param_count = static_cast<int>(_param_server_store.size());
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
        if (_param_server_store.find(safe_param_id) != _param_server_store.end()) {
            auto new_work = std::make_shared<WorkItem>(_parent.timeout_s());
            new_work->type = WorkItem::Type::Value;
            new_work->param_name = safe_param_id;
            new_work->param_value = _param_server_store.at(safe_param_id);
            new_work->extended = true;
            _work_queue.push_back(new_work);
        } else {
            LogDebug() << "Missing Param " << safe_param_id;
        }
    }
}

bool MAVLinkParameters::ParamValue::set_from_mavlink_param_value(
    const mavlink_param_value_t& mavlink_value)
{
    union {
        float float_value;
        int32_t int32_value;
    } temp{};

    temp.float_value = mavlink_value.param_value;
    switch (mavlink_value.param_type) {
        case MAV_PARAM_TYPE_UINT32:
        // FALLTHROUGH
        case MAV_PARAM_TYPE_INT32:
            _value = temp.int32_value;
            break;
        case MAV_PARAM_TYPE_REAL32:
            _value = temp.float_value;
            break;
        default:
            // This would be worrying
            LogErr() << "Error: unknown mavlink param type";
            return false;
    }
    return true;
}

bool MAVLinkParameters::ParamValue::set_from_mavlink_param_set(
    const mavlink_param_set_t& mavlink_set)
{
    mavlink_param_value_t mavlink_value{};
    mavlink_value.param_value = mavlink_set.param_value;
    mavlink_value.param_type = mavlink_set.param_type;
    return set_from_mavlink_param_value(mavlink_value);
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
        case MAV_PARAM_EXT_TYPE_CUSTOM:
            LogErr() << "EXT_TYPE_CUSTOM is not supported";
            return false;
        default:
            // This would be worrying
            LogErr() << "Error: unknown mavlink ext param type";
            assert(false);
            return false;
    }
    return true;
}

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
        case MAV_PARAM_EXT_TYPE_CUSTOM:
            LogErr() << "EXT_TYPE_CUSTOM is not supported";
            return false;
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
        uint8_t temp = std::stoi(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "int8") == 0) {
        int8_t temp = std::stoi(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "uint16") == 0) {
        uint16_t temp = std::stoi(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "int16") == 0) {
        int16_t temp = std::stoi(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "uint32") == 0) {
        uint32_t temp = std::stoi(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "int32") == 0) {
        int32_t temp = std::stoi(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "uint64") == 0) {
        uint64_t temp = std::stoll(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "int64") == 0) {
        int64_t temp = std::stoll(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "float") == 0) {
        float temp = std::stof(value_str);
        _value = temp;
    } else if (strcmp(type_str.c_str(), "double") == 0) {
        double temp = std::stod(value_str);
        _value = temp;
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
    if (std::get_if<float>(&_value)) {
        return MAV_PARAM_TYPE_REAL32;
    } else if (std::get_if<int32_t>(&_value)) {
        return MAV_PARAM_TYPE_INT32;
    } else {
        LogErr() << "Unknown param type sent";
        return MAV_PARAM_TYPE_REAL32;
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

[[nodiscard]] float MAVLinkParameters::ParamValue::get_4_float_bytes() const
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

void MAVLinkParameters::ParamValue::get_128_bytes(char* bytes) const
{
    if (std::get_if<uint8_t>(&_value)) {
        memcpy(bytes, &std::get<uint8_t>(_value), sizeof(uint8_t));
    } else if (std::get_if<int8_t>(&_value)) {
        memcpy(bytes, &std::get<int8_t>(_value), sizeof(int8_t));
    } else if (std::get_if<uint16_t>(&_value)) {
        memcpy(bytes, &std::get<uint16_t>(_value), sizeof(uint16_t));
    } else if (std::get_if<int16_t>(&_value)) {
        memcpy(bytes, &std::get<int16_t>(_value), sizeof(int16_t));
    } else if (std::get_if<uint32_t>(&_value)) {
        memcpy(bytes, &std::get<uint32_t>(_value), sizeof(uint32_t));
    } else if (std::get_if<int32_t>(&_value)) {
        memcpy(bytes, &std::get<int32_t>(_value), sizeof(int32_t));
    } else if (std::get_if<uint64_t>(&_value)) {
        memcpy(bytes, &std::get<uint64_t>(_value), sizeof(uint64_t));
    } else if (std::get_if<int64_t>(&_value)) {
        memcpy(bytes, &std::get<int64_t>(_value), sizeof(int64_t));
    } else if (std::get_if<float>(&_value)) {
        memcpy(bytes, &std::get<float>(_value), sizeof(float));
    } else if (std::get_if<double>(&_value)) {
        memcpy(bytes, &std::get<double>(_value), sizeof(double));
    } else {
        LogErr() << "Unknown type";
        assert(false);
    }
}

[[nodiscard]] std::string MAVLinkParameters::ParamValue::get_string() const
{
    return std::visit([](auto value) { return std::to_string(value); }, _value);
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
        (std::get_if<double>(&_value) && std::get_if<double>(&rhs._value))) {
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
    }
    // FIXME: Added to fix CI error (control reading end of non-void function)
    return "unknown";
}

} // namespace mavsdk
