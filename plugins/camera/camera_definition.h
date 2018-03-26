#pragma once

#include "mavlink_parameters.h"
#include <tinyxml2.h>
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace dronecore {

class CameraDefinition
{
public:
    CameraDefinition();
    ~CameraDefinition();

    void load_file(const char *filename);
    void load_string(const std::string &content);

    const char *get_vendor() const;
    const char *get_model() const;

    struct ParameterOption {
        std::string name;
        MavlinkParameters::ParamValue value;
    };

    typedef std::vector<std::shared_ptr<ParameterOption>> option_list_t;

    struct Parameter {
        std::string description;
        option_list_t options;
    };

    void update_setting(const std::string &name, const MavlinkParameters::ParamValue &value);

    typedef std::map<std::string, std::shared_ptr<Parameter>> parameter_map_t;
    bool get_parameters(parameter_map_t &parameters, bool filter_possible);

    // Non-copyable
    CameraDefinition(const CameraDefinition &) = delete;
    const CameraDefinition &operator=(const CameraDefinition &) = delete;

private:
    tinyxml2::XMLDocument _doc;

    std::map<std::string, MavlinkParameters::ParamValue> _current_settings = {};
};

} // namespace dronecore
