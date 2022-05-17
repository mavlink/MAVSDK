#pragma once

#include "mavlink_include.h"
#include "log.h"
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace mavsdk {

// std::to_string doesn't work for std::string, so we need this workaround.
template<typename T> std::string to_string(T&& value)
{
    return std::to_string(std::forward<T>(value));
}

inline std::string& to_string(std::string& value)
{
    return value;
}

class ParamValue {
public:
    bool set_from_mavlink_param_value_bytewise(const mavlink_param_value_t& mavlink_value);
    bool set_from_mavlink_param_value_cast(const mavlink_param_value_t& mavlink_value);
    bool set_from_mavlink_param_set_bytewise(const mavlink_param_set_t& mavlink_set);
    bool set_from_mavlink_param_ext_set(const mavlink_param_ext_set_t& mavlink_ext_set);
    bool set_from_mavlink_param_ext_value(const mavlink_param_ext_value_t& mavlink_ext_value);
    bool set_from_xml(const std::string& type_str, const std::string& value_str);
    bool set_empty_type_from_xml(const std::string& type_str);

    [[nodiscard]] MAV_PARAM_TYPE get_mav_param_type() const;
    [[nodiscard]] MAV_PARAM_EXT_TYPE get_mav_param_ext_type() const;

    bool set_as_same_type(const std::string& value_str);

    [[nodiscard]] float get_4_float_bytes_bytewise() const;
    [[nodiscard]] float get_4_float_bytes_cast() const;

    [[nodiscard]] std::optional<int> get_int() const;
    [[nodiscard]] std::optional<float> get_float() const;
    [[nodiscard]] std::optional<std::string> get_custom() const;

    bool set_int(int new_value);
    void set_float(float new_value);
    void set_custom(const std::string& new_value);

    std::array<char, 128> get_128_bytes() const;

    [[nodiscard]] std::string get_string() const;

    template<typename T>[[nodiscard]] bool is() const
    {
        return (std::get_if<T>(&_value) != nullptr);
    }

    template<typename T> T get() const { return std::get<T>(_value); }

    template<typename T> void set(T new_value) { _value = new_value; }

    [[nodiscard]] bool is_same_type(const ParamValue& rhs) const;

    bool operator==(const ParamValue& rhs) const
    {
        if (!is_same_type(rhs)) {
            LogWarn() << "Trying to compare different types.";
            return false;
        }

        return _value == rhs._value;
    }

    bool operator<(const ParamValue& rhs) const
    {
        if (!is_same_type(rhs)) {
            LogWarn() << "Trying to compare different types.";
            return false;
        }

        return _value < rhs._value;
    }

    bool operator>(const ParamValue& rhs) const
    {
        if (!is_same_type(rhs)) {
            LogWarn() << "Trying to compare different types.";
            return false;
        }

        return _value > rhs._value;
    }

    bool operator==(const std::string& value_str) const;

    [[nodiscard]] std::string typestr() const;

    std::variant<
        uint8_t,
        int8_t,
        uint16_t,
        int16_t,
        uint32_t,
        int32_t,
        uint64_t,
        int64_t,
        float,
        double,
        std::string>
        _value{};
};

std::ostream& operator<<(std::ostream& strm, const ParamValue& obj);

} // namespace mavsdk