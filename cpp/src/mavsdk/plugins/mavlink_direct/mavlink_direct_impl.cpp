#include "mavlink_direct_impl.h"
#include <mav/Message.h>
#include <mav/MessageSet.h>
#include <variant>
#include <json/json.h>
#include <cmath>
#include <limits>
#include "log.h"
#include "connection.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<MavlinkDirect::MavlinkMessage>;

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
    // Subscribe to all messages from SystemImpl and distribute to internal CallbackList
    _system_subscription = _system_impl->register_libmav_message_handler(
        "", // Empty string means all messages
        [this](const Mavsdk::MavlinkMessage& message) {
            // Convert Mavsdk::MavlinkMessage to MavlinkDirect::MavlinkMessage
            MavlinkDirect::MavlinkMessage mavlink_direct_message;
            mavlink_direct_message.message_name = message.message_name;
            mavlink_direct_message.system_id = message.system_id;
            mavlink_direct_message.component_id = message.component_id;
            mavlink_direct_message.target_system_id = message.target_system_id;
            mavlink_direct_message.target_component_id = message.target_component_id;
            mavlink_direct_message.fields_json = message.fields_json;

            // Call callbacks directly, queueing needs to happen in the user APIs
            _callbacks(mavlink_direct_message);
        });
}

void MavlinkDirectImpl::deinit()
{
    // Unsubscribe from SystemImpl - this automatically prevents dangling callbacks
    if (_system_subscription.valid()) {
        _system_impl->unregister_libmav_message_handler(_system_subscription);
        _system_subscription = {}; // Reset handle
    }
    // Internal CallbackList will be automatically cleaned up when destroyed
}

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
    if (message.target_system_id != 0) {
        // For messages that have target_system field, set it
        libmav_message.set("target_system", static_cast<uint8_t>(message.target_system_id));
    }
    if (message.target_component_id != 0) {
        // For messages that have target_component field, set it
        libmav_message.set(
            "target_component_id", static_cast<uint8_t>(message.target_component_id));
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
    // Add filtering callback to internal CallbackList
    auto filtering_callback =
        [this, message_name, callback](const MavlinkDirect::MavlinkMessage& message) {
            // Apply message filtering (empty string means all messages)
            if (!message_name.empty() && message_name != message.message_name) {
                return;
            }

            // Queue user callback to user callback thread
            _system_impl->call_user_callback([callback, message]() { callback(message); });
        };

    // Subscribe to internal CallbackList and return handle directly
    return _callbacks.subscribe(filtering_callback);
}

void MavlinkDirectImpl::unsubscribe_message(MavlinkDirect::MessageHandle handle)
{
    // Unregister from internal CallbackList directly
    _callbacks.unsubscribe(handle);
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
        // libmav handles type casting based on field definition, so we just need to pass
        // int64/uint64
        if (field_value.isInt() || field_value.isInt64()) {
            int64_t value = field_value.asInt64();
            auto result = msg.set(field_name, value);
            if (result != ::mav::MessageResult::Success) {
                LogWarn() << "Failed to set integer field " << field_name << " = " << value;
            }
        } else if (field_value.isUInt() || field_value.isUInt64()) {
            uint64_t value = field_value.asUInt64();
            auto result = msg.set(field_name, value);
            if (result != ::mav::MessageResult::Success) {
                LogWarn() << "Failed to set unsigned integer field " << field_name << " = "
                          << value;
            }
        } else if (field_value.isNull() || field_value.isDouble()) {
            // Handle float/double values (including null -> NaN)
            auto field_opt = msg.type().getField(field_name);
            if (!field_opt) {
                LogWarn() << "Field " << field_name << " not found in message definition";
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
                LogWarn() << "Field " << field_name << " is not a float or double field";
                continue;
            }

            if (result != ::mav::MessageResult::Success) {
                LogWarn() << "Failed to set float/double field " << field_name << " = "
                          << (field_value.isNull() ? "null" :
                                                     std::to_string(field_value.asDouble()));
            }
        } else if (field_value.isString()) {
            auto result = msg.setString(field_name, field_value.asString());
            if (result != ::mav::MessageResult::Success) {
                LogWarn() << "Failed to set string field " << field_name << " = "
                          << field_value.asString();
            }
        } else if (field_value.isArray()) {
            // Handle array fields with proper type detection
            auto array_size = field_value.size();

            // Get field definition to determine correct type
            auto field_opt = msg.type().getField(field_name);
            if (!field_opt) {
                LogWarn() << "Field " << field_name << " not found in message definition";
                continue;
            }

            auto field = field_opt.value();
            if (field.type.size <= 1) {
                LogWarn() << "Field " << field_name << " is not an array field";
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
                    LogWarn() << "Unsupported array field type for " << field_name;
                    break;
            }

            if (result != ::mav::MessageResult::Success) {
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
    if (_debugging) {
        LogDebug() << "Loading custom XML definitions...";
    }

    // Load the custom XML into the MessageSet with thread safety
    // This uses the thread-safe method that protects against concurrent read operations
    bool success = _system_impl->load_custom_xml_to_message_set(xml_content);

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
