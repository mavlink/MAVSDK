#include "libmav_receiver.h"
#include "mavsdk_impl.h"
#include <mav/MessageSet.h>
#include <mav/BufferParser.h>
#include <json/json.h>
#include <variant>
#include <cstring>
#include <sstream>
#include <charconv>
#include <cmath>
#include "log.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// Check if std::to_chars supports floating-point types
// - GCC 11+ (libstdc++) - use __GNUC__ version, not __GLIBCXX__ date
// - MSVC 2019 16.4+
// - macOS with libc++ 14+
// - iOS: disabled because Apple marks it unavailable for iOS < 16.3
#if defined(__APPLE__) && (TARGET_OS_IOS || TARGET_OS_TV || TARGET_OS_WATCH)
// Disable on iOS/tvOS/watchOS - function is marked unavailable for older deployment targets
#define MAVSDK_HAS_FLOAT_TO_CHARS 0
#elif defined(__GNUC__) && !defined(__clang__)
// GCC: floating-point to_chars was added in GCC 11
#if __GNUC__ >= 11
#define MAVSDK_HAS_FLOAT_TO_CHARS 1
#else
#define MAVSDK_HAS_FLOAT_TO_CHARS 0
#endif
#elif defined(_LIBCPP_VERSION)
// libc++ has had floating-point to_chars since version 14 (macOS 13.3+)
#if _LIBCPP_VERSION >= 14000
#define MAVSDK_HAS_FLOAT_TO_CHARS 1
#else
#define MAVSDK_HAS_FLOAT_TO_CHARS 0
#endif
#elif defined(_MSC_VER)
// MSVC has had it since VS 2019 16.4
#if _MSC_VER >= 1924
#define MAVSDK_HAS_FLOAT_TO_CHARS 1
#else
#define MAVSDK_HAS_FLOAT_TO_CHARS 0
#endif
#else
#define MAVSDK_HAS_FLOAT_TO_CHARS 0
#endif

namespace mavsdk {

// Original implementation using std::ostream << for float/double conversion.
// Kept for benchmark comparison.
template<typename T> void value_to_json_stream(std::ostream& json_stream, const T& value)
{
    if constexpr (
        std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> ||
        std::is_same_v<T, uint64_t> || std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
        std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>) {
        json_stream << static_cast<int64_t>(value);
    } else if constexpr (std::is_same_v<T, char>) {
        json_stream << static_cast<int>(value);
    } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
        if (!std::isfinite(value)) {
            json_stream << "null";
        } else {
            json_stream << value;
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        json_stream << "\"" << value << "\"";
    } else if constexpr (
        std::is_same_v<T, std::vector<uint8_t>> || std::is_same_v<T, std::vector<int8_t>>) {
        // Handle uint8_t/int8_t vectors specially to avoid character output
        json_stream << "[";
        bool first = true;
        for (const auto& elem : value) {
            if (!first)
                json_stream << ",";
            first = false;
            json_stream << static_cast<int>(elem);
        }
        json_stream << "]";
    } else if constexpr (
        std::is_same_v<T, std::vector<uint16_t>> || std::is_same_v<T, std::vector<uint32_t>> ||
        std::is_same_v<T, std::vector<uint64_t>> || std::is_same_v<T, std::vector<int16_t>> ||
        std::is_same_v<T, std::vector<int32_t>> || std::is_same_v<T, std::vector<int64_t>>) {
        // Handle integer vector types
        json_stream << "[";
        bool first = true;
        for (const auto& elem : value) {
            if (!first)
                json_stream << ",";
            first = false;
            json_stream << elem;
        }
        json_stream << "]";
    } else if constexpr (
        std::is_same_v<T, std::vector<float>> || std::is_same_v<T, std::vector<double>>) {
        // Handle float/double vector types with NaN check
        json_stream << "[";
        bool first = true;
        for (const auto& elem : value) {
            if (!first)
                json_stream << ",";
            first = false;
            if (!std::isfinite(elem)) {
                json_stream << "null";
            } else {
                json_stream << elem;
            }
        }
        json_stream << "]";
    } else {
        // Fallback for unknown types
        json_stream << "null";
    }
}

// Optimized implementation using std::to_chars for float/double conversion.
template<typename T> void value_to_json_stream_fast(std::ostream& json_stream, const T& value)
{
    if constexpr (
        std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> ||
        std::is_same_v<T, uint64_t> || std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
        std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>) {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);
        json_stream.write(buf, ptr - buf);
    } else if constexpr (std::is_same_v<T, char>) {
        char buf[8];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), static_cast<int>(value));
        json_stream.write(buf, ptr - buf);
    } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
        if (!std::isfinite(value)) {
            json_stream << "null";
        } else {
#if MAVSDK_HAS_FLOAT_TO_CHARS
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);
            json_stream.write(buf, ptr - buf);
