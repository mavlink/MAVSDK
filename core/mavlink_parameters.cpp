#include "mavlink_parameters.h"
#include "system_impl.h"

namespace dronecode_sdk {

MAVLinkParameters::MAVLinkParameters(SystemImpl &parent) : _parent(parent)
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

void MAVLinkParameters::set_param_async(const std::string &name,
                                        const ParamValue &value,
                                        set_param_callback_t callback,
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
            callback(false);
        }
        return;
    }

    SetParamWork new_work;
    new_work.callback = callback;
    new_work.param_name = name;
    new_work.param_value = value;
    new_work.extended = extended;

    _set_param_queue.push_back(new_work);
}

void MAVLinkParameters::get_param_async(const std::string &name,
                                        get_param_callback_t callback,
                                        bool extended)
{
    // LogDebug() << "getting param " << name << ", extended: " << (extended ? "yes" : "no");

    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            ParamValue empty_param;
            callback(false, empty_param);
        }
        return;
    }

    GetParamWork new_work;
    new_work.callback = callback;
    new_work.param_name = name;
    new_work.extended = extended;

    _get_param_queue.push_back(new_work);
}

void MAVLinkParameters::do_work()
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state != State::NONE) {
        // If we're still busy, let's wait
        return;
    }

    auto set_param_work = _set_param_queue.borrow_front();
    auto get_param_work = _get_param_queue.borrow_front();

    if (!set_param_work && !get_param_work) {
        return;
    }

    // There params to set which we always do first
    if (set_param_work) {
        // We don't need get_param_work and can give it back.
        _get_param_queue.return_front();

        // We need to wait for this param to get sent back as confirmation.
        _state = State::SET_PARAM_BUSY;

        char param_id[PARAM_ID_LEN] = {};
        STRNCPY(param_id, set_param_work->param_name.c_str(), sizeof(param_id) - 1);

        mavlink_message_t message = {};
        if (set_param_work->extended) {
            char param_value_buf[128] = {};
            set_param_work->param_value.get_128_bytes(param_value_buf);

            // FIXME: extended currently always go to the camera component
            mavlink_msg_param_ext_set_pack(GCSClient::system_id,
                                           GCSClient::component_id,
                                           &message,
                                           _parent.get_system_id(),
                                           MAV_COMP_ID_CAMERA,
                                           param_id,
                                           param_value_buf,
                                           set_param_work->param_value.get_mav_param_ext_type());
        } else {
            // Param set is intended for Autopilot only.
            mavlink_msg_param_set_pack(GCSClient::system_id,
                                       GCSClient::component_id,
                                       &message,
                                       _parent.get_system_id(),
                                       _parent.get_autopilot_id(),
                                       param_id,
                                       set_param_work->param_value.get_4_float_bytes(),
                                       set_param_work->param_value.get_mav_param_type());
        }

        if (!_parent.send_message(message)) {
            LogErr() << "Error: Send message failed";
            if (set_param_work->callback) {
                set_param_work->callback(false);
            }
            _state = State::NONE;
            _set_param_queue.pop_front();
            return;
        }

        // _last_request_time = _parent.get_time().steady_time();

        // We want to get notified if a timeout happens
        _parent.register_timeout_handler(
            std::bind(&MAVLinkParameters::receive_timeout, this), 0.5, &_timeout_cookie);

        _set_param_queue.return_front();

    } else {
        _set_param_queue.return_front();

        // The busy flag gets reset when the param comes in
        // or after a timeout.
        _state = State::GET_PARAM_BUSY;

        char param_id[PARAM_ID_LEN] = {};
        STRNCPY(param_id, get_param_work->param_name.c_str(), sizeof(param_id) - 1);

        // LogDebug() << "now getting: " << get_param_work->param_name;

        mavlink_message_t message = {};
        if (get_param_work->extended) {
            mavlink_msg_param_ext_request_read_pack(GCSClient::system_id,
                                                    GCSClient::component_id,
                                                    &message,
                                                    _parent.get_system_id(),
                                                    MAV_COMP_ID_CAMERA,
                                                    param_id,
                                                    -1);

        } else {
            // LogDebug() << "request read: "
            //    << (int)GCSClient::system_id << ":"
            //    << (int)GCSClient::component_id <<
            //    " to "
            //    << (int)_parent.get_system_id() << ":"
            //    << (int)_parent.get_autopilot_id();

            mavlink_msg_param_request_read_pack(GCSClient::system_id,
                                                GCSClient::component_id,
                                                &message,
                                                _parent.get_system_id(),
                                                _parent.get_autopilot_id(),
                                                param_id,
                                                -1);
        }

        if (!_parent.send_message(message)) {
            LogErr() << "Error: Send message failed";
            if (get_param_work->callback) {
                ParamValue empty_param;
                get_param_work->callback(false, empty_param);
            }
            _state = State::NONE;
            _get_param_queue.pop_front();
            return;
        }

        // _last_request_time = _parent.get_time().steady_time();

        // We want to get notified if a timeout happens
        _parent.register_timeout_handler(
            std::bind(&MAVLinkParameters::receive_timeout, this), 0.5, &_timeout_cookie);

        _get_param_queue.return_front();
    }
}

