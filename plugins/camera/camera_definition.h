#pragma once

#include "mavlink_parameters.h"
#include <tinyxml2.h>
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace dronecore {

class CameraDefinition {
public:
    CameraDefinition();
    ~CameraDefinition();

    bool load_file(const std::string &filepath);
    bool load_string(const std::string &content);

    std::string get_vendor() const;
    std::string get_model() const;

    void assume_default_settings();

    struct Setting {
        std::string name;
        MAVLinkParameters::ParamValue value;
    };

    bool set_setting(const std::string &name, const MAVLinkParameters::ParamValue &value);
    bool get_setting(const std::string &name, MAVLinkParameters::ParamValue &value);
    bool get_all_settings(std::map<std::string, MAVLinkParameters::ParamValue> &settings);
    bool get_possible_settings(std::map<std::string, MAVLinkParameters::ParamValue> &settings);

    bool get_option_value(const std::string &param_name,
                          const std::string &option_value,
                          MAVLinkParameters::ParamValue &value);
    bool get_all_options(const std::string &name,
                         std::vector<MAVLinkParameters::ParamValue> &values);
    bool get_possible_options(const std::string &name,
                              std::vector<MAVLinkParameters::ParamValue> &values);

    bool get_setting_str(const std::string &setting_name, std::string &description);
    bool get_option_str(const std::string &setting_name,
                        const std::string &option_name,
                        std::string &description);

    bool get_unknown_params(std::vector<std::string> &params);
    void set_all_params_unknown();

    // Non-copyable
    CameraDefinition(const CameraDefinition &) = delete;
    const CameraDefinition &operator=(const CameraDefinition &) = delete;

private:
    typedef std::map<std::string, MAVLinkParameters::ParamValue> parameter_range_t;

    struct Option {
        std::string name;
        MAVLinkParameters::ParamValue value;
        std::vector<std::string> exclusions;
        std::map<std::string, parameter_range_t> parameter_ranges;
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

    tinyxml2::XMLDocument _doc{};

    std::map<std::string, std::shared_ptr<Parameter>> _parameter_map;

    struct InternalCurrentSetting {
        MAVLinkParameters::ParamValue value;
        bool needs_updating;
    };

    std::map<std::string, InternalCurrentSetting> _current_settings;

    std::string _model;
    std::string _vendor;
};

} // namespace dronecore
