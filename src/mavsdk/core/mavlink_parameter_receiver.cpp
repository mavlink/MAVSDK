#include "mavlink_parameter_receiver.h"
#include <cstring>
#include <cassert>

namespace mavsdk {

MavlinkParameterReceiver::MavlinkParameterReceiver(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    TimeoutSCallback timeout_s_callback) :
    _sender(sender),
    _message_handler(message_handler)
{
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

MavlinkParameterReceiver::~MavlinkParameterReceiver()
{
    _message_handler.unregister_all(this);
}

std::string MavlinkParameterReceiver::extract_safe_param_id(const char param_id[])
{
    // The param_id field of the MAVLink struct has length 16 and can not be null terminated.
    // Therefore, we make a 0 terminated copy first.
    char param_id_long_enough[PARAM_ID_LEN + 1] = {};
    std::memcpy(param_id_long_enough, param_id, PARAM_ID_LEN);
    return {param_id_long_enough};
}

std::array<char, MavlinkParameterReceiver::PARAM_ID_LEN> MavlinkParameterReceiver::param_id_to_message_buffer(const std::string& param_id)
{
    assert(param_id.length()<=PARAM_ID_LEN);
    std::array<char,PARAM_ID_LEN> ret={};
    std::memcpy(ret.data(), param_id.c_str(),param_id.length());
    return ret;
}

MavlinkParameterReceiver::Result
MavlinkParameterReceiver::provide_server_param(const std::string& name, ParamValue param_value)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        return Result::ParamNameTooLong;
    }
    if(param_value.is<std::string>()){
        const auto s=param_value.get<std::string>();
        if(s.size()> sizeof(mavlink_param_ext_set_t::param_value)){
            LogErr()<<"Error: param value too long";
            return Result::ParamValueTooLong;
        }
    }
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(_all_params.find(name) != _all_params.end()){
        const auto curr_value = _all_params.at(name);
        if(!curr_value.is_same_type(param_value)){
            LogErr()<<"Cannot mutate the type of "<<name<<" from "<<curr_value.typestr() << " to "<<param_value.typestr();
            return Result::WrongType;
        }else{
            // update the value, even though that might result in unwanted behaviour.
            LogDebug()<<"Updating value of "<<name<<" on server, clients can be out of sync";
            _all_params.at(name).update_value_typesafe(param_value);
            return Result::Success;
        }
    }
    _all_params.insert_or_assign(name, param_value);
    return Result::Success;
}

MavlinkParameterReceiver::Result
MavlinkParameterReceiver::provide_server_param_float(const std::string& name, float value)
{
    ParamValue param_value;
    param_value.set(value);
    return provide_server_param(name,param_value);
}

MavlinkParameterReceiver::Result
MavlinkParameterReceiver::provide_server_param_int(const std::string& name,int32_t value)
{
    ParamValue param_value;
    param_value.set(value);
    return provide_server_param(name,param_value);
}

MavlinkParameterReceiver::Result MavlinkParameterReceiver::provide_server_param_custom(
    const std::string& name, const std::string& value)
{
    ParamValue param_value;
    param_value.set(value);
    return provide_server_param(name,param_value);
}

std::map<std::string, ParamValue> MavlinkParameterReceiver::retrieve_all_server_params()
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    return _all_params;
}

template<class T>
std::pair<MavlinkParameterReceiver::Result, T> MavlinkParameterReceiver::retrieve_server_param(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    const auto param_opt= find_param(name);
    if(!param_opt.has_value()){
        return {Result::NotFound, {}};
    }
    // This parameter exists, check its type
    const auto& param=param_opt.value();
    if(param.is_same_type_templated<T>()){
        return {Result::Success, param.get<T>()};
    }
    return {Result::WrongType, {}};
}

std::pair<MavlinkParameterReceiver::Result, float>
MavlinkParameterReceiver::retrieve_server_param_float(const std::string& name)
{
        return retrieve_server_param<float>(name);
}

std::pair<MavlinkParameterReceiver::Result, std::string>
MavlinkParameterReceiver::retrieve_server_param_custom(const std::string& name)
{
    return retrieve_server_param<std::string>(name);
}

std::pair<MavlinkParameterReceiver::Result, int32_t>
MavlinkParameterReceiver::retrieve_server_param_int(const std::string& name)
{
    return retrieve_server_param<int32_t>(name);
}

