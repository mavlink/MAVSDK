#include "mavlink_parameter_sender.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "system_impl.h"
#include <algorithm>
#include <cstring>
#include <future>
#include <cassert>

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
    if(value.is_same_type_templated<std::string>() && !extended){
        LogErr()<<"std::string needs extended protocol";
        if(callback){
            callback(Result::UnknownError);
        }
        return;
    }
    auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Set,name,_timeout_s_callback(),callback,extended,maybe_component_id);
    new_work->param_value = value;
    new_work->cookie = cookie;
    _work_queue.push_back(new_work);
}



void MavlinkParameterSender::set_param_int_async(
    const std::string& name,
    int32_t value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended,bool adhere_to_mavlink_specs)
{
    if (name.size() > PARAM_ID_LEN) {
        LogErr() << "Error: param name too long";
        if (callback) {
            callback(Result::ParamNameTooLong);
        }
        return;
    }
    if(adhere_to_mavlink_specs){
        // Lol so much easier - but assumes that the user meant int32_t when saying int ;)
        ParamValue value_to_set;
        value_to_set.set(static_cast<int32_t>(value));
        set_param_async(name,value_to_set,callback,cookie,maybe_component_id,extended);
        return;
    }
    // PX4 only uses int32_t, so we can be sure and don't need to check the exact type first
    // by getting the param, or checking the cache.
    if(_sender.autopilot() == SystemImpl::Autopilot::Px4){
        ParamValue value_to_set;
        value_to_set.set(static_cast<int32_t>(value));
        set_param_async(name,value_to_set,callback,cookie,maybe_component_id,extended);
    }else{
        // Here I implemented the same behaviour as the original author, but by using the same "do the checking in the callback" - trick
        // the duplicated code can be reduced nicely.
        // NOTE that his behaviour is not exactly to the mavlink specifications.
        // The issue here is: The user of the parameters library called set_int - the expected behaviour here would be to just send
        // a set - message with type==int aka int32_t. However, in this case, if the server actually stores (for example) and int8_t we will
        // get a invalid value back from the server, and the parameter is not changed. To work around this issue, here we try and find out what
        // the server stores, then cast the user-provided int parameter into this exact type and send that instead. Not to the specs, but I didn't want to
        // break things here.

        // Action to perform once we know the exact type, as we have the parameter cached.
        auto send_message_once_type_is_known=[this,name,value,callback,cookie,maybe_component_id,extended](){
            //std::lock_guard<std::mutex> lock(_all_params_mutex);
            assert(_all_params.find(name) != _all_params.end());
            auto copy= _all_params.at(name);
            const auto is_any_int=copy.set_int(value);
            if(is_any_int){
                LogDebug()<<"set_int() casted the type to whatever is cached internally";
                set_param_async(name,copy,callback,cookie,maybe_component_id,extended);
            }else{
                // Now if we have pre-cached the value already, and the user calls set_int() with a key that is not for an int, we could say
                // Hey, we know that this is not an int, and give the response of invalid value immediately back to the user. However, to keep things
                // consistently, here we just send out the message for an int32_t , perhaps the server allows changing the type of a parameter ?
                // This really goes to show how messy the workaround above is, though.
                LogDebug()<<"Weird case on set_int() read the documentation in code";
                ParamValue value_to_set;
                value_to_set.set(static_cast<int32_t>(value));
                set_param_async(name,value_to_set,callback,cookie,maybe_component_id,extended);
            }
        };
        // we need to be carefully with the mutex here
        _all_params_mutex.lock();
        const bool is_parameter_cached=_all_params.find(name) != _all_params.end();
        _all_params_mutex.unlock();
        // Now the parameter might change, but that doesn't matter, we never remove elements from the map.
        if(is_parameter_cached){
            send_message_once_type_is_known();
        }else{
            auto cb=[this,send_message_once_type_is_known,name,value,cookie,callback,maybe_component_id,extended]
                (Result result, ParamValue param_value){
                    if(result!=Result::Success){
                        // we didn't get the expected response from the server, but now send the message out anyways.
                        ParamValue value_to_set;
                        value_to_set.set(static_cast<int32_t>(value));
                        set_param_async(name,value_to_set,callback,cookie,maybe_component_id,extended);
                    }else{
                        // Now we have it cached, apply the same logic as above
                        send_message_once_type_is_known();
                    }
                };
            get_param_async(name, cb,cookie,maybe_component_id,extended);
        }
        // We do a get_param_async to find out the actual type, then cast the user-provided int value
        // to the int value the server side uses. This results in the same behaviour as implemented by the original author,
        // but I changed the code to
        get_param_async(name,nullptr,cookie,maybe_component_id,extended);
    }
}

