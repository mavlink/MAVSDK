#include "mavlink_direct_impl.h"
#include <mav/Message.h>
#include <mav/MessageSet.h>
#include <variant>
#include <json/json.h>
#include "log.h"
#include "connection.h"

namespace mavsdk {

MavlinkDirectImpl::MavlinkDirectImpl(System& system) : PluginImplBase(system)
{
    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
    _system_impl->register_plugin(this);
}

MavlinkDirectImpl::MavlinkDirectImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
    _system_impl->register_plugin(this);
}

MavlinkDirectImpl::~MavlinkDirectImpl()
{
    _system_impl->unregister_plugin(this);
}

void MavlinkDirectImpl::init()
{
    // No need to initialize MessageSet here - LibmavReceiver handles that
    // No need to register for mavlink messages anymore - we'll use libmav subscription system
}

void MavlinkDirectImpl::deinit() {}

void MavlinkDirectImpl::enable() {}

void MavlinkDirectImpl::disable() {}

MavlinkDirect::Result MavlinkDirectImpl::send_message(MavlinkDirect::MavlinkMessage message)
{
    // Get access to the MessageSet through the system
    auto& message_set = _system_impl->get_message_set();

    // Create libmav message from the message name
    auto libmav_message_opt = message_set.create(message.message_name);
    if (!libmav_message_opt) {
        LogErr() << "Failed to create message: " << message.message_name;
        return MavlinkDirect::Result::InvalidMessage; // Message type not found
    }

    if (_debugging) {
        LogDebug() << "Created message " << message.message_name
                   << " with ID: " << libmav_message_opt.value().id();
    }

    auto libmav_message = libmav_message_opt.value();

    // Convert JSON fields to libmav message fields
    if (!json_to_libmav_message(message.fields_json, libmav_message)) {
        LogErr() << "Failed to convert JSON fields to libmav message for " << message.message_name;
        return MavlinkDirect::Result::InvalidField; // JSON conversion failed
    }

    if (_debugging) {
        LogDebug() << "Successfully populated fields for " << message.message_name;
    }

    // Set target system/component if specified
    if (message.target_system != 0) {
        // For messages that have target_system field, set it
        libmav_message.set("target_system", static_cast<uint8_t>(message.target_system));
    }
    if (message.target_component != 0) {
        // For messages that have target_component field, set it
        libmav_message.set("target_component", static_cast<uint8_t>(message.target_component));
    }

    if (_debugging) {
        LogDebug() << "Sending " << message.message_name << " via unified libmav API";
    }

    _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t mavlink_message;

        // Use clean libmav helper methods to get payload data
        auto payload_view = libmav_message.getPayloadView();
        const uint8_t payload_length = libmav_message.getPayloadLength();

        // Set up the mavlink_message_t structure
        mavlink_message.msgid = libmav_message.id();
        mavlink_message.len = payload_length;
        memcpy(mavlink_message.payload64, payload_view.first, payload_length);

        mavlink_finalize_message_chan(
            &mavlink_message,
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            payload_length,
            libmav_message.type().maxPayloadSize(),
            libmav_message.type().crcExtra());

        return mavlink_message;
    });

    if (_debugging) {
        LogDebug() << "Successfully sent " << message.message_name << " as raw data";
    }

    return MavlinkDirect::Result::Success;
}

MavlinkDirect::MessageHandle MavlinkDirectImpl::subscribe_message(
    std::string message_name, const MavlinkDirect::MessageCallback& callback)
{
    std::lock_guard<std::mutex> lock(_message_callbacks_mutex);

    // Create a proper handle
    auto handle = _message_handle_factory.create();

    // Store the callback and message name mapped to the handle
    _message_callbacks[handle] = callback;
    _message_handle_to_name[handle] = message_name;

    // Register with SystemImpl
    _system_impl->register_libmav_message_handler(
        message_name, // Empty string means all messages, specific name means filtered
        [this, handle](const LibmavMessage& libmav_msg) {
            std::lock_guard<std::mutex> callback_lock(_message_callbacks_mutex);
            auto it = _message_callbacks.find(handle);
            if (it != _message_callbacks.end()) {
                // Convert LibmavMessage to MavlinkDirect::MavlinkMessage
                MavlinkDirect::MavlinkMessage message;
                message.message_name = libmav_msg.message_name;
                message.system_id = libmav_msg.system_id;
                message.component_id = libmav_msg.component_id;
                message.target_system = libmav_msg.target_system;
                message.target_component = libmav_msg.target_component;
                message.fields_json = libmav_msg.fields_json;

                it->second(message);
            }
        },
        &handle // Use handle address as cookie for specific unregistration
    );

    return handle;
}