void MavlinkParameterReceiver::process_param_set_internally(const std::string& param_id,const ParamValue& value,bool extended)
{
    LogDebug() << "Param set request "<<(extended ? "Ext" : "")<<": " << param_id << " with value_type: "
               <<value.typestr()<<" value: "<<value.get_string();
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    // Check if we have this parameter
    const auto param_opt= find_param(param_id);
    if(!param_opt.has_value()){
        LogDebug() << "Missing Param: " << param_id << "(this: " << this << ")";
        return;
    }
    const auto& curr_value=param_opt.value();
    bool param_was_changed=false;
    PARAM_ACK param_ack=PARAM_ACK_ACCEPTED; // only extended differentiates.
    // check if the type of the param from the param set matches the type from the message
    if(curr_value.is_same_type(value)){
        // TODO check if the change has no effect
        LogDebug() << "Changing param "<<param_id<<" from " << curr_value << " to " << value;
        _all_params.at(param_id) = value;
        param_was_changed= true;
    }else{
        LogDebug()<< "Ignoring invalid param set request due to type mismatch";
        param_ack=PARAM_ACK_FAILED;
    }
    // No matter if we've changed the value or not, we need to respond to the request.
    // It is up to the component who performed the request to reason if the set was successfully.
    // (Unfortunately, the non-extended protocol does not differentiate here between a failed and non-failed set request)
    const int param_count = get_current_parameters_count(extended);
    const int param_idx= -1; //TODO
    if(extended){
        auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Ack,param_id,true,_all_params.at(param_id),param_count,param_idx);
        new_work->param_ack = param_ack;
        _work_queue.push_back(new_work);
    }else{
        auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Value,param_id,false,_all_params.at(param_id),param_count,param_idx);
        _work_queue.push_back(new_work);
    }
    if(param_was_changed){
        find_and_call_subscriptions_value_changed(param_id,value);
    }

}

void MavlinkParameterReceiver::process_param_set(const mavlink_message_t& message)
{
    mavlink_param_set_t set_request{};
    mavlink_msg_param_set_decode(&message, &set_request);
    const std::string safe_param_id = extract_safe_param_id(set_request.param_id);
    if(safe_param_id.empty()){
        // TODO support int as index, as done in the mavlink specification
        LogWarn() << "Invalid Param Set ID Request: " << safe_param_id;
        return;
    }
    ParamValue value;
    if (!value.set_from_mavlink_param_set_bytewise(set_request)) {
        // This should never happen, the type enum in the message is unknown.
        LogWarn() << "Invalid Param Set Request: " << safe_param_id;
        return;
    }
    process_param_set_internally(safe_param_id,value, false);
}

void MavlinkParameterReceiver::process_param_ext_set(const mavlink_message_t& message)
{
    mavlink_param_ext_set_t set_request{};
    mavlink_msg_param_ext_set_decode(&message, &set_request);
    const std::string safe_param_id = extract_safe_param_id(set_request.param_id);
    if(safe_param_id.empty()){
        // TODO support int as index, as done in the mavlink specification
        LogWarn() << "Invalid Param Set ID Request: " << safe_param_id;
        return;
    }
    // Use the ID
    ParamValue value;
    if (!value.set_from_mavlink_param_ext_set(set_request)) {
        LogWarn() << "Invalid Param Ext Set Request: " << safe_param_id;
        return;
    }
    process_param_set_internally(safe_param_id,value, true);
}

void MavlinkParameterReceiver::process_param_request_read(const mavlink_message_t& message)
{
    mavlink_param_request_read_t read_request{};
    mavlink_msg_param_request_read_decode(&message, &read_request);

    if (read_request.param_index == -1) {
        const auto safe_param_id= extract_safe_param_id(read_request.param_id);
        LogDebug() << "Request Param " << safe_param_id;
        const auto param_opt= find_param(safe_param_id);
        if(!param_opt.has_value()){
            LogDebug() << "Missing Param " << safe_param_id;
            return;
        }
        const auto& param_value=param_opt.value();
        if(param_value.needs_extended()){
            LogDebug()<<"Not forwarding param"<<safe_param_id<<" since it needs extended";
            return;
        }
        const int param_count = get_current_parameters_count(false);
        const int param_idx= -1; //TODO
        auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Value,safe_param_id,false,param_value,param_count,param_idx);
        _work_queue.push_back(new_work);
    }else{
        // TODO the server should be able to handle requests with a int index instead of string as param_id.
    }
}

void MavlinkParameterReceiver::process_param_request_list(const mavlink_message_t& message)
{
    mavlink_param_request_list_t list_request{};
    mavlink_msg_param_request_list_decode(&message, &list_request);
    int idx=0;
    for (const auto& pair : _all_params) {
        // make sure extended parameters never make it out via the param request list
        // (use param extended list)
        if(pair.second.needs_extended()){
            LogDebug()<<"Not forwarding param"<<pair.first<<" since it needs extended";
            continue;
        }
        // the count of parameters when queried from a non-ext perspective is different, since we need to hide the parameters
        // that need the extended protocol
        const int param_count = get_current_parameters_count(false);
        const int param_idx= idx++;
        auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Value,pair.first,false,pair.second,param_count,param_idx);
        _work_queue.push_back(new_work);
    }
}