#else
            // Fallback for older compilers without floating-point to_chars
            json_stream << value;
#endif
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        json_stream << "\"" << value << "\"";
    } else if constexpr (
        std::is_same_v<T, std::vector<uint8_t>> || std::is_same_v<T, std::vector<int8_t>>) {
        json_stream << "[";
        bool first = true;
        for (const auto& elem : value) {
            if (!first)
                json_stream << ",";
            first = false;
            char buf[8];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), static_cast<int>(elem));
            json_stream.write(buf, ptr - buf);
        }
        json_stream << "]";
    } else if constexpr (
        std::is_same_v<T, std::vector<uint16_t>> || std::is_same_v<T, std::vector<uint32_t>> ||
        std::is_same_v<T, std::vector<uint64_t>> || std::is_same_v<T, std::vector<int16_t>> ||
        std::is_same_v<T, std::vector<int32_t>> || std::is_same_v<T, std::vector<int64_t>>) {
        json_stream << "[";
        bool first = true;
        for (const auto& elem : value) {
            if (!first)
                json_stream << ",";
            first = false;
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), elem);
            json_stream.write(buf, ptr - buf);
        }
        json_stream << "]";
    } else if constexpr (
        std::is_same_v<T, std::vector<float>> || std::is_same_v<T, std::vector<double>>) {
        json_stream << "[";
        bool first = true;
        for (const auto& elem : value) {
            if (!first)
                json_stream << ",";
            first = false;
            if (!std::isfinite(elem)) {
                json_stream << "null";
            } else {
#if MAVSDK_HAS_FLOAT_TO_CHARS
                char buf[32];
                auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), elem);
                json_stream.write(buf, ptr - buf);
#else
                // Fallback for older compilers without floating-point to_chars
                json_stream << elem;
#endif
            }
        }
        json_stream << "]";
    } else {
        // Fallback for unknown types
        json_stream << "null";
    }
}

// Explicit instantiations for benchmark testing - original
template void value_to_json_stream<float>(std::ostream&, const float&);
template void value_to_json_stream<double>(std::ostream&, const double&);
template void value_to_json_stream<int8_t>(std::ostream&, const int8_t&);
template void value_to_json_stream<int16_t>(std::ostream&, const int16_t&);
template void value_to_json_stream<int32_t>(std::ostream&, const int32_t&);
template void value_to_json_stream<int64_t>(std::ostream&, const int64_t&);
template void value_to_json_stream<uint8_t>(std::ostream&, const uint8_t&);
template void value_to_json_stream<uint16_t>(std::ostream&, const uint16_t&);
template void value_to_json_stream<uint32_t>(std::ostream&, const uint32_t&);
template void value_to_json_stream<uint64_t>(std::ostream&, const uint64_t&);
template void value_to_json_stream<std::string>(std::ostream&, const std::string&);
template void value_to_json_stream<std::vector<float>>(std::ostream&, const std::vector<float>&);
template void value_to_json_stream<std::vector<double>>(std::ostream&, const std::vector<double>&);

// Explicit instantiations for benchmark testing - optimized
template void value_to_json_stream_fast<float>(std::ostream&, const float&);
template void value_to_json_stream_fast<double>(std::ostream&, const double&);
template void value_to_json_stream_fast<int8_t>(std::ostream&, const int8_t&);
template void value_to_json_stream_fast<int16_t>(std::ostream&, const int16_t&);
template void value_to_json_stream_fast<int32_t>(std::ostream&, const int32_t&);
template void value_to_json_stream_fast<int64_t>(std::ostream&, const int64_t&);
template void value_to_json_stream_fast<uint8_t>(std::ostream&, const uint8_t&);
template void value_to_json_stream_fast<uint16_t>(std::ostream&, const uint16_t&);
template void value_to_json_stream_fast<uint32_t>(std::ostream&, const uint32_t&);
template void value_to_json_stream_fast<uint64_t>(std::ostream&, const uint64_t&);
template void value_to_json_stream_fast<std::string>(std::ostream&, const std::string&);
template void
value_to_json_stream_fast<std::vector<float>>(std::ostream&, const std::vector<float>&);
template void
value_to_json_stream_fast<std::vector<double>>(std::ostream&, const std::vector<double>&);

