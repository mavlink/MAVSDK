#include "mavsdk.h"

#include <mavsdk/mavsdk.h>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

namespace {
    struct CallbackContext {
        void* callback;
        void* user_data;
    };
}

using namespace mavsdk;

// ===== Helper conversions =====

// C → C++ ComponentType
static ComponentType cpp_component_type_from_c(mavsdk_component_type_t type) {
    switch (type) {
        case MAVSDK_COMPONENT_TYPE_AUTOPILOT: return ComponentType::Autopilot;
        case MAVSDK_COMPONENT_TYPE_GROUND_STATION: return ComponentType::GroundStation;
        case MAVSDK_COMPONENT_TYPE_COMPANION_COMPUTER: return ComponentType::CompanionComputer;
        case MAVSDK_COMPONENT_TYPE_CAMERA: return ComponentType::Camera;
        case MAVSDK_COMPONENT_TYPE_GIMBAL: return ComponentType::Gimbal;
        case MAVSDK_COMPONENT_TYPE_REMOTE_ID: return ComponentType::RemoteId;
        default: return ComponentType::GroundStation;
    }
}

// C++ → C ComponentType
static mavsdk_component_type_t c_component_type_from_cpp(ComponentType type) {
    switch (type) {
        case ComponentType::Autopilot: return MAVSDK_COMPONENT_TYPE_AUTOPILOT;
        case ComponentType::GroundStation: return MAVSDK_COMPONENT_TYPE_GROUND_STATION;
        case ComponentType::CompanionComputer: return MAVSDK_COMPONENT_TYPE_COMPANION_COMPUTER;
        case ComponentType::Camera: return MAVSDK_COMPONENT_TYPE_CAMERA;
        case ComponentType::Gimbal: return MAVSDK_COMPONENT_TYPE_GIMBAL;
        case ComponentType::RemoteId: return MAVSDK_COMPONENT_TYPE_REMOTE_ID;
        default: return MAVSDK_COMPONENT_TYPE_GROUND_STATION;
    }
}

// C++ → C ConnectionResult
static mavsdk_connection_result_t c_connection_result_from_cpp(ConnectionResult result) {
    switch (result) {
        case ConnectionResult::Success: return MAVSDK_CONNECTION_RESULT_SUCCESS;
        case ConnectionResult::Timeout: return MAVSDK_CONNECTION_RESULT_TIMEOUT;
        case ConnectionResult::SocketError: return MAVSDK_CONNECTION_RESULT_SOCKET_ERROR;
        case ConnectionResult::BindError: return MAVSDK_CONNECTION_RESULT_BIND_ERROR;
        case ConnectionResult::SocketConnectionError: return MAVSDK_CONNECTION_RESULT_SOCKET_CONNECTION_ERROR;
        case ConnectionResult::ConnectionError: return MAVSDK_CONNECTION_RESULT_CONNECTION_ERROR;
        case ConnectionResult::NotImplemented: return MAVSDK_CONNECTION_RESULT_NOT_IMPLEMENTED;
        case ConnectionResult::SystemNotConnected: return MAVSDK_CONNECTION_RESULT_SYSTEM_NOT_CONNECTED;
        case ConnectionResult::SystemBusy: return MAVSDK_CONNECTION_RESULT_SYSTEM_BUSY;
        case ConnectionResult::CommandDenied: return MAVSDK_CONNECTION_RESULT_COMMAND_DENIED;
        case ConnectionResult::DestinationIpUnknown: return MAVSDK_CONNECTION_RESULT_DESTINATION_IP_UNKNOWN;
        case ConnectionResult::ConnectionsExhausted: return MAVSDK_CONNECTION_RESULT_CONNECTIONS_EXHAUSTED;
        case ConnectionResult::ConnectionUrlInvalid: return MAVSDK_CONNECTION_RESULT_CONNECTION_URL_INVALID;
        case ConnectionResult::BaudrateUnknown: return MAVSDK_CONNECTION_RESULT_BAUDRATE_UNKNOWN;
        default: return MAVSDK_CONNECTION_RESULT_CONNECTION_ERROR;
    }
}

// C → C++ ForwardingOption
static ForwardingOption cpp_forwarding_option_from_c(mavsdk_forwarding_option_t option) {
    switch (option) {
        case MAVSDK_FORWARDING_OPTION_OFF: return ForwardingOption::ForwardingOff;
        case MAVSDK_FORWARDING_OPTION_ON: return ForwardingOption::ForwardingOn;
        default: return ForwardingOption::ForwardingOff;
    }
}