void MavlinkDirectImpl::unsubscribe_message(MavlinkDirect::MessageHandle handle)
{
    std::lock_guard<std::mutex> lock(_message_callbacks_mutex);

    // Find the message name for this handle
    auto name_it = _message_handle_to_name.find(handle);
    if (name_it != _message_handle_to_name.end()) {
        const std::string& message_name = name_it->second;

        // Unregister from SystemImpl using the handle address as cookie
        _system_impl->unregister_libmav_message_handler(message_name, &handle);

        // Remove from our callback maps
        _message_callbacks.erase(handle);
        _message_handle_to_name.erase(handle);
    }
}

std::optional<uint32_t> MavlinkDirectImpl::message_name_to_id(const std::string& name) const
{
    // Get MessageSet to access message definitions
    auto& message_set = _system_impl->get_message_set();

    // Use MessageSet's message name to ID conversion
    auto id_opt = message_set.idForMessage(name);
    if (id_opt.has_value()) {
        return static_cast<uint32_t>(id_opt.value());
    }

    return std::nullopt;
}

std::optional<std::string> MavlinkDirectImpl::message_id_to_name(uint32_t id) const
{
    // Get MessageSet to access message definitions
    auto& message_set = _system_impl->get_message_set();

    // Use MessageSet's message ID to name conversion
    auto message_def = message_set.getMessageDefinition(static_cast<int>(id));
    if (message_def) {
        return message_def.get().name();
    }
    return std::nullopt;
}

Json::Value MavlinkDirectImpl::libmav_to_json(const mav::Message& msg) const
{
    (void)msg;
    // TODO: Implement field iteration once libmav noexcept API is stable
    return Json::Value();
}

bool MavlinkDirectImpl::json_to_libmav(const Json::Value& json, mav::Message& msg) const
{
    (void)json;
    (void)msg;
    // TODO: Implement once libmav noexcept API set methods are stable
    return false;
}

