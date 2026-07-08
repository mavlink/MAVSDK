#include "libmav_conversions.hpp"

#include "log.hpp"

#include <mav/Message.h>
#include <json/json.h>
#include <cmath>
#include <limits>
#include <vector>

namespace mavsdk {

bool json_to_libmav_message(const std::string& json_string, mav::Message& msg)
{
    Json::Value json;
    Json::Reader reader;

    if (!reader.parse(json_string, json)) {
        LogErr("Failed to parse JSON: {}", json_string);
        return false;
    }

    // Iterate through all JSON fields and set them in the libmav message
    for (const auto& field_name : json.getMemberNames()) {
        const Json::Value& field_value = json[field_name];

        // Convert JSON values to appropriate types and set in message
        // libmav handles type casting based on field definition, so we just need to pass
        // int64/uint64
        if (field_value.isInt() || field_value.isInt64()) {
            int64_t value = field_value.asInt64();
            auto result = msg.set(field_name, value);
            if (result != ::mav::MessageResult::Success) {
                LogWarn("Failed to set integer field {} = {}", field_name, value);
            }
        } else if (field_value.isUInt() || field_value.isUInt64()) {
            uint64_t value = field_value.asUInt64();
            auto result = msg.set(field_name, value);
            if (result != ::mav::MessageResult::Success) {
                LogWarn("Failed to set unsigned integer field {} = {}", field_name, value);
            }
        } else if (field_value.isNull() || field_value.isDouble()) {
            // Handle float/double values (including null -> NaN)
            auto field_opt = msg.type().getField(field_name);
            if (!field_opt) {
                LogWarn("Field {} not found in message definitio", field_name);
                continue;
            }

            auto field = field_opt.value();
            ::mav::MessageResult result = ::mav::MessageResult::FieldNotFound;

            if (field.type.base_type == ::mav::FieldType::BaseType::FLOAT) {
                if (field_value.isNull()) {
                    result = msg.set(field_name, std::numeric_limits<float>::quiet_NaN());
                } else {
                    result = msg.set(field_name, static_cast<float>(field_value.asFloat()));
                }
            } else if (field.type.base_type == ::mav::FieldType::BaseType::DOUBLE) {
                if (field_value.isNull()) {
                    result = msg.set(field_name, std::numeric_limits<double>::quiet_NaN());
                } else {
                    result = msg.set(field_name, field_value.asDouble());
                }
            } else {
                LogWarn("Field {} is not a float or double field", field_name);
                continue;
            }

            if (result != ::mav::MessageResult::Success) {
                LogWarn(
                    "Failed to set float/double field {} = {}",
                    field_name,
                    (field_value.isNull() ? "null" : std::to_string(field_value.asDouble())));
            }
        } else if (field_value.isString()) {
            auto result = msg.setString(field_name, field_value.asString());
            if (result != ::mav::MessageResult::Success) {
                LogWarn("Failed to set string field {} = {}", field_name, field_value.asString());
            }
        } else if (field_value.isArray()) {
            // Handle array fields with proper type detection
            auto array_size = field_value.size();

            // Get field definition to determine correct type
            auto field_opt = msg.type().getField(field_name);
            if (!field_opt) {
                LogWarn("Field {} not found in message definitio", field_name);
                continue;
            }

            auto field = field_opt.value();
            if (field.type.size <= 1) {
                LogWarn("Field {} is not an array field", field_name);
                continue;
            }

            // Create and populate only the correct vector type
            ::mav::MessageResult result = ::mav::MessageResult::FieldNotFound;
            switch (field.type.base_type) {
                case ::mav::FieldType::BaseType::UINT8: {
                    std::vector<uint8_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<uint8_t>(elem.asUInt()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::UINT16: {
                    std::vector<uint16_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<uint16_t>(elem.asUInt()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::UINT32: {
                    std::vector<uint32_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<uint32_t>(elem.asUInt()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::UINT64: {
                    std::vector<uint64_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(
                            elem.isNumeric() ? static_cast<uint64_t>(elem.asUInt64()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT8: {
                    std::vector<int8_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<int8_t>(elem.asInt()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT16: {
                    std::vector<int16_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<int16_t>(elem.asInt()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT32: {
                    std::vector<int32_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<int32_t>(elem.asInt()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT64: {
                    std::vector<int64_t> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<int64_t>(elem.asInt64()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::FLOAT: {
                    std::vector<float> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        if (elem.isNumeric()) {
                            vec.push_back(static_cast<float>(elem.asFloat()));
                        } else {
                            // For non-numeric values (including null), use NaN
                            vec.push_back(std::numeric_limits<float>::quiet_NaN());
                        }
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::DOUBLE: {
                    std::vector<double> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        if (elem.isNumeric()) {
                            vec.push_back(elem.asDouble());
                        } else {
                            // For non-numeric values (including null), use NaN
                            vec.push_back(std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::CHAR: {
                    std::vector<char> vec;
                    vec.reserve(array_size);
                    for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                        const auto& elem = field_value[i];
                        vec.push_back(elem.isNumeric() ? static_cast<char>(elem.asInt()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                default:
                    LogWarn("Unsupported array field type for {}", field_name);
                    break;
            }

            if (result != ::mav::MessageResult::Success) {
                LogWarn("Failed to set array field {}", field_name);
            }
        } else {
            LogWarn("Unsupported JSON field type for {}", field_name);
        }
    }

    return true;
}

} // namespace mavsdk