// ===== C API Implementation =====
extern "C" {

// --- Configuration ---
mavsdk_configuration_t mavsdk_configuration_create_with_component_type(mavsdk_component_type_t type) {
    auto* config = new Mavsdk::Configuration(cpp_component_type_from_c(type));
    return static_cast<mavsdk_configuration_t>(config);
}

mavsdk_configuration_t mavsdk_configuration_create_manual(uint8_t system_id, uint8_t component_id, int always_send_heartbeats) {
    auto* config = new Mavsdk::Configuration(system_id, component_id, always_send_heartbeats != 0);
    return static_cast<mavsdk_configuration_t>(config);
}

void mavsdk_configuration_destroy(mavsdk_configuration_t config) {
    delete static_cast<Mavsdk::Configuration*>(config);
}

uint8_t mavsdk_configuration_get_system_id(mavsdk_configuration_t config) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    return cpp_config->get_system_id();
}

void mavsdk_configuration_set_system_id(mavsdk_configuration_t config, uint8_t system_id) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    cpp_config->set_system_id(system_id);
}

uint8_t mavsdk_configuration_get_component_id(mavsdk_configuration_t config) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    return cpp_config->get_component_id();
}

void mavsdk_configuration_set_component_id(mavsdk_configuration_t config, uint8_t component_id) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    cpp_config->set_component_id(component_id);
}

int mavsdk_configuration_get_always_send_heartbeats(mavsdk_configuration_t config) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    return cpp_config->get_always_send_heartbeats() ? 1 : 0;
}

void mavsdk_configuration_set_always_send_heartbeats(mavsdk_configuration_t config, int always_send_heartbeats) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    cpp_config->set_always_send_heartbeats(always_send_heartbeats != 0);
}

mavsdk_component_type_t mavsdk_configuration_get_component_type(mavsdk_configuration_t config) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    return c_component_type_from_cpp(cpp_config->get_component_type());
}

void mavsdk_configuration_set_component_type(mavsdk_configuration_t config, mavsdk_component_type_t component_type) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    cpp_config->set_component_type(cpp_component_type_from_c(component_type));
}

uint8_t mavsdk_configuration_get_mav_type(mavsdk_configuration_t config) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    return cpp_config->get_mav_type();
}

void mavsdk_configuration_set_mav_type(mavsdk_configuration_t config, uint8_t mav_type) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    cpp_config->set_mav_type(mav_type);
}

// --- Core ---
mavsdk_t mavsdk_create(mavsdk_configuration_t config) {
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(config);
    auto* mavsdk = new Mavsdk(*cpp_config);
    return static_cast<mavsdk_t>(mavsdk);
}

void mavsdk_destroy(mavsdk_t mavsdk) {
    delete static_cast<Mavsdk*>(mavsdk);
}

const char* mavsdk_version(mavsdk_t mavsdk) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    static std::string version_str = cpp_mavsdk->version();
    return version_str.c_str();
}

// --- Connection Management ---
mavsdk_connection_result_t mavsdk_add_any_connection(mavsdk_t mavsdk, const char* connection_url) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto result = cpp_mavsdk->add_any_connection(std::string(connection_url));
    return c_connection_result_from_cpp(result);
}

mavsdk_connection_result_t mavsdk_add_any_connection_with_forwarding(
    mavsdk_t mavsdk,
    const char* connection_url,
    mavsdk_forwarding_option_t forwarding_option
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto result = cpp_mavsdk->add_any_connection(
        std::string(connection_url),
        cpp_forwarding_option_from_c(forwarding_option)
    );
    return c_connection_result_from_cpp(result);
}

mavsdk_connection_result_with_handle_t mavsdk_add_any_connection_with_handle(
    mavsdk_t mavsdk,
    const char* connection_url
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto result_pair = cpp_mavsdk->add_any_connection_with_handle(std::string(connection_url));

    mavsdk_connection_result_with_handle_t c_result;
    c_result.result = c_connection_result_from_cpp(result_pair.first);
    c_result.handle = new Mavsdk::ConnectionHandle(std::move(result_pair.second));
    return c_result;
}

mavsdk_connection_result_with_handle_t mavsdk_add_any_connection_with_handle_and_forwarding(
    mavsdk_t mavsdk,
    const char* connection_url, mavsdk_forwarding_option_t forwarding_option
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto result_pair = cpp_mavsdk->add_any_connection_with_handle(
        std::string(connection_url),
        cpp_forwarding_option_from_c(forwarding_option)
    );

    mavsdk_connection_result_with_handle_t c_result;
    c_result.result = c_connection_result_from_cpp(result_pair.first);
    c_result.handle = new Mavsdk::ConnectionHandle(std::move(result_pair.second));
    return c_result;
}

void mavsdk_remove_connection(mavsdk_t mavsdk, mavsdk_connection_handle_t handle) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto* cpp_handle = static_cast<Mavsdk::ConnectionHandle*>(handle);
    cpp_mavsdk->remove_connection(*cpp_handle);
    delete cpp_handle;
}