MavlinkParameterSender::Result MavlinkParameterSender::set_param_int(
    const std::string& name,
    int32_t value,
    std::optional<uint8_t> maybe_component_id,
    bool extended,bool adhere_to_mavlink_specs)
{
    auto prom = std::promise<Result>();
    auto res = prom.get_future();

    set_param_int_async(
        name,
        value,
        [&prom](Result result) { prom.set_value(result); },
        this,
        maybe_component_id,
        extended,adhere_to_mavlink_specs);

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
    ParamValue value_to_set;
    value_to_set.set_float(value);
    set_param_async(name,value_to_set,callback,cookie,maybe_component_id,extended);
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
    ParamValue value_to_set;
    value_to_set.set_custom(value);
    set_param_async(name,value_to_set,callback,cookie,std::nullopt, true);
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

void MavlinkParameterSender::get_param_async(
    const std::string& name,
    GetParamAnyCallback callback,
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
            callback(Result::ParamNameTooLong, {});
        }
        return;
    }
    // Otherwise, push work onto queue.
    auto new_work = std::make_shared<WorkItem>(WorkItem::Type::Get,name,_timeout_s_callback(),callback,extended,maybe_component_id);
    // We don't need to know the exact type when getting a value - neither extended or non-extended protocol mavlink messages
    // specify the exact type on a "get_xxx" message. This makes total sense. The client still can reason about the type and return
    // the proper error codes, it just needs to delay these checks until a response from the server has been received.
    //new_work->param_value = value;
    new_work->cookie = cookie;
    _work_queue.push_back(new_work);
}

void MavlinkParameterSender::get_param_async(
    const std::string& name,
    ParamValue value_type,
    const GetParamAnyCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    // We need to delay the type checking until we get a response from the server.
    GetParamAnyCallback callback_future_result=[callback,value_type](Result result, ParamValue value){
        if(result==Result::Success){
            if(value.is_same_type(value_type)){
                callback(Result::Success,std::move(value));
            }else{
                callback(Result::WrongType,{});
            }
        }else{
            callback(result,{});
        }
    };
    get_param_async(name,callback,cookie,maybe_component_id,extended);
}

template<class T>
void MavlinkParameterSender::get_param_async_typesafe(
    const std::string& name,
    const GetParamTypesafeCallback<T> callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    // We need to delay the type checking until we get a response from the server.
    GetParamAnyCallback callback_future_result=[callback](Result result, ParamValue value){
        if(result==Result::Success){
            if(value.is_same_type_templated<T>()){
                callback(Result::Success,value.get<T>());
            }else{
                callback(Result::WrongType,{});
            }
        }else{
            callback(result,{});
        }
    };
    get_param_async(name,callback_future_result,cookie,maybe_component_id,extended);
}

void MavlinkParameterSender::get_param_float_async(
    const std::string& name,
    const GetParamFloatCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    get_param_async_typesafe<float>(name,callback,cookie,maybe_component_id,extended);
}


void MavlinkParameterSender::get_param_int_async(
    const std::string& name,
    const GetParamIntCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    get_param_async_typesafe<int32_t>(name,callback,cookie,maybe_component_id,extended);
}

void MavlinkParameterSender::get_param_custom_async(
    const std::string& name, const GetParamCustomCallback& callback, const void* cookie,std::optional<uint8_t> maybe_component_id)
{
    get_param_async_typesafe<std::string>(name,callback,cookie,maybe_component_id,true);
}