LibmavReceiver::LibmavReceiver(MavsdkImpl& mavsdk_impl) : _mavsdk_impl(mavsdk_impl)
{
    // No need for individual BufferParser - we'll use MavsdkImpl's thread-safe parsing

    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
}

LibmavReceiver::~LibmavReceiver() = default;

void LibmavReceiver::set_new_datagram(char* datagram, unsigned datagram_len)
{
    _datagram = datagram;
    _datagram_len = datagram_len;
}

bool LibmavReceiver::parse_message()
{
    if (!_datagram || _datagram_len == 0) {
        return false;
    }

    // Use libmav to parse messages directly from the buffer
    return parse_libmav_message_from_buffer(
        reinterpret_cast<const uint8_t*>(_datagram), _datagram_len);
}

bool LibmavReceiver::parse_libmav_message_from_buffer(const uint8_t* buffer, size_t buffer_len)
{
    size_t bytes_consumed = 0;

    // Use thread-safe parsing from MavsdkImpl (handles MessageSet synchronization internally)
    auto message_opt = _mavsdk_impl.parse_message_safe(buffer, buffer_len, bytes_consumed);

    if (!message_opt) {
        return false; // No complete message found
    }

    auto message = message_opt.value();

    if (_debugging) {
        LogDebug() << "Parsed message: " << message.name() << " (ID: " << message.id() << ")";
    }

    // Extract system and component IDs from header
    auto header = message.header();

    // Generate complete JSON with all field values
    std::string json = libmav_message_to_json(message);

    // Fill our message structures
    _last_libmav_message = message;
    _last_message.message_name = message.name();
    _last_message.system_id = header.systemId();
    _last_message.component_id = header.componentId();

    // Extract target_system and target_component if present in message fields
    uint8_t target_system_id = 0;
    uint8_t target_component_id = 0;
    if (message.get("target_system", target_system_id) == mav::MessageResult::Success) {
        _last_message.target_system_id = target_system_id;
    } else {
        _last_message.target_system_id = 0;
    }
    if (message.get("target_component", target_component_id) == mav::MessageResult::Success) {
        _last_message.target_component_id = target_component_id;
    } else {
        _last_message.target_component_id = 0;
    }

    _last_message.fields_json = json;

    // Clear the original datagram since we processed it
    _datagram = nullptr;
    _datagram_len = 0;

    return true;
}

std::string LibmavReceiver::libmav_message_to_json(const mav::Message& msg) const
{
    std::ostringstream json_stream;
    json_stream << "{";
    json_stream << "\"message_id\":" << msg.id();
    json_stream << ",\"message_name\":\"" << msg.name() << "\"";

    // Get message definition to iterate through all fields (using thread-safe method)
    auto message_def_opt = _mavsdk_impl.get_message_definition_safe(static_cast<int>(msg.id()));

    if (message_def_opt) {
        auto& message_def = message_def_opt.get();

        // Get field names and iterate through them
        auto field_names = message_def.fieldNames();
        for (const auto& field_name : field_names) {
            json_stream << ",\"" << field_name << "\":";

            // Extract field value based on type and convert to JSON
            auto variant_opt = msg.getAsNativeTypeInVariant(field_name);
            if (variant_opt) {
                const auto& variant = variant_opt.value();

                // Convert variant to JSON string using extracted function
                std::visit(
                    [&json_stream](const auto& value) { value_to_json_stream(json_stream, value); },
                    variant);
            } else {
                // Field not present or failed to extract
                json_stream << "null";
            }
        }
    }

    json_stream << "}";
    return json_stream.str();
}

std::optional<std::string> LibmavReceiver::message_id_to_name(uint32_t id) const
{
    return _mavsdk_impl.message_id_to_name_safe(id);
}

std::optional<int> LibmavReceiver::message_name_to_id(const std::string& name) const
{
    return _mavsdk_impl.message_name_to_id_safe(name);
}

std::optional<mav::Message> LibmavReceiver::create_message(const std::string& message_name) const
{
    return _mavsdk_impl.create_message_safe(message_name);
}

bool LibmavReceiver::load_custom_xml(const std::string& xml_content)
{
    // Note: This method should not be called directly on LibmavReceiver instances.
    // Use MavlinkDirect::load_custom_xml() instead which goes through proper channels.

    // Use thread-safe method from MavsdkImpl
    return _mavsdk_impl.load_custom_xml_to_message_set(xml_content);
}

} // namespace mavsdk
