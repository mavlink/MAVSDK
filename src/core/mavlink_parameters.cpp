#include "mavlink_parameters.h"
#include "system_impl.h"
#include <cstring>
#include <future>

namespace mavsdk {

MAVLinkParameters::MAVLinkParameters(SystemImpl& parent) : _parent(parent)
{
    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_VALUE,
        std::bind(&MAVLinkParameters::process_param_value, this, std::placeholders::_1),
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_EXT_VALUE,
        std::bind(&MAVLinkParameters::process_param_ext_value, this, std::placeholders::_1),
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_EXT_ACK,
        std::bind(&MAVLinkParameters::process_param_ext_ack, this, std::placeholders::_1),
        this);
}

MAVLinkParameters::~MAVLinkParameters()
{
    _parent.unregister_all_mavlink_message_handlers(this);
}

void MAVLinkParameters::set_param_async(
    const std::string& name,
    const ParamValue& value,
    set_param_callback_t callback,
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

    auto new_work = std::make_shared<WorkItem>();
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
    get_param_callback_t callback,
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

    // Otherwise push work onto queue.
    auto new_work = std::make_shared<WorkItem>();
    new_work->type = WorkItem::Type::Get;
    new_work->get_param_callback = callback;
    new_work->param_name = name;
    new_work->param_value = value_type;
    new_work->extended = extended;
    new_work->cookie = cookie;

    _work_queue.push_back(new_work);
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
    ParamValue value_type,
    MAVLinkParameters::ParamChangedCallback callback,
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
            if (it->param_name.compare(name) == 0 && it->cookie == cookie) {
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
    STRNCPY(param_id, work->param_name.c_str(), sizeof(param_id) - 1);

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
                std::bind(&MAVLinkParameters::receive_timeout, this),
                work->timeout_s,
                &_timeout_cookie);

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
                std::bind(&MAVLinkParameters::receive_timeout, this),
                work->timeout_s,
                &_timeout_cookie);

        } break;
    }
}

void MAVLinkParameters::process_param_value(const mavlink_message_t& message)
{
    mavlink_param_value_t param_value;
    mavlink_msg_param_value_decode(&message, &param_value);

    // LogDebug() << "getting param value: " << extract_safe_param_id(param_value.param_id);

    notify_param_subscriptions(param_value);

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (!work->already_requested) {
        return;
    }

    if (work->param_name.compare(extract_safe_param_id(param_value.param_id)) != 0) {
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
    }
}

void MAVLinkParameters::notify_param_subscriptions(const mavlink_param_value_t& param_value)
{
    std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);

    for (const auto& subscription : _param_changed_subscriptions) {
        if (subscription.param_name.compare(extract_safe_param_id(param_value.param_id)) != 0) {
            continue;
        }

        ParamValue value;
        value.set_from_mavlink_param_value(param_value);
        if (!subscription.value_type.is_same_type(value)) {
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

    if (work->param_name.compare(extract_safe_param_id(param_ext_value.param_id)) != 0) {
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
            } else if (value.is_uint8() && work->param_value.is_uint16()) {
                // FIXME: workaround for mismatching type uint8_t which should be uint16_t.
                ParamValue correct_type_value;
                correct_type_value.set_uint16(static_cast<uint16_t>(value.get_uint8()));
                if (work->get_param_callback) {
                    work->get_param_callback(
                        MAVLinkParameters::Result::Success, correct_type_value);
                }
            } else if (value.is_uint8() && work->param_value.is_uint32()) {
                // FIXME: workaround for mismatching type uint8_t which should be uint32_t.
                ParamValue correct_type_value;
                correct_type_value.set_uint32(static_cast<uint32_t>(value.get_uint8()));
                if (work->get_param_callback) {
                    work->get_param_callback(
                        MAVLinkParameters::Result::Success, correct_type_value);
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

        case WorkItem::Type::Set:
            LogWarn() << "Unexpected ParamExtValue response";
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
    if (work->param_name.compare(extract_safe_param_id(param_ext_ack.param_id)) != 0) {
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
                    work->set_param_callback(MAVLinkParameters::Result::Timeout);
                }

                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                work_queue_guard.pop_front();
            }
        } break;
    }
}

void MAVLinkParameters::receive_timeout()
{
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
                        std::bind(&MAVLinkParameters::receive_timeout, this),
                        work->timeout_s,
                        &_timeout_cookie);
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
                        std::bind(&MAVLinkParameters::receive_timeout, this),
                        work->timeout_s,
                        &_timeout_cookie);
                }
            } else {
                // We have tried retransmitting, giving up now.
                LogErr() << "Error: Retrying failed get param busy timeout: " << work->param_name;

                work_queue_guard.pop_front();
                work->set_param_callback(MAVLinkParameters::Result::Timeout);
            }
        } break;
    }
}

std::string MAVLinkParameters::extract_safe_param_id(const char param_id[])
{
    // The param_id field of the MAVLink struct has length 16 and is not 0 terminated.
    // Therefore, we make a 0 terminated copy first.
    char param_id_long_enough[PARAM_ID_LEN + 1] = {};
    std::memcpy(param_id_long_enough, param_id, PARAM_ID_LEN);
    return std::string(param_id_long_enough);
}

std::ostream& operator<<(std::ostream& strm, const MAVLinkParameters::ParamValue& obj)
{
    strm << obj.get_string();
    return strm;
}

} // namespace mavsdk
