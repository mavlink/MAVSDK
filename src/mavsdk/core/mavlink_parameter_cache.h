#pragma once

#include "param_value.h"

#include <limits>
#include <map>
#include <string>
#include <vector>

namespace mavsdk {

class MavlinkParameterCache {
public:
    struct Param {
        std::string id;
        ParamValue value;
        uint16_t index; // matches the index when in a vector
    };

    enum class AddNewParamResult {
        Ok,
        AlreadyExists,
        TooManyParams,
    };
    AddNewParamResult
    add_new_param(const std::string& param_id, ParamValue value, int16_t index = -1);

    enum class UpdateExistingParamResult {
        Ok,
        MissingParam,
        WrongType,
    };
    UpdateExistingParamResult update_existing_param(const std::string& param_id, ParamValue value);

    [[nodiscard]] std::vector<Param> all_parameters(bool including_extended) const;
    [[nodiscard]] std::map<std::string, ParamValue>
    all_parameters_map(bool including_extended) const;

    [[nodiscard]] std::optional<Param>
    param_by_id(const std::string& param_id, bool including_extended) const;

    [[nodiscard]] std::optional<Param>
    param_by_index(uint16_t param_index, bool including_extended) const;

    [[nodiscard]] uint16_t count(bool including_extended) const;

    [[nodiscard]] std::optional<uint16_t> next_missing_index(uint16_t count);

    void clear();

private:
    [[nodiscard]] bool exists(const std::string& param_id) const;

    std::vector<Param> _all_params;
};

} // namespace mavsdk