bool MavlinkDirectImpl::json_to_libmav_message(
    const std::string& json_string, mav::Message& msg) const
{
    Json::Value json;
    Json::Reader reader;

    if (!reader.parse(json_string, json)) {
        LogErr() << "Failed to parse JSON: " << json_string;
        return false;
    }

    // Iterate through all JSON fields and set them in the libmav message
    for (const auto& field_name : json.getMemberNames()) {
        const Json::Value& field_value = json[field_name];

        // Convert JSON values to appropriate types and set in message
        if (field_value.isInt()) {
            auto result = msg.set(field_name, static_cast<int32_t>(field_value.asInt()));
            if (result != ::mav::MessageResult::Success) {
                // Try as other integer types
                if (msg.set(field_name, static_cast<uint32_t>(field_value.asUInt())) !=
                        ::mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<int16_t>(field_value.asInt())) !=
                        ::mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint16_t>(field_value.asUInt())) !=
                        ::mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<int8_t>(field_value.asInt())) !=
                        ::mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint8_t>(field_value.asUInt())) !=
                        ::mav::MessageResult::Success) {
                    LogWarn() << "Failed to set integer field " << field_name << " = "
                              << field_value.asInt();
                }
            }
        } else if (field_value.isUInt()) {
            auto result = msg.set(field_name, static_cast<uint32_t>(field_value.asUInt()));
            if (result != ::mav::MessageResult::Success) {
                // Try as other unsigned integer types
                if (msg.set(field_name, static_cast<uint64_t>(field_value.asUInt64())) !=
                        ::mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint16_t>(field_value.asUInt())) !=
                        ::mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint8_t>(field_value.asUInt())) !=
                        ::mav::MessageResult::Success) {
                    LogWarn() << "Failed to set unsigned integer field " << field_name << " = "
                              << field_value.asUInt();
                }
            }
        } else if (field_value.isDouble()) {
            auto result = msg.set(field_name, static_cast<float>(field_value.asFloat()));
            if (result != ::mav::MessageResult::Success) {
                // Try as double
                if (msg.set(field_name, field_value.asDouble()) != ::mav::MessageResult::Success) {
                    LogWarn() << "Failed to set float/double field " << field_name << " = "
                              << field_value.asDouble();
                }
            }
        } else if (field_value.isString()) {
            auto result = msg.setString(field_name, field_value.asString());
            if (result != ::mav::MessageResult::Success) {
                LogWarn() << "Failed to set string field " << field_name << " = "
                          << field_value.asString();
            }
        } else if (field_value.isArray()) {
            // Handle array fields
            auto array_size = field_value.size();

            // Try different vector types based on typical MAVLink array field types
            std::vector<uint8_t> uint8_vec;
            std::vector<uint16_t> uint16_vec;
            std::vector<uint32_t> uint32_vec;
            std::vector<int8_t> int8_vec;
            std::vector<int16_t> int16_vec;
            std::vector<int32_t> int32_vec;
            std::vector<float> float_vec;
            std::vector<double> double_vec;

            // Convert JSON array to vectors of different types
            uint8_vec.reserve(array_size);
            uint16_vec.reserve(array_size);
            uint32_vec.reserve(array_size);
            int8_vec.reserve(array_size);
            int16_vec.reserve(array_size);
            int32_vec.reserve(array_size);
            float_vec.reserve(array_size);
            double_vec.reserve(array_size);

            for (Json::ArrayIndex i = 0; i < array_size; ++i) {
                const auto& elem = field_value[i];
                if (elem.isNumeric()) {
                    uint8_vec.push_back(static_cast<uint8_t>(elem.asUInt()));
                    uint16_vec.push_back(static_cast<uint16_t>(elem.asUInt()));
                    uint32_vec.push_back(static_cast<uint32_t>(elem.asUInt()));
                    int8_vec.push_back(static_cast<int8_t>(elem.asInt()));
                    int16_vec.push_back(static_cast<int16_t>(elem.asInt()));
                    int32_vec.push_back(static_cast<int32_t>(elem.asInt()));
                    float_vec.push_back(static_cast<float>(elem.asFloat()));
                    double_vec.push_back(elem.asDouble());
                } else {
                    // Default to 0 for non-numeric values
                    uint8_vec.push_back(0);
                    uint16_vec.push_back(0);
                    uint32_vec.push_back(0);
                    int8_vec.push_back(0);
                    int16_vec.push_back(0);
                    int32_vec.push_back(0);
                    float_vec.push_back(0.0f);
                    double_vec.push_back(0.0);
                }
            }

            // Try to set the array field with different vector types
            if (msg.set(field_name, uint8_vec) == ::mav::MessageResult::Success ||
                msg.set(field_name, uint16_vec) == ::mav::MessageResult::Success ||
                msg.set(field_name, uint32_vec) == ::mav::MessageResult::Success ||
                msg.set(field_name, int8_vec) == ::mav::MessageResult::Success ||
                msg.set(field_name, int16_vec) == ::mav::MessageResult::Success ||
                msg.set(field_name, int32_vec) == ::mav::MessageResult::Success ||
                msg.set(field_name, float_vec) == ::mav::MessageResult::Success ||
                msg.set(field_name, double_vec) == ::mav::MessageResult::Success) {
                // Successfully set the array field
            } else {
                LogWarn() << "Failed to set array field " << field_name;
            }
        } else {
            LogWarn() << "Unsupported JSON field type for " << field_name;
        }
    }

    return true;
}

MavlinkDirect::Result MavlinkDirectImpl::load_custom_xml(const std::string& xml_content)
{
    // Get access to the MessageSet through the system
    auto& message_set = _system_impl->get_message_set();

    if (_debugging) {
        LogDebug() << "Loading custom XML definitions...";
    }

    // Load the custom XML into the MessageSet
    auto result = message_set.addFromXMLString(xml_content, false /* recursive_open_includes */);
    bool success = (result == ::mav::MessageSetResult::Success);

    if (success) {
        if (_debugging) {
            LogDebug() << "Successfully loaded custom XML definitions";
        }
        return MavlinkDirect::Result::Success;
    } else {
        LogErr() << "Failed to load custom XML definitions";
        return MavlinkDirect::Result::Error;
    }
}

} // namespace mavsdk
