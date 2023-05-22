#pragma once

#include "mavlink_parameter_client.h"
#include <tinyxml2.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>
#include <tuple>
#include <utility>

namespace mavsdk {

class CameraDefinition {
public:
    CameraDefinition();
    ~CameraDefinition();

    bool load_file(const std::string& filepath);
    bool load_string(const std::string& content);

    std::string get_vendor() const;
    std::string get_model() const;

    void assume_default_settings();

    struct Setting {
        std::string name;
        ParamValue value;
    };

    bool set_setting(const std::string& name, const ParamValue& value);
    bool get_setting(const std::string& name, ParamValue& value);
    bool get_all_settings(std::unordered_map<std::string, ParamValue>& settings);
    bool get_possible_settings(std::unordered_map<std::string, ParamValue>& settings);

    bool get_option_value(
        const std::string& param_name, const std::string& option_value, ParamValue& value);
    bool get_all_options(const std::string& name, std::vector<ParamValue>& values);
    bool get_possible_options(const std::string& name, std::vector<ParamValue>& values);

    bool is_setting_range(const std::string& name);

    bool get_setting_str(const std::string& setting_name, std::string& description);
    bool get_option_str(
        const std::string& setting_name, const std::string& option_name, std::string& description);

    void get_unknown_params(std::vector<std::pair<std::string, ParamValue>>& params);
    void set_all_params_unknown();

    // Non-copyable
    CameraDefinition(const CameraDefinition&) = delete;
    const CameraDefinition& operator=(const CameraDefinition&) = delete;

private:
    bool get_possible_settings_locked(std::unordered_map<std::string, ParamValue>& settings);

    using ParameterRange = std::unordered_map<std::string, ParamValue>;

    struct Option {
        std::string name{};
        ParamValue value{};
        std::vector<std::string> exclusions{};
        std::unordered_map<std::string, ParameterRange> parameter_ranges{};
    };

    struct Parameter {
        std::string description{};
        bool is_control{false};
        bool is_readonly{false};
        bool is_writeonly{false};
        ParamValue type{}; // for type only, doesn't hold a value
        std::vector<std::string> updates{};
        std::vector<std::shared_ptr<Option>> options{};
        Option default_option{};
        bool is_range{false};
    };

    bool parse_xml();

    // Until we have std::optional we need to use std::pair to return something that might be
    // nothing.
    std::pair<bool, std::vector<std::shared_ptr<Option>>> parse_options(
        const tinyxml2::XMLElement* options_handle,
        const std::string& param_name,
        std::unordered_map<std::string, std::string>& type_map);
    std::tuple<bool, std::vector<std::shared_ptr<Option>>, Option> parse_range_options(
        const tinyxml2::XMLElement* param_handle,
        const std::string& param_name,
        std::unordered_map<std::string, std::string>& type_map);
    std::pair<bool, Option> find_default(
        const std::vector<std::shared_ptr<Option>>& options, const std::string& default_str);

    mutable std::mutex _mutex{};

    tinyxml2::XMLDocument _doc{};

    std::unordered_map<std::string, std::shared_ptr<Parameter>> _parameter_map{};

    struct InternalCurrentSetting {
        ParamValue value{};
        bool needs_updating{false};
    };

    std::unordered_map<std::string, InternalCurrentSetting> _current_settings{};

    std::string _model{};
    std::string _vendor{};
};

} // namespace mavsdk
