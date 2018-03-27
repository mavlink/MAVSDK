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

    typedef std::map<std::string, MavlinkParameters::ParamValue> roption_t;

    typedef std::map<std::string, roption_t> parameter_range_t;

    struct Option {
        std::string name;
        MavlinkParameters::ParamValue value;
        std::vector<std::string> exclusions;
        std::vector<parameter_range_t> parameter_ranges;
        bool is_default;
    };

    struct Parameter {
        std::string description;
        bool is_control;
        bool is_readonly;
        bool is_writeonly;
        std::vector<std::string> updates;
        std::vector<std::shared_ptr<Option>> options;
    };

    typedef std::map<std::string, std::shared_ptr<Parameter>> parameter_map_t;

    parameter_map_t _settings;

    void update_setting(const std::string &name, const MavlinkParameters::ParamValue &value);

    bool get_parameters(parameter_map_t &parameters, bool filter_possible);

    // Non-copyable
    CameraDefinition(const CameraDefinition &) = delete;
    const CameraDefinition &operator=(const CameraDefinition &) = delete;

private:
    tinyxml2::XMLDocument _doc {};

    std::map<std::string, MavlinkParameters::ParamValue> _current_settings = {};
};

} // namespace dronecore