// --- System Discovery and Management ---
size_t mavsdk_system_count(mavsdk_t mavsdk) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto systems = cpp_mavsdk->systems();
    return systems.size();
}

mavsdk_system_t* mavsdk_get_systems(mavsdk_t mavsdk, size_t* count) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto systems = cpp_mavsdk->systems();
    *count = systems.size();

    if (systems.empty()) {
        return nullptr;
    }

    auto* result = new mavsdk_system_t[systems.size()];
    for (size_t i = 0; i < systems.size(); ++i) {
        result[i] = static_cast<mavsdk_system_t>(new std::shared_ptr<System>(systems[i]));
    }
    return result;
}

void mavsdk_free_systems_array(mavsdk_system_t* systems) {
    delete[] systems;
}

mavsdk_system_t mavsdk_first_autopilot(mavsdk_t mavsdk, double timeout_s) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto system_opt = cpp_mavsdk->first_autopilot(timeout_s);
    if (system_opt.has_value()) {
        return static_cast<mavsdk_system_t>(new std::shared_ptr<System>(system_opt.value()));
    }
    return nullptr;
}

// --- Connection Error Handling ---
mavsdk_connection_error_handle_t mavsdk_subscribe_connection_errors(
    mavsdk_t mavsdk,
    mavsdk_connection_error_callback_t callback,
    void* user_data
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);

    struct CallbackContext {
        mavsdk_connection_error_callback_t callback;
        void* user_data;
    };

    auto* ctx = new CallbackContext{callback, user_data};

    auto handle = cpp_mavsdk->subscribe_connection_errors(
        [ctx](Mavsdk::ConnectionError error) {
            mavsdk_connection_error_t c_error;
            c_error.error_description = new char[error.error_description.length() + 1];
            std::strcpy(c_error.error_description, error.error_description.c_str());
            c_error.connection_handle = new Mavsdk::ConnectionHandle(error.connection_handle);

            ctx->callback(c_error, ctx->user_data);

            delete[] c_error.error_description;
            delete static_cast<Mavsdk::ConnectionHandle*>(c_error.connection_handle);
        }
    );

    auto* result = new std::pair<Mavsdk::ConnectionErrorHandle, CallbackContext*>(
        std::move(handle), ctx
    );
    return static_cast<mavsdk_connection_error_handle_t>(result);
}

void mavsdk_unsubscribe_connection_errors(
    mavsdk_t mavsdk,
    mavsdk_connection_error_handle_t handle
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto* pair = static_cast<std::pair<Mavsdk::ConnectionErrorHandle, CallbackContext*>*>(handle);

    cpp_mavsdk->unsubscribe_connection_errors(pair->first);
    delete pair->second;
    delete pair;
}

// --- System Event Subscriptions ---
mavsdk_new_system_handle_t mavsdk_subscribe_on_new_system(
    mavsdk_t mavsdk,
    mavsdk_new_system_callback_t callback,
    void* user_data
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);

    struct CallbackContext {
        mavsdk_new_system_callback_t callback;
        void* user_data;
    };

    auto* ctx = new CallbackContext{callback, user_data};

    auto handle = cpp_mavsdk->subscribe_on_new_system(
        [ctx]() {
            ctx->callback(ctx->user_data);
        }
    );

    auto* result = new std::pair<Mavsdk::NewSystemHandle, CallbackContext*>(
        std::move(handle), ctx
    );
    return static_cast<mavsdk_new_system_handle_t>(result);
}

void mavsdk_unsubscribe_on_new_system(
    mavsdk_t mavsdk,
    mavsdk_new_system_handle_t handle
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto* pair = static_cast<std::pair<Mavsdk::NewSystemHandle, CallbackContext*>*>(handle);

    cpp_mavsdk->unsubscribe_on_new_system(pair->first);
    delete pair->second;
    delete pair;
}

// --- Configuration ---
void mavsdk_set_configuration(
    mavsdk_t mavsdk,
    mavsdk_configuration_t configuration
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto* cpp_config = static_cast<Mavsdk::Configuration*>(configuration);
    cpp_mavsdk->set_configuration(*cpp_config);
}

// --- Server Components ---
mavsdk_server_component_t mavsdk_server_component(
    mavsdk_t mavsdk,
    unsigned int instance
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto component = cpp_mavsdk->server_component(instance);
    if (component) {
        return static_cast<mavsdk_server_component_t>(
            new std::shared_ptr<ServerComponent>(component)
        );
    }
    return nullptr;
}

mavsdk_server_component_t mavsdk_server_component_by_type(
    mavsdk_t mavsdk,
    mavsdk_component_type_t component_type,
    unsigned int instance
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto component = cpp_mavsdk->server_component_by_type(
        cpp_component_type_from_c(component_type),
        instance
    );
    if (component) {
        return static_cast<mavsdk_server_component_t>(
            new std::shared_ptr<ServerComponent>(component)
        );
    }
    return nullptr;
}

