//
// Created by consti10 on 17.06.22.
//

#ifndef MAVSDK_SRC_MAVSDK_CORE_MAVLINK_PARAM_VALUE_H_
#define MAVSDK_SRC_MAVSDK_CORE_MAVLINK_PARAM_VALUE_H_

#include "mavlink_include.h"
#include "log.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <functional>
#include <cassert>
#include <vector>
#include <optional>
#include <variant>

namespace mavsdk {
namespace parameters {

/**
 * This class wraps a mavlink extended or non-extended parameter value.
 * For now, it has just been refactored out of mavlink_parameters.h for better readability.
 * In general, I think this class should expose the following behaviour:
 * While the parameter value can change (e.g. a set changes the internally hold value)
 * the type of the internal value can never change.
 *
 * For example, it is possible to change an
 * int=0 to int=1 but it is NOT POSSIBLE to change an int=0 to a float=1.0.
 *
 * Maybe we can do that compiler type safe in c++, but I am not sure.
 */
class ParamValue {
 public:
  bool set_from_mavlink_param_value_bytewise(const mavlink_param_value_t &mavlink_value);
  bool set_from_mavlink_param_value_cast(const mavlink_param_value_t &mavlink_value);
  bool set_from_mavlink_param_set_bytewise(const mavlink_param_set_t &mavlink_set);
  bool set_from_mavlink_param_ext_set(const mavlink_param_ext_set_t &mavlink_ext_set);
  bool set_from_mavlink_param_ext_value(const mavlink_param_ext_value_t &mavlink_ext_value);
  bool set_from_xml(const std::string &type_str, const std::string &value_str);
  bool set_empty_type_from_xml(const std::string &type_str);

  [[nodiscard]] MAV_PARAM_TYPE get_mav_param_type() const;
  [[nodiscard]] MAV_PARAM_EXT_TYPE get_mav_param_ext_type() const;

  bool set_as_same_type(const std::string &value_str);

  [[nodiscard]] float get_4_float_bytes_bytewise() const;
  [[nodiscard]] float get_4_float_bytes_cast() const;

  [[nodiscard]] std::optional<int> get_int() const;
  [[nodiscard]] std::optional<float> get_float() const;
  [[nodiscard]] std::optional<std::string> get_custom() const;

  bool set_int(int new_value);
  void set_float(float new_value);
  void set_custom(const std::string &new_value);

  std::array<char, 128> get_128_bytes() const;

  [[nodiscard]] std::string get_string() const;

  template<typename T>
  [[nodiscard]] bool is() const {
	return (std::get_if<T>(&_value) != nullptr);
  }

  template<typename T>
  T get() const { return std::get<T>(_value); }

  template<typename T>
  void set(T new_value) { _value = new_value; }

  [[nodiscard]] bool is_same_type(const ParamValue &rhs) const;

  bool operator==(const ParamValue &rhs) const {
	if (!is_same_type(rhs)) {
	  LogWarn() << "Trying to compare different types.";
	  return false;
	}

	return _value == rhs._value;
  }

  bool operator<(const ParamValue &rhs) const {
	if (!is_same_type(rhs)) {
	  LogWarn() << "Trying to compare different types.";
	  return false;
	}

	return _value < rhs._value;
  }

  bool operator>(const ParamValue &rhs) const {
	if (!is_same_type(rhs)) {
	  LogWarn() << "Trying to compare different types.";
	  return false;
	}

	return _value > rhs._value;
  }

  bool operator==(const std::string &value_str) const;

  [[nodiscard]] std::string typestr() const;

  // Consti10: hacky, returns true if this parameter needs the extended parameters' protocol
  // (which is the case when its value is represented by a string)
  bool needs_extended() const {
	// true if it is a string, false otherwise.
	return is<std::string>();
  }
  friend std::ostream& operator<<(std::ostream&, const parameters::ParamValue&);
 private:
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

}
}

#endif //MAVSDK_SRC_MAVSDK_CORE_MAVLINK_PARAM_VALUE_H_