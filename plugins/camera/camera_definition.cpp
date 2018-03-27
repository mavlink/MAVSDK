#include "global_include.h"
#include "log.h"
#include "camera_definition.h"

namespace dronecore {

CameraDefinition::CameraDefinition() :
    _doc()
{
}

CameraDefinition::~CameraDefinition()
{
}

void CameraDefinition::load_file(const char *filename)
{
    _doc.LoadFile(filename);
}

void CameraDefinition::load_string(const std::string &content)
{
    _doc.Parse(content.c_str());
}

const char *CameraDefinition::get_model() const
{
    return _doc.FirstChildElement("mavlinkcamera")->
           FirstChildElement("definition")->
           FirstChildElement("model")->GetText();
}

const char *CameraDefinition::get_vendor() const
{
    return _doc.FirstChildElement("mavlinkcamera")->
           FirstChildElement("definition")->
           FirstChildElement("vendor")->GetText();
}

bool CameraDefinition::get_parameters(parameter_map_t &parameters, bool filter_possible)
{
    // Reset input/output first.
    parameters.clear();

    auto e_mavlinkcamera = _doc.FirstChildElement("mavlinkcamera");
    if (!e_mavlinkcamera) {
        return false;
    }

    auto e_parameters = e_mavlinkcamera->FirstChildElement("parameters");
    if (!e_parameters) {
        return false;
    }

    for (auto e_parameter = e_parameters->FirstChildElement("parameter");
         e_parameter != nullptr;
         e_parameter = e_parameter->NextSiblingElement("parameter")) {

        const char *param_name = e_parameter->Attribute("name");
        if (!param_name) {
            return false;
        }

        const char *param_type = e_parameter->Attribute("type");
        if (!param_type) {
            return false;
        }

        // By default control is on.
        bool is_control = true;
        const char *control_str = e_parameter->Attribute("control");
        if (control_str) {
            if (strcmp(control_str, "0") == 0) {
                is_control = false;
            }
        }

        bool is_readonly = false;
        const char *readonly_str = e_parameter->Attribute("readonly");
        if (readonly_str) {
            if (strcmp(readonly_str, "1") == 0) {
                is_readonly = true;
            }
        }

        bool is_writeonly = false;
        const char *writeonly_str = e_parameter->Attribute("writeonly");
        if (writeonly_str) {
            if (strcmp(writeonly_str, "1") == 0) {
                is_writeonly = true;
            }
        }

        if (is_readonly && is_writeonly) {
            return false;
        }

        const char *type_str = e_parameter->Attribute("type");
        if (!type_str) {
            return false;
        }

        // Be definition custom types do not have control.
        if (strcmp(type_str, "custom") == 0) {
            is_control = false;
        }

        auto e_description = e_parameter->FirstChildElement("description");
        if (!e_description) {
            return false;
        }

        LogDebug() << "Found: " << e_description->GetText()
                   << " (" << param_name
                   << ", control: " << (is_control ? "yes" : "no")
                   << ", readonly: " << (is_readonly ? "yes" : "no")
                   << ", writeonly: " << (is_writeonly ? "yes" : "no")
                   << ")";

        // TODO: parse default which is of param type.

        auto e_updates = e_parameter->FirstChildElement("updates");
        if (e_updates) {
            for (auto e_update = e_updates->FirstChildElement("update");
                 e_update != nullptr;
                 e_update = e_update->NextSiblingElement("update")) {

                LogDebug() << "Updates: " << e_update->GetText();
            }
        }


        auto e_options = e_parameter->FirstChildElement("options");
        if (!e_options) {
            LogDebug() << "No options found";
            continue;
        }

        for (auto e_option = e_options->FirstChildElement("option");
             e_option != nullptr;
             e_option = e_option->NextSiblingElement("option")) {

            const char *option_name = e_option->Attribute("name");
            if (!option_name) {
                LogErr() << "no option name given";
                return false;
            }

            auto new_param = std::make_shared<MavlinkParameters::ParamValue>();
            new_param->set_from_xml(type_str, option_name);

            LogDebug() << "Type: " << type_str << ", name: " << option_name;

            auto e_exclusions = e_option->FirstChildElement("exclusions");
            if (e_exclusions) {
                for (auto e_exclude = e_exclusions->FirstChildElement("exclude");
                     e_exclude != nullptr;
                     e_exclude = e_exclude->NextSiblingElement("exclude")) {

                    LogDebug() << "Exclude: " << e_exclude->GetText();
                }
            }

            auto e_parameterranges = e_option->FirstChildElement("parameterranges");
            if (e_parameterranges) {
                for (auto e_parameterrange = e_parameterranges->FirstChildElement("parameterrange");
                     e_parameterrange != nullptr;
                     e_parameterrange = e_parameterrange->NextSiblingElement("parameterrange")) {

                    for (auto e_roption = e_parameterrange->FirstChildElement("roption");
                         e_roption != nullptr;
                         e_roption = e_roption->NextSiblingElement("roption")) {

                        LogDebug() << "range option: "
                                   << e_roption->Attribute("name")
                                   << " -> "
                                   << e_roption->Attribute("value");
                    }
                }
            }

            // TODO: do something smart here
        }

    }

    return true;
}

void CameraDefinition::update_setting(const std::string &name,
                                      const MavlinkParameters::ParamValue &value)
{
    UNUSED(name);
    UNUSED(value);
#if 0
    if (_current_settings.find(name) == _current_settings.end()) {
        _current_settings.insert(
            std::pair<std::string, ParameterValue>
            (name, value));
    } else {
        _current_settings[name] = value;
    }
#endif
}


} // namespace dronecore