mavsdk_server_component_t mavsdk_server_component_by_id(
    mavsdk_t mavsdk,
    uint8_t component_id
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto component = cpp_mavsdk->server_component_by_id(component_id);
    if (component) {
        return static_cast<mavsdk_server_component_t>(
            new std::shared_ptr<ServerComponent>(component)
        );
    }
    return nullptr;
}

// --- Message Interception (JSON) ---
mavsdk_intercept_json_handle_t mavsdk_subscribe_incoming_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_callback_t callback,
    void* user_data
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);

    struct CallbackContext {
        mavsdk_intercept_json_callback_t callback;
        void* user_data;
    };

    auto* ctx = new CallbackContext{callback, user_data};

    auto handle = cpp_mavsdk->subscribe_incoming_messages_json(
        [ctx](Mavsdk::MavlinkMessage msg) -> bool {
            mavsdk_message_t c_msg;
            c_msg.message_name = new char[msg.message_name.length() + 1];
            std::strcpy(c_msg.message_name, msg.message_name.c_str());
            c_msg.system_id = msg.system_id;
            c_msg.component_id = msg.component_id;
            c_msg.target_system_id = msg.target_system_id;
            c_msg.target_component_id = msg.target_component_id;
            c_msg.fields_json = new char[msg.fields_json.length() + 1];
            std::strcpy(c_msg.fields_json, msg.fields_json.c_str());

            int result = ctx->callback(c_msg, ctx->user_data);

            delete[] c_msg.message_name;
            delete[] c_msg.fields_json;

            return result != 0;
        }
    );

    auto* result = new std::pair<Mavsdk::InterceptJsonHandle, CallbackContext*>(
        std::move(handle), ctx
    );
    return static_cast<mavsdk_intercept_json_handle_t>(result);
}

void mavsdk_unsubscribe_incoming_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_handle_t handle
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto* pair = static_cast<std::pair<Mavsdk::InterceptJsonHandle, CallbackContext*>*>(handle);

    cpp_mavsdk->unsubscribe_incoming_messages_json(pair->first);
    delete pair->second;
    delete pair;
}

mavsdk_intercept_json_handle_t mavsdk_subscribe_outgoing_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_callback_t callback,
    void* user_data
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);

    struct CallbackContext {
        mavsdk_intercept_json_callback_t callback;
        void* user_data;
    };

    auto* ctx = new CallbackContext{callback, user_data};

    auto handle = cpp_mavsdk->subscribe_outgoing_messages_json(
        [ctx](Mavsdk::MavlinkMessage msg) -> bool {
            mavsdk_message_t c_msg;
            c_msg.message_name = new char[msg.message_name.length() + 1];
            std::strcpy(c_msg.message_name, msg.message_name.c_str());
            c_msg.system_id = msg.system_id;
            c_msg.component_id = msg.component_id;
            c_msg.target_system_id = msg.target_system_id;
            c_msg.target_component_id = msg.target_component_id;
            c_msg.fields_json = new char[msg.fields_json.length() + 1];
            std::strcpy(c_msg.fields_json, msg.fields_json.c_str());

            int result = ctx->callback(c_msg, ctx->user_data);

            delete[] c_msg.message_name;
            delete[] c_msg.fields_json;

            return result != 0;
        }
    );

    auto* result = new std::pair<Mavsdk::InterceptJsonHandle, CallbackContext*>(
        std::move(handle), ctx
    );
    return static_cast<mavsdk_intercept_json_handle_t>(result);
}

void mavsdk_unsubscribe_outgoing_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_handle_t handle
) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    auto* pair = static_cast<std::pair<Mavsdk::InterceptJsonHandle, CallbackContext*>*>(handle);

    cpp_mavsdk->unsubscribe_outgoing_messages_json(pair->first);
    delete pair->second;
    delete pair;
}

// --- Timeout ---
void mavsdk_set_timeout_s(mavsdk_t mavsdk, double timeout_s) {
    auto* cpp_mavsdk = static_cast<Mavsdk*>(mavsdk);
    cpp_mavsdk->set_timeout_s(timeout_s);
}

// --- Memory Management Helpers ---
void mavsdk_free_connection_error(mavsdk_connection_error_t* error) {
    if (error) {
        delete[] error->error_description;
        error->error_description = nullptr;
    }
}

void mavsdk_free_mavsdk_message(mavsdk_message_t* message) {
    if (message) {
        delete[] message->message_name;
        delete[] message->fields_json;
        message->message_name = nullptr;
        message->fields_json = nullptr;
    }
}

} // extern "C"
