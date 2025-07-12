#include "mavlink_direct_impl.h"
#include <mav/Message.h>
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
    // Get access to the LibmavReceiver's MessageSet through the system
    // For now, we'll access it through the connection's libmav receiver
    auto connections = _system_impl->get_connections();
    if (connections.empty()) {
        return MavlinkDirect::Result::ConnectionError;
    }

    // Use the first connection to get the MessageSet
    auto connection = connections[0];
    auto libmav_receiver = connection->get_libmav_receiver();
    if (!libmav_receiver) {
        return MavlinkDirect::Result::ConnectionError;
    }

    // Create libmav message from the message name
    auto libmav_message_opt = libmav_receiver->create_message(message.message_name);
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
        return MavlinkDirect::Result::InvalidField; // JSON conversion failed
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

    // Finalize the message (add header, CRC, etc.)
    mav::Identifier sender{
        static_cast<uint8_t>(message.system_id), static_cast<uint8_t>(message.component_id)};
    auto message_size_opt = libmav_message.finalize(0, sender); // seq=0 for now
    if (!message_size_opt) {
        return MavlinkDirect::Result::Error; // Message finalization failed
    }

    // Convert libmav message back to mavlink_message_t for sending
    mavlink_message_t mavlink_msg;
    if (!libmav_receiver->libmav_to_mavlink_message(libmav_message, mavlink_msg)) {
        return MavlinkDirect::Result::Error; // Conversion failed
    }

    // Send through existing connection interface
    auto send_result = connection->send_message(mavlink_msg);
    if (!send_result.first) {
        return MavlinkDirect::Result::ConnectionError;
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
    // Get connections to access LibmavReceiver
    auto connections = _system_impl->get_connections();
    if (connections.empty()) {
        return std::nullopt;
    }

    auto connection = connections[0];
    auto libmav_receiver = connection->get_libmav_receiver();
    if (!libmav_receiver) {
        return std::nullopt;
    }

    // Use LibmavReceiver's message name to ID conversion
    auto id_opt = libmav_receiver->message_name_to_id(name);
    if (id_opt.has_value()) {
        return static_cast<uint32_t>(id_opt.value());
    }

    return std::nullopt;
}

std::optional<std::string> MavlinkDirectImpl::message_id_to_name(uint32_t id) const
{
    // Get connections to access LibmavReceiver
    auto connections = _system_impl->get_connections();
    if (connections.empty()) {
        return std::nullopt;
    }

    auto connection = connections[0];
    auto libmav_receiver = connection->get_libmav_receiver();
    if (!libmav_receiver) {
        return std::nullopt;
    }

    // Use LibmavReceiver's message ID to name conversion
    return libmav_receiver->message_id_to_name(id);
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
            if (result != mav::MessageResult::Success) {
                // Try as other integer types
                if (msg.set(field_name, static_cast<uint32_t>(field_value.asUInt())) !=
                        mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<int16_t>(field_value.asInt())) !=
                        mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint16_t>(field_value.asUInt())) !=
                        mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<int8_t>(field_value.asInt())) !=
                        mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint8_t>(field_value.asUInt())) !=
                        mav::MessageResult::Success) {
                    LogWarn() << "Failed to set integer field " << field_name << " = "
                              << field_value.asInt();
                }
            }
        } else if (field_value.isUInt()) {
            auto result = msg.set(field_name, static_cast<uint32_t>(field_value.asUInt()));
            if (result != mav::MessageResult::Success) {
                // Try as other unsigned integer types
                if (msg.set(field_name, static_cast<uint64_t>(field_value.asUInt64())) !=
                        mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint16_t>(field_value.asUInt())) !=
                        mav::MessageResult::Success &&
                    msg.set(field_name, static_cast<uint8_t>(field_value.asUInt())) !=
                        mav::MessageResult::Success) {
                    LogWarn() << "Failed to set unsigned integer field " << field_name << " = "
                              << field_value.asUInt();
                }
            }
        } else if (field_value.isDouble()) {
            auto result = msg.set(field_name, static_cast<float>(field_value.asFloat()));
            if (result != mav::MessageResult::Success) {
                // Try as double
                if (msg.set(field_name, field_value.asDouble()) != mav::MessageResult::Success) {
                    LogWarn() << "Failed to set float/double field " << field_name << " = "
                              << field_value.asDouble();
                }
            }
        } else if (field_value.isString()) {
            auto result = msg.setString(field_name, field_value.asString());
            if (result != mav::MessageResult::Success) {
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
            if (msg.set(field_name, uint8_vec) == mav::MessageResult::Success ||
                msg.set(field_name, uint16_vec) == mav::MessageResult::Success ||
                msg.set(field_name, uint32_vec) == mav::MessageResult::Success ||
                msg.set(field_name, int8_vec) == mav::MessageResult::Success ||
                msg.set(field_name, int16_vec) == mav::MessageResult::Success ||
                msg.set(field_name, int32_vec) == mav::MessageResult::Success ||
                msg.set(field_name, float_vec) == mav::MessageResult::Success ||
                msg.set(field_name, double_vec) == mav::MessageResult::Success) {
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

} // namespace mavsdk