void MavlinkParameterReceiver::process_param_ext_request_list(const mavlink_message_t& message)
{
    mavlink_param_ext_request_list_t ext_list_request{};
    mavlink_msg_param_ext_request_list_decode(&message, &ext_list_request);
    int idx=0;
    for (const auto& pair : _all_params) {
        const int param_count = get_current_parameters_count(true);
        const int param_idx= idx++;
        auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Value,pair.first,true,pair.second,param_count,param_idx);
        _work_queue.push_back(new_work);
    }
}

void MavlinkParameterReceiver::process_param_ext_request_read(const mavlink_message_t& message)
{
    mavlink_param_request_read_t read_request{};
    mavlink_msg_param_request_read_decode(&message, &read_request);

    if (read_request.param_index == -1) {
        const auto safe_param_id = extract_safe_param_id(read_request.param_id);
        LogDebug() << "Request Param " << safe_param_id;
        const auto param_opt= find_param(safe_param_id);
        if(!param_opt.has_value()){
            LogDebug() << "Missing Param " << safe_param_id;
            return;
        }
        const auto& param_value = param_opt.value();
        const int param_count = get_current_parameters_count(true);
        const int param_idx= -1; //TODO
        auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Value,safe_param_id,true,param_value,param_count,param_idx);
        _work_queue.push_back(new_work);
    }else{
        // TODO the server should be able to handle requests with a int index instead of string as param_id.
    }
}

void MavlinkParameterReceiver::do_work()
{
    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();
    if (!work) {
        return;
    }
    const auto param_id_message_buffer=param_id_to_message_buffer(work->param_name);
    mavlink_message_t mavlink_message;
    switch (work->type) {
        case WorkItem::Type::Value: {
            if (work->extended) {
                const auto buf = work->param_value.get_128_bytes();
                //mavlink_msg_param_ext_value_encode()
                mavlink_msg_param_ext_value_pack(
                    _sender.get_own_system_id(),
                    _sender.get_own_component_id(),
                    &mavlink_message,
                    param_id_message_buffer.data(),
                    buf.data(),
                    work->param_value.get_mav_param_ext_type(),
                    work->param_count,
                    work->param_index);
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
                    work->param_count,
                    work->param_index);
            }
            if (!_sender.send_message(mavlink_message)) {
                LogErr() << "Error: Send message failed";
                work_queue_guard.pop_front();
                return;
            }
            work_queue_guard.pop_front();
        } break;

        case WorkItem::Type::Ack: {
            // ACK is only in the extended protocol.
            assert(work->extended);
            assert(work->param_ack.has_value());
            auto buf = work->param_value.get_128_bytes();
            mavlink_msg_param_ext_ack_pack(
                _sender.get_own_system_id(),
                _sender.get_own_component_id(),
                &mavlink_message,
                param_id_message_buffer.data(),
                buf.data(),
                work->param_value.get_mav_param_ext_type(),
                work->param_ack.value());
            if (!_sender.send_message(mavlink_message)) {
                LogErr() << "Error: Send message failed";
                work_queue_guard.pop_front();
                return;
            }
            work_queue_guard.pop_front();
        } break;
    }
}

std::ostream& operator<<(std::ostream& str, const MavlinkParameterReceiver::Result& result)
{
    switch (result) {
        case MavlinkParameterReceiver::Result::Success:
            return str << "Success";
        case MavlinkParameterReceiver::Result::WrongType:
            return str << "WrongType";
        case MavlinkParameterReceiver::Result::ParamNameTooLong:
            return str << "ParamNameTooLong";
        case MavlinkParameterReceiver::Result::NotFound:
            return str << "NotFound";
        case MavlinkParameterReceiver::Result::ParamValueTooLong:
            return str << ":ParamValueTooLong";
        default:
            return str << "UnknownError";
    }
}

int MavlinkParameterReceiver::get_current_parameters_count(bool extended)const{
    if(extended){
        // easy, we can do all parameters.
        return static_cast<int>(_all_params.size());
    }
    // a bit messy, we need to loop through all params and only count the ones that are non-extended
    int count=0;
    for (auto const& [key, val] : _all_params){
        if(!val.needs_extended()){
            count++;
        }
    }
    return count;
}

std::optional<ParamValue> MavlinkParameterReceiver::find_param(const std::string& param_id)
{
    if(_all_params.find(param_id) != _all_params.end()){
        return _all_params.at(param_id);
    }
    return {};
}

} // namespace mavsdk