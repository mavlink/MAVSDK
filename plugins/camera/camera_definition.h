#pragma once

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

    struct ParameterValue {
        union {
            float as_float;
            uint32_t as_uint32;
        } value;
    };

    enum class Type {
        FLOAT,
        UINT32
    } type;

    struct ParameterOption {
        std::string name;
        ParameterValue value;
    };

    typedef std::vector<std::shared_ptr<ParameterOption>> option_list_t;

    struct Parameter {
        std::string description;
        Type type;
        option_list_t options;
    };

    void update_setting(const std::string &name, const ParameterValue &value);

    typedef std::map<std::string, std::shared_ptr<Parameter>> parameter_map_t;
    bool get_parameters(parameter_map_t &parameters, bool filter_possible);

    // Non-copyable
    CameraDefinition(const CameraDefinition &) = delete;
    const CameraDefinition &operator=(const CameraDefinition &) = delete;

private:
    tinyxml2::XMLDocument _doc;

    std::map<std::string, ParameterValue> _current_settings = {};
};

} // namespace dronecore
