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

    bool load_file(const char *filename);
    bool load_string(const std::string &content);

    std::string get_vendor() const;
    std::string get_model() const;

    void assume_default_settings();

    struct Setting {
        std::string name;
        MavlinkParameters::ParamValue value;
    };

    bool set_setting(const std::string &name, const MavlinkParameters::ParamValue &value);
    bool get_setting(const std::string &name, MavlinkParameters::ParamValue &value) const;

    bool get_all_settings(std::map<std::string, MavlinkParameters::ParamValue> &settings) const;

    // Non-copyable
    CameraDefinition(const CameraDefinition &) = delete;
    const CameraDefinition &operator=(const CameraDefinition &) = delete;

private:
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
    bool parse_xml();

    tinyxml2::XMLDocument _doc {};

    std::map<std::string, std::shared_ptr<Parameter>> _parameter_map;

    std::map<std::string, MavlinkParameters::ParamValue> _current_settings;

    std::string _model;
    std::string _vendor;
};

} // namespace dronecore