void MAVLinkParameters::process_param_value(const mavlink_message_t &message)
{
    // LogDebug() << "getting param value";

    mavlink_param_value_t param_value;
    mavlink_msg_param_value_decode(&message, &param_value);

    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state == State::NONE) {
        return;
    }

    if (_state == State::GET_PARAM_BUSY) {
        auto work = _get_param_queue.borrow_front();
        if (work) {
            if (strncmp(work->param_name.c_str(), param_value.param_id, PARAM_ID_LEN) == 0) {
                if (work->callback) {
                    ParamValue value;
                    value.set_from_mavlink_param_value(param_value);
                    work->callback(true, value);
                }
                _state = State::NONE;
                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                _get_param_queue.pop_front();
            } else {
                // No match, let's just return the borrowed work item.
                _get_param_queue.return_front();
            }
        }
    }

    else if (_state == State::SET_PARAM_BUSY) {
        auto work = _set_param_queue.borrow_front();
        if (work) {
            // Now it still needs to match the param name
            if (strncmp(work->param_name.c_str(), param_value.param_id, PARAM_ID_LEN) == 0) {
                // We are done, inform caller and go back to idle
                if (work->callback) {
                    work->callback(true);
                }

                _state = State::NONE;
                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                _set_param_queue.pop_front();
            } else {
                _set_param_queue.return_front();
            }
        }
    }
}

void MAVLinkParameters::process_param_ext_value(const mavlink_message_t &message)
{
    // LogDebug() << "getting param ext value";
    mavlink_param_ext_value_t param_ext_value;
    mavlink_msg_param_ext_value_decode(&message, &param_ext_value);

    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state == State::NONE) {
        return;
    }

    if (_state == State::GET_PARAM_BUSY) {
        auto work = _get_param_queue.borrow_front();
        if (work) {
            if (strncmp(work->param_name.c_str(), param_ext_value.param_id, PARAM_ID_LEN) == 0) {
                if (work->callback) {
                    ParamValue value;
                    value.set_from_mavlink_param_ext_value(param_ext_value);
                    work->callback(true, value);
                }
                _state = State::NONE;
                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                _get_param_queue.pop_front();
            } else {
                _get_param_queue.return_front();
            }
        }
    }

#if 0
    else if (_state == State::SET_PARAM_BUSY) {

        auto work = _set_param_queue.borrow_front();
        if (work) {
            // Now it still needs to match the param name
            if (strncmp(work->param_name.c_str(), param_ext_value.param_id, PARAM_ID_LEN) == 0) {

                // We are done, inform caller and go back to idle
                if (work->callback) {
                    work->callback(true);
                }

                _state = State::NONE;
                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " << _parent.get_time().elapsed_since_s(_last_request_time);
                _set_param_queue.pop_front();
            } else {
                _set_param_queue.return_front();
            }
        }
    }
#endif
}

void MAVLinkParameters::process_param_ext_ack(const mavlink_message_t &message)
{
    // LogDebug() << "getting param ext ack";

    mavlink_param_ext_ack_t param_ext_ack;
    mavlink_msg_param_ext_ack_decode(&message, &param_ext_ack);

    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state != State::SET_PARAM_BUSY) {
        return;
    }

    auto work = _set_param_queue.borrow_front();
    if (work) {
        // Now it still needs to match the param name
        if (strncmp(work->param_name.c_str(), param_ext_ack.param_id, PARAM_ID_LEN) == 0) {
            if (param_ext_ack.param_result == PARAM_ACK_ACCEPTED) {
                // We are done, inform caller and go back to idle
                if (work->callback) {
                    work->callback(true);
                }

                _state = State::NONE;
                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                _set_param_queue.pop_front();

            } else if (param_ext_ack.param_result == PARAM_ACK_IN_PROGRESS) {
                // Reset timeout and wait again.
                _parent.refresh_timeout_handler(_timeout_cookie);
                _set_param_queue.return_front();

            } else {
                LogErr() << "Somehow we did not get an ack, we got: "
                         << int(param_ext_ack.param_result);

                // We are done but unsuccessful
                // TODO: we need better error feedback
                if (work->callback) {
                    work->callback(false);
                }

                _state = State::NONE;
                _parent.unregister_timeout_handler(_timeout_cookie);
                // LogDebug() << "time taken: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                _set_param_queue.pop_front();
            }
        } else {
            _set_param_queue.return_front();
        }
    }
}

void MAVLinkParameters::receive_timeout()
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state == State::NONE) {
        // Strange, a timeout even though we're not doing anything
        return;
    }

    if (_state == State::GET_PARAM_BUSY) {
        auto work = _get_param_queue.borrow_front();
        if (work) {
            if (work->callback) {
                ParamValue empty_value;
                // Notify about timeout
                LogErr() << "Error: get param busy timeout: " << work->param_name;
                // LogErr() << "Got it after: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                work->callback(false, empty_value);
            }
            _state = State::NONE;
            _get_param_queue.pop_front();
        }
    }

    else if (_state == State::SET_PARAM_BUSY) {
        // This means work has been going on that we should try again
        if (_set_param_queue.size() > 0) {
            auto work = _set_param_queue.borrow_front();

            if (work->callback) {
                // Notify about timeout
                LogErr() << "Error: set param busy timeout: " << work->param_name;
                // LogErr() << "Got it after: " <<
                // _parent.get_time().elapsed_since_s(_last_request_time);
                work->callback(false);
            }
            _state = State::NONE;
            _set_param_queue.pop_front();
        }
    }
}

std::ostream &operator<<(std::ostream &strm, const MAVLinkParameters::ParamValue &obj)
{
    strm << obj.get_string();
    return strm;
}

} // namespace dronecode_sdk
