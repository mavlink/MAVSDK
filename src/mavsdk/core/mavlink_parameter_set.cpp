#include "mavlink_parameter_set.h"

#include <utility>

namespace mavsdk{

bool MavlinkParameterSet::add_new_parameter(const std::string& param_id, ParamValue value)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(_all_params.find(param_id) != _all_params.end()) {
        return false;
    }
    _all_params[param_id]=std::move(value);
    return true;
}

bool MavlinkParameterSet::update_existing_parameter(const std::string& param_id,const ParamValue& value)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(_all_params.find(param_id) == _all_params.end()) {
        // this parameter does not exist yet.
        return false;
    }
    const auto curr_value = _all_params.at(param_id);
    if(!curr_value.is_same_type(value)){
        // We cannot mutate the parameter type.
        return false;
    }
    if(curr_value._value==value._value){
        // The parameter already has the given value
        return false;
    }
    _all_params[param_id].update_value_typesafe(value);
    return true;
}

std::optional<ParamValue>  MavlinkParameterSet::get_value(const std::string& param_id)
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    if(_all_params.find(param_id) == _all_params.end()) {
        // this parameter does not exist yet.
        return std::nullopt;
    }
    return _all_params[param_id];
}

template<class T>
std::optional<ParamValue> MavlinkParameterSet::get_value_exact_type(const std::string& param_id)
{
    auto value = get_value(param_id);
    if (value.has_value() && value->is_same_type_templated<T>()){
        return value;
    }
    return {};
}
std::map<std::string, ParamValue> MavlinkParameterSet::get_all()
{
    std::lock_guard<std::mutex> lock(_all_params_mutex);
    return _all_params;
}

uint16_t MavlinkParameterSet::get_current_parameters_count(bool extended) const
{
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

}