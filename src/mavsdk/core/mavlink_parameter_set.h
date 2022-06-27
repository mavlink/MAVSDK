#ifndef MAVSDK_SUPERBUILD_SRC_MAVSDK_CORE_MAVLINK_PARAMETER_SET_H_
#define MAVSDK_SUPERBUILD_SRC_MAVSDK_CORE_MAVLINK_PARAMETER_SET_H_

#include <map>
#include <mutex>
#include "param_value.h"

namespace mavsdk{

// This class provides convenient methods to handle a set of mavlink parameters.
// Its public methods are written under the following premises:
// 1) Once a parameter has been added, its type can not be mutated anymore.
// 2) Once a parameter has been added, it cannot be removed.
// This restriction makes sense when viewed from both a mavlink parameter server and client perspective:
// Changing the type of parameter (aka a Setting) most likely was a programming mistake
// and would easily lead to bugs / crashes.
class MavlinkParameterSet{
public:
    /**
     * add a new parameter to the parameter set, @return false if this parameter does already exist.
     */
    bool add_new_parameter(const std::string& param_id,ParamValue value);
    /**
     * update the value of an already existing parameter
     * @return true if the store value's type matches the type provided and the value therefore can be updated,
     * false otherwise.
     */
    bool update_existing_parameter(const std::string& param_id,const ParamValue& value);

    /**
     * Get the current value for a given parameter.
     * @return empty when the value does not exist, the current value otherwise.
     */
    std::optional<ParamValue> get_value(const std::string& param_id);

    template<class T>
    std::optional<ParamValue> get_value_exact_type(const std::string& param_id);

    [[nodiscard]] uint16_t get_current_parameters_count(bool extended)const;
    std::map<std::string, ParamValue> get_all();
private:
    std::mutex _all_params_mutex{};
    std::map<std::string, ParamValue> _all_params{};
};
}

#endif // MAVSDK_SUPERBUILD_SRC_MAVSDK_CORE_MAVLINK_PARAMETER_SET_H_