std::pair<MavlinkParameterSender::Result, ParamValue> MavlinkParameterSender::get_param(
    const std::string& name,bool extended)
{
    auto prom = std::promise<std::pair<Result, ParamValue>>();
    auto res = prom.get_future();
    get_param_async(
        name,
        [&prom](Result result, ParamValue new_value) {
            prom.set_value(std::make_pair<>(result, std::move(new_value)));
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

void MavlinkParameterSender::get_all_params_async(const GetAllParamsCallback& callback,const bool use_extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(_all_params_callback!= nullptr){
        LogDebug()<<"Give get_all_params_async time to complete before requesting again";
        // make sure any already existing request is terminated, since we immediately have to override an already
        // existing callback here. ( A future might be blocked on it).
        _all_params_callback({});
        _all_params_callback= nullptr;
    }
    _all_params_callback = callback;
    mavlink_message_t msg;
    if(use_extended){
        mavlink_msg_param_ext_request_list_pack(
            _sender.get_own_system_id(),
            _sender.get_own_component_id(),
            &msg,
            _sender.get_system_id(),
            MAV_COMP_ID_AUTOPILOT1); // FIXME: what should the component be?
    }else{
        mavlink_msg_param_request_list_pack(
            _sender.get_own_system_id(),
            _sender.get_own_component_id(),
            &msg,
            _sender.get_system_id(),
            MAV_COMP_ID_AUTOPILOT1); // FIXME: what should the component be?
    }
    if (!_sender.send_message(msg)) {
        LogErr() << "Failed to send param list request!";
        callback(std::map<std::string, ParamValue>{});
    }
    // There are 2 possible cases - we get all the messages in time - in this case, the
    // _all_params_callback member is called. Otherwise, we get a timeout at some point, which then calls
    // the _all_params_callback member with an empty result.
    _timeout_handler.add(
        [this] { receive_timeout(); }, _timeout_s_callback(), &_all_params_timeout_cookie);
}

std::map<std::string, ParamValue> MavlinkParameterSender::get_all_params(const bool use_extended)
{
    std::promise<std::map<std::string, ParamValue>> prom;
    auto res = prom.get_future();
    get_all_params_async(
        // Make sure to NOT use a reference for all_params here, pass by value.
        // Since for example on a timeout, the empty all_params result is constructed in-place and then
        // goes out of scope when the callback returns.
        [&prom](std::map<std::string, ParamValue> all_params) {
            prom.set_value(std::move(all_params));
        },use_extended);
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
            if (work->extended) {
                const auto param_value_buf = work->param_value.get_128_bytes();
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
                const float value_set = (_sender.autopilot() == SystemImpl::Autopilot::ArduPilot) ?
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
                        callback(Result::ConnectionError);
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
                if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                    if (callback) {
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
    const std::string param_id = extract_safe_param_id(param_value.param_id);
    if (_parameter_debugging) {
        LogDebug() << "getting param value: " << param_id;
    }
    ParamValue received_value;
    if (_sender.autopilot() == SystemImpl::Autopilot::ArduPilot) {
        received_value.set_from_mavlink_param_value_cast(param_value);
    } else {
        received_value.set_from_mavlink_param_value_bytewise(param_value);
    }
    {
        std::lock_guard<std::mutex> lock(_all_params_mutex);
        _all_params[param_id] = received_value;
        // check if we are looking for param list (get all parameters).
        if (_all_params_callback) {
            // If we are currently waiting for all parameters, this is a hacky way to basically say
            // "Hey, we got the last parameter, so we got all parameters, and can forward that to the user."
            // Note that I don't think this accounts for the case where a parameter that is not the last parameter went missing.
            // So it is better than nothing, but also not ideal. Correct me if I'm wrong.
            if (param_value.param_index + 1 == param_value.param_count) {
                _timeout_handler.remove(_all_params_timeout_cookie);
                _all_params_callback(_all_params);
                _all_params_callback = nullptr;
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

    // TODO I think we need to consider more edge cases here
    find_and_call_subscriptions_value_changed(param_id,received_value);

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (!work->already_requested) {
        return;
    }

    if (work->param_name != param_id) {
        // No match, let's just return the borrowed work item.
        return;
    }

    switch (work->type) {
        case WorkItem::Type::Get: {
            if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                if (callback) {
                    callback(Result::Success, received_value);
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
    const auto safe_param_id=extract_safe_param_id(param_ext_value.param_id);

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }
    if (!work->already_requested) {
        return;
    }
    if (work->param_name != safe_param_id) {
        return;
    }

    switch (work->type) {
        case WorkItem::Type::Get: {
            ParamValue value;
            value.set_from_mavlink_param_ext_value(param_ext_value);
            if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                if (callback) {
                    callback(Result::Success,value);
                }
            }
            _timeout_handler.remove(_timeout_cookie);
            // LogDebug() << "time taken: " <<
            // _sender.get_time().elapsed_since_s(_last_request_time);
            work_queue_guard.pop_front();
        } break;
        // According to the mavlink spec, PARAM_EXT_VALUE is only emitted in response to a PARAM_EXT_REQUEST_LIST or PARAM_EXT_REQUEST_READ.
        default:
            LogWarn() << "Unexpected ParamExtValue response";
            break;
    }
}


void MavlinkParameterSender::process_param_ext_ack(const mavlink_message_t& message)
{
    // LogDebug() << "getting param ext ack";

    mavlink_param_ext_ack_t param_ext_ack;
    mavlink_msg_param_ext_ack_decode(&message, &param_ext_ack);
    const auto safe_param_id=extract_safe_param_id(param_ext_ack.param_id);

    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (!work->already_requested) {
        return;
    }

    // Now it still needs to match the param name
    if (work->param_name != safe_param_id) {
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
                        callback(Result::Success);
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
                                    return Result::Failed;
                                case PARAM_ACK_VALUE_UNSUPPORTED:
                                    return Result::ValueUnsupported;
                                default:
                                    return Result::UnknownError;
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
        std::lock_guard<std::mutex> lock(_all_params_mutex);
        // first check if we are waiting for param list response
        if (_all_params_callback) {
            LogDebug()<<"All params receive timeout";
            _all_params_callback({});
            _all_params_callback= nullptr;
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

                    if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                        const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                        if (callback) {
                            callback(Result::ConnectionError, {});
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
                if (std::get_if<GetParamAnyCallback>(&work->callback)) {
                    const auto& callback = std::get<GetParamAnyCallback>(work->callback);
                    if (callback) {
                        callback(Result::Timeout, {});
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
                            callback(Result::ConnectionError);
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
                        callback(Result::Timeout);
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
