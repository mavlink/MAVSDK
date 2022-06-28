#ifndef MAVSDK_SUPERBUILD_SRC_MAVSDK_CORE_MAVLINK_PARAMETER_SET_H_
#define MAVSDK_SUPERBUILD_SRC_MAVSDK_CORE_MAVLINK_PARAMETER_SET_H_

#include <map>
#include <mutex>
#include "param_value.h"

namespace mavsdk{

// This class provides convenient methods to handle a set of mavlink parameters.
// As an example, if at any point in the parameter server / client you need to update an existing parameter
// and safely handle all the possible errors in this operation (for example, the parameter does not exist yet)
// you can use update_existing_parameter().
// Its public methods are written for the following premises:
// 1) Once a parameter has been added, its type can not be mutated anymore.
// 2) Once a parameter has been added, it cannot be removed.
// This restriction makes sense when viewed from both a mavlink parameter server and client perspective:
// Changing the type (not value) of parameter (aka a Setting) most likely was a programming mistake
// and would easily lead to bugs / crashes.
class MavlinkParameterSet{
public:
    /**
     * add a new parameter to the parameter set, as long as the parameter does not exist yet, there is space available and
     * the param_id is not empty.
     * @return true on success, false otherwise.
     */
    bool add_new_parameter(const std::string& param_id,ParamValue value);

    enum class UpdateExistingParamResult{
        SUCCESS,
        MISSING_PARAM,
        WRONG_PARAM_TYPE
    };
    friend std::ostream& operator<<(std::ostream& strm, const MavlinkParameterSet::UpdateExistingParamResult& obj);
    /**
     * update the value of an already existing parameter, as long as current and provided type match.
     * Does not add the parameter as a new parameter if missing.
     * @return one of the results above.
     */
    UpdateExistingParamResult update_existing_parameter(const std::string& param_id,const ParamValue& value);

    struct Parameter{
        // unique parameter id
        const std::string param_id;
        // unique parameter index
        const int param_index;
        // value of this parameter.
        ParamValue value;
    };
    std::vector<Parameter> get_all(bool supports_extended);
    std::map<std::string, ParamValue> get_copy();

    std::optional<Parameter> get_param(const std::string& param_id,bool extended);
    std::optional<Parameter> get_param(const uint16_t param_idx,bool extended);
    // Mavlink uses uint16_t for parameter indices, which allows for that many parameters maximum
    static constexpr auto MAX_N_PARAMETERS= 65535;
    /*
     * Return the n of parameters, either from an extended or non-extended perspective.
     * ( we need to hide parameters that need extended from non-extended queries).
     * Doesn't acquire the all-parameters lock, since when used it should already be locked.
     */
    [[nodiscard]] uint16_t get_current_parameters_count(bool extended);
private:
    std::mutex _all_params_mutex{};
    // list of all the parameters supported by both non-extended and extended protocol.
    std::vector<Parameter> _all_params;
    // list of all the parameters supported only by the extended protocol
    //std::vector<Parameter> _string_params;
    // if an element exists in this map, since we never remove parameters, it is guaranteed that the returned index is
    // inside the _all_params range.
    std::map<std::string,uint16_t> _param_id_to_idx;
    //
    const bool enable_debugging=true;
};

std::ostream& operator<<(std::ostream& strm, const MavlinkParameterSet::Parameter& obj);

}

#endif // MAVSDK_SUPERBUILD_SRC_MAVSDK_CORE_MAVLINK_PARAMETER_SET_H_
