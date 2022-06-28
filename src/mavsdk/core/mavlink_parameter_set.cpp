#include "mavlink_parameter_set.h"

#include <utility>

namespace mavsdk{

bool MavlinkParameterSet::add_new_parameter(const std::string& param_id, ParamValue value)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(param_id.empty()){
        if(enable_debugging){
            LogDebug()<<"Cannot add parameter with empty param id: ";
        }
        return false;
    }
    if(_param_id_to_idx.find(param_id) != _param_id_to_idx.end()) {
        // this parameter does already exist, we cannot add it as a new one.
        return false;
    }
    if(_all_params.size()<MAX_N_PARAMETERS){
        Parameter parameter{param_id,static_cast<uint16_t>(_all_params.size()),std::move(value)};
        _all_params.push_back(parameter);
        _param_id_to_idx[param_id]=parameter.param_index;
        if(enable_debugging){
            LogDebug()<<"Added parameter: "<<parameter;
        }
        return true;
    }
    return false;
}

MavlinkParameterSet::UpdateExistingParamResult MavlinkParameterSet::update_existing_parameter(const std::string& param_id,const ParamValue& value)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(_param_id_to_idx.find(param_id) == _param_id_to_idx.end()) {
        // this parameter does not exist yet.
        LogDebug()<<"MavlinkParameterSet::update_existing_parameter "<<param_id<<" does not exist";
        return UpdateExistingParamResult::MISSING_PARAM;
    }
    const auto index=_param_id_to_idx.at(param_id);
    const auto parameter = _all_params.at(index);
    if(!parameter.value.is_same_type(value)){
        // We cannot mutate the parameter type.
        LogDebug()<<"Cannot mutate the type of "<<param_id<<" from "<<parameter.value.typestr() << " to "<<value.typestr();
        return  UpdateExistingParamResult::WRONG_PARAM_TYPE;
    }
    _all_params.at(index).value.update_value_typesafe(value);
    return UpdateExistingParamResult::SUCCESS;
}


std::vector<MavlinkParameterSet::Parameter> MavlinkParameterSet::get_all(const bool supports_extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(supports_extended)return _all_params;
    std::vector<MavlinkParameterSet::Parameter> ret;
    for (const auto& param : _all_params) {
        if(!param.value.needs_extended()){
            ret.push_back(param);
        }
    }
    return ret;
}

std::map<std::string, ParamValue> MavlinkParameterSet::get_copy()
{
   std::lock_guard<std::mutex> lock(_all_params_mutex);
   std::map<std::string,ParamValue> ret;
   for(const auto& param:_all_params){
       ret[param.param_id]=param.value;
   }
   return ret;
}

uint16_t MavlinkParameterSet::get_current_parameters_count(bool extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(extended){
        // easy, we can do all parameters.
        return static_cast<uint16_t>(_all_params.size());
    }
    // a bit messy, we need to loop through all params and only count the ones that are non-extended
    int count=0;
    for (const auto& param : _all_params){
        if(!param.value.needs_extended()){
            count++;
        }
    }
    return  static_cast<uint16_t>(count);
}

std::optional<MavlinkParameterSet::Parameter> MavlinkParameterSet::get_param(const std::string& param_id,bool extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(_param_id_to_idx.find(param_id)==_param_id_to_idx.end()){
        // param does not exist
        return {};
    }
    const auto param_index=_param_id_to_idx.at(param_id);
    return _all_params.at(param_index);
}

std::optional<MavlinkParameterSet::Parameter> MavlinkParameterSet::get_param(const uint16_t param_idx,bool extended)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(param_idx<_all_params.size()){
        return _all_params.at(param_idx);
    }
    return {};
}
std::ostream&
operator<<(std::ostream& strm, const MavlinkParameterSet::UpdateExistingParamResult& obj)
{
    switch (obj) {
        case MavlinkParameterSet::UpdateExistingParamResult::MISSING_PARAM:
            strm<<"MISSING_PARAM";
            break;
        case MavlinkParameterSet::UpdateExistingParamResult::SUCCESS:
            strm<<"SUCCESS";
            break;
        case MavlinkParameterSet::UpdateExistingParamResult::WRONG_PARAM_TYPE:
            strm<<"WRONG_PARAM_TYPE";
            break;
    }
    return strm;
}

std::string MavlinkParameterSet::extract_safe_param_id(const char* param_id)
{
    // The param_id field of the MAVLink struct has length 16 and can not be null terminated.
    // Therefore, we make a 0 terminated copy first.
    char param_id_long_enough[PARAM_ID_LEN + 1] = {};
    std::memcpy(param_id_long_enough, param_id, PARAM_ID_LEN);
    return {param_id_long_enough};
}

std::array<char, MavlinkParameterSet::PARAM_ID_LEN>
MavlinkParameterSet::param_id_to_message_buffer(const std::string& param_id)
{
    assert(param_id.length()<=PARAM_ID_LEN);
    std::array<char,PARAM_ID_LEN> ret={};
    std::memcpy(ret.data(), param_id.c_str(),param_id.length());
    return ret;
}

std::ostream& operator<<(std::ostream& strm, const MavlinkParameterSet::Parameter& obj)
{
    strm << "Parameter{"<<obj.param_id<<":"<<obj.param_index<<" value:"<<obj.value.typestr()<<","<<obj.value.get_string()<<"}";
    return strm;
}
}