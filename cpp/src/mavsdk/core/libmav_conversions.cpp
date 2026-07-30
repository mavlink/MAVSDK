#include "libmav_conversions.hpp"

#include "log.hpp"

#include <mav/Message.h>
#include <nlohmann/json.hpp>
#include <cmath>
#include <limits>
#include <vector>

namespace mavsdk {

bool json_to_libmav_message(const std::string& json_string, mav::Message& msg)
{
    // Parse without exceptions (MAVSDK is built with -fno-exceptions); a parse
    // error yields a discarded value instead of throwing.
    auto json = nlohmann::json::parse(json_string, nullptr, false);

    if (json.is_discarded() || !json.is_object()) {
        LogErr("Failed to parse JSON: {}", json_string);
        return false;
    }

    // Iterate through all JSON fields and set them in the libmav message
    for (const auto& [field_name, field_value] : json.items()) {
        // Convert JSON values to appropriate types and set in message
        // libmav handles type casting based on field definition, so we just need to pass
        // int64/uint64. Check unsigned first since nlohmann stores non-negative
        // integer literals as unsigned.
        if (field_value.is_number_unsigned()) {
            uint64_t value = field_value.get<uint64_t>();
            auto result = msg.set(field_name, value);
            if (result != ::mav::MessageResult::Success) {
                LogWarn("Failed to set unsigned integer field {} = {}", field_name, value);
            }
        } else if (field_value.is_number_integer()) {
            int64_t value = field_value.get<int64_t>();
            auto result = msg.set(field_name, value);
            if (result != ::mav::MessageResult::Success) {
                LogWarn("Failed to set integer field {} = {}", field_name, value);
            }
        } else if (field_value.is_null() || field_value.is_number_float()) {
            // Handle float/double values (including null -> NaN)
            auto field_opt = msg.type().getField(field_name);
            if (!field_opt) {
                LogWarn("Field {} not found in message definitio", field_name);
                continue;
            }

            auto field = field_opt.value();
            ::mav::MessageResult result = ::mav::MessageResult::FieldNotFound;

            if (field.type.base_type == ::mav::FieldType::BaseType::FLOAT) {
                if (field_value.is_null()) {
                    result = msg.set(field_name, std::numeric_limits<float>::quiet_NaN());
                } else {
                    result = msg.set(field_name, field_value.get<float>());
                }
            } else if (field.type.base_type == ::mav::FieldType::BaseType::DOUBLE) {
                if (field_value.is_null()) {
                    result = msg.set(field_name, std::numeric_limits<double>::quiet_NaN());
                } else {
                    result = msg.set(field_name, field_value.get<double>());
                }
            } else {
                LogWarn("Field {} is not a float or double field", field_name);
                continue;
            }

            if (result != ::mav::MessageResult::Success) {
                LogWarn(
                    "Failed to set float/double field {} = {}",
                    field_name,
                    (field_value.is_null() ? "null" : std::to_string(field_value.get<double>())));
            }
        } else if (field_value.is_string()) {
            auto value = field_value.get<std::string>();
            auto result = msg.setString(field_name, value);
            if (result != ::mav::MessageResult::Success) {
                LogWarn("Failed to set string field {} = {}", field_name, value);
            }
        } else if (field_value.is_array()) {
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
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<uint8_t>(elem.get<uint64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::UINT16: {
                    std::vector<uint16_t> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<uint16_t>(elem.get<uint64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::UINT32: {
                    std::vector<uint32_t> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<uint32_t>(elem.get<uint64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::UINT64: {
                    std::vector<uint64_t> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<uint64_t>(elem.get<uint64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT8: {
                    std::vector<int8_t> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<int8_t>(elem.get<int64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT16: {
                    std::vector<int16_t> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<int16_t>(elem.get<int64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT32: {
                    std::vector<int32_t> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<int32_t>(elem.get<int64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::INT64: {
                    std::vector<int64_t> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<int64_t>(elem.get<int64_t>()) : 0);
                    }
                    result = msg.set(field_name, vec);
                    break;
                }
                case ::mav::FieldType::BaseType::FLOAT: {
                    std::vector<float> vec;
                    vec.reserve(array_size);
                    for (const auto& elem : field_value) {
                        if (elem.is_number()) {
                            vec.push_back(elem.get<float>());
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
                    for (const auto& elem : field_value) {
                        if (elem.is_number()) {
                            vec.push_back(elem.get<double>());
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
                    for (const auto& elem : field_value) {
                        vec.push_back(
                            elem.is_number() ? static_cast<char>(elem.get<int64_t>()) : 0);
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
