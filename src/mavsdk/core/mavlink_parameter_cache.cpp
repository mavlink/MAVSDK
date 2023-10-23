#include "mavlink_parameter_cache.h"

#include <algorithm>

namespace mavsdk {

MavlinkParameterCache::AddNewParamResult
MavlinkParameterCache::add_new_param(const std::string& param_id, ParamValue value, int16_t index)
{
    if (exists(param_id)) {
        return AddNewParamResult::AlreadyExists;
    }

    if (static_cast<size_t>(_all_params.size() + 1) >
        static_cast<size_t>(std::numeric_limits<int16_t>::max())) {
        return AddNewParamResult::TooManyParams;
    }

    _all_params.push_back(Param{
        param_id,
        std::move(value),
        (index != -1 ? static_cast<uint16_t>(index) : static_cast<uint16_t>(_all_params.size()))});
    return MavlinkParameterCache::AddNewParamResult::Ok;
}

MavlinkParameterCache::UpdateExistingParamResult
MavlinkParameterCache::update_existing_param(const std::string& param_id, ParamValue value)
{
    auto it = std::find_if(_all_params.begin(), _all_params.end(), [&](const auto& param) {
        return (param_id == param.id);
    });

    if (it == _all_params.end()) {
        return UpdateExistingParamResult::MissingParam;
    }

    if (!it->value.is_same_type(value)) {
        return MavlinkParameterCache::UpdateExistingParamResult::WrongType;
    } else {
        it->value.update_value_typesafe(value);
        return MavlinkParameterCache::UpdateExistingParamResult::Ok;
    }
}

std::vector<MavlinkParameterCache::Param>
MavlinkParameterCache::all_parameters(bool including_extended) const
{
    if (including_extended) {
        return _all_params;
    } else {
        // TODO: we might want to cache this
        std::vector<MavlinkParameterCache::Param> params_without_extended{};
        std::copy_if(
            _all_params.begin(),
            _all_params.end(),
            std::back_inserter(params_without_extended),
            [](auto& entry) { return !entry.value.needs_extended(); });

        return params_without_extended;
    }
}

std::map<std::string, ParamValue>
MavlinkParameterCache::all_parameters_map(bool including_extended) const
{
    std::map<std::string, ParamValue> mp{};
    if (including_extended) {
        for (const auto& param : _all_params) {
            mp.insert({param.id, param.value});
        }

    } else {
        for (const auto& param : _all_params) {
            if (param.value.needs_extended()) {
                continue;
            }
            mp.insert({param.id, param.value});
        }
    }

    return mp;
}

std::optional<MavlinkParameterCache::Param>
MavlinkParameterCache::param_by_id(const std::string& param_id, bool including_extended) const
{
    const auto& params = all_parameters(including_extended);

    for (const auto& param : params) {
        if (param.id == param_id) {
            return param;
        }
    }

    return {};
}

std::optional<MavlinkParameterCache::Param>
MavlinkParameterCache::param_by_index(uint16_t param_index, bool including_extended) const
{
    const auto& params = all_parameters(including_extended);
    if (param_index >= params.size()) {
        LogErr() << "param at " << (int)param_index << " out of bounds (" << params.size() << ")";
        return {};
    }

    const auto& param = params[param_index];
    // Check that the redundant index matches the actual vector index.
    assert(param.index == param_index);
    return {param};
}

uint16_t MavlinkParameterCache::count(bool including_extended) const
{
    const auto num = all_parameters(including_extended).size();
    assert(num < std::numeric_limits<uint16_t>::max());
    return static_cast<uint16_t>(num);
}

void MavlinkParameterCache::clear()
{
    _all_params.clear();
}

bool MavlinkParameterCache::exists(const std::string& param_id) const
{
    auto it = std::find_if(_all_params.begin(), _all_params.end(), [&](const auto& param) {
        return (param_id == param.id);
    });
    return it != _all_params.end();
}

std::optional<uint16_t> MavlinkParameterCache::next_missing_index(uint16_t count)
{
    // Extended doesn't matter here because we use this function in the sender
    // which is always either all extended or not.
    std::sort(_all_params.begin(), _all_params.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.index < rhs.index;
    });

    for (unsigned i = 0; i < count; ++i) {
        if (_all_params.size() <= i) {
            // We have reached the end but it's not complete yet.
            return i;
        }
        if (_all_params[i].index > i) {
            // We have found a hole to fill.
            return i;
        }
    }
    return {};
}

} // namespace mavsdk
