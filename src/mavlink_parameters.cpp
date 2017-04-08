#include "mavlink_parameters.h"
#include "device_impl.h"

namespace dronelink {

MavlinkParameters::MavlinkParameters(DeviceImpl *parent) :
    _parent(parent),
    _request_state(RequestState::NONE),
    _set_param_queue(),
    _get_param_queue()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_PARAM_VALUE,
        std::bind(&MavlinkParameters::process_param_value, this, std::placeholders::_1), this);
}

MavlinkParameters::~MavlinkParameters()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MavlinkParameters::set_param_async(const std::string &name,
                                        const ParamValue &value,
                                        set_param_callback_t callback)
{
    if (name.size() > PARAM_ID_LEN) {
        Debug() << "Error: param name too long";
        if (callback) {
            callback(false);
        }
        return;
    }

    SetParamWork new_work;
    new_work.callback = callback;
    new_work.param_name = name;
    new_work.param_value = value;

    _set_param_queue.push_back(new_work);

}


void MavlinkParameters::get_param_async(const std::string &name, get_param_callback_t callback)
{
    if (name.size() > PARAM_ID_LEN) {
        Debug() << "Error: param name too long";
        if (callback) {
            ParamValue empty_param;
            callback(false, empty_param);
        }
        return;
    }

    GetParamWork new_work;
    new_work.callback = callback;
    new_work.param_name = name;

    _get_param_queue.push_back(new_work);

}

//void MavlinkParameters::save_async()
//{
//    _parent->send_command(MAV_CMD_PREFLIGHT_STORAGE,
//                          DeviceImpl::CommandParams {1.0f, 1.0f, 0.0f, NAN, NAN, NAN, NAN});
//}

void MavlinkParameters::do_work()
{
    if (_request_state != RequestState::NONE) {
        // If we're still busy, let's wait
        return;
    }


    if (_set_param_queue.size() > 0) {
        // There params to set which we always do first
        SetParamWork &work = _set_param_queue.front();

        // We need to wait for this param to get sent back as confirmation.
        _request_state = RequestState::SET_PARAM_BUSY;

        char param_id[PARAM_ID_LEN] = {};
        strncpy_s(param_id, work.param_name.c_str(), sizeof(param_id));

        mavlink_message_t message = {};
        mavlink_msg_param_set_pack(_parent->get_own_system_id(),
                                   _parent->get_own_component_id(),
                                   &message,
                                   _parent->get_target_system_id(),
                                   _parent->get_target_component_id(),
                                   param_id,
                                   work.param_value.get_float_casted_value(),
                                   work.param_value.get_mav_param_type());

        if (!_parent->send_message(message)) {
            Debug() << "Error: Send message failed";
            if (work.callback) {
                work.callback(false);
            }
            _request_state = RequestState::NONE;
            return;
        }

        // We want to get notified if a timeout happens
        _parent->register_timeout_handler(std::bind(&MavlinkParameters::receive_timeout, this),
                                          3.0,
                                          this);

    } else if (_get_param_queue.size() > 0) {

        GetParamWork work = _get_param_queue.front();

        // The busy flag gets reset when the param comes in
        // or after a timeout.
        _request_state = RequestState::GET_PARAM_BUSY;

        char param_id[PARAM_ID_LEN] = {};
        strncpy_s(param_id, work.param_name.c_str(), sizeof(param_id));

        mavlink_message_t message = {};
        mavlink_msg_param_request_read_pack(_parent->get_own_system_id(),
                                            _parent->get_own_component_id(),
                                            &message,
                                            _parent->get_target_system_id(),
                                            _parent->get_target_component_id(),
                                            param_id,
                                            -1);

        if (!_parent->send_message(message)) {
            Debug() << "Error: Send message failed";
            if (work.callback) {
                ParamValue empty_param;
                work.callback(false, empty_param);
            }
            _request_state = RequestState::NONE;
            return;
        }

        // We want to get notified if a timeout happens
        _parent->register_timeout_handler(std::bind(&MavlinkParameters::receive_timeout, this),
                                          3.0,
                                          this);
    }
}

void MavlinkParameters::process_param_value(const mavlink_message_t &message)
{
    mavlink_param_value_t param_value;
    mavlink_msg_param_value_decode(&message, &param_value);

    if (_request_state == RequestState::NONE) {
        return;
    }

    if (_request_state == RequestState::GET_PARAM_BUSY) {

        // This means we should have a queue entry to use
        if (_get_param_queue.size() > 0) {
            GetParamWork &work = _get_param_queue.front();

            if (strncmp(work.param_name.c_str(), param_value.param_id, PARAM_ID_LEN) == 0) {

                if (work.callback) {
                    ParamValue value;
                    value.set_from_mavlink_param_value(param_value);
                    work.callback(true, value);
                }
                _request_state = RequestState::NONE;
                _parent->unregister_timeout_handler(this);
                _get_param_queue.pop_front();
            }
        }
    }

    else if (_request_state == RequestState::SET_PARAM_BUSY) {

        // This means we should have a queue entry to use
        if (_set_param_queue.size() > 0) {
            SetParamWork &work = _set_param_queue.front();

            // Now it still needs to match the param name
            if (strncmp(work.param_name.c_str(), param_value.param_id, PARAM_ID_LEN) == 0) {

                // We are done, inform caller and go back to idle
                if (work.callback) {
                    work.callback(true);
                }

                _request_state = RequestState::NONE;
                _parent->unregister_timeout_handler(this);
                _set_param_queue.pop_front();
            }
        }
    }
}

void MavlinkParameters::receive_timeout()
{
    if (_request_state == RequestState::NONE) {
        // Strange, a timeout even though we're not doing anything
        return;
    }

    if (_request_state == RequestState::GET_PARAM_BUSY) {

        // This means work has been going on that we should try again
        if (_get_param_queue.size() > 0) {
            GetParamWork &work = _get_param_queue.front();

            if (work.callback) {
                ParamValue empty_value;
                // Notify about timeout
                Debug() << "Error: timeout";
                work.callback(false, empty_value);
            }
            _request_state = RequestState::NONE;
            _get_param_queue.pop_front();
        }
    }

    else if (_request_state == RequestState::SET_PARAM_BUSY) {

        // This means work has been going on that we should try again
        if (_set_param_queue.size() > 0) {
            SetParamWork &work = _set_param_queue.front();

            if (work.callback) {
                // Notify about timeout
                Debug() << "Error: timeout";
                work.callback(false);
            }
            _request_state = RequestState::NONE;
            _set_param_queue.pop_front();
        }
    }
}

} // namespace dronelink
