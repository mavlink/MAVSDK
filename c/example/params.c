//
// Simple example to demonstrate how to get and set params.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cmavsdk/mavsdk.h"
#include "cmavsdk/plugins/param/param.h"

void print_usage(const char* bin_name)
{
    fprintf(stderr, "Usage :%s <connection url> <action> [args]\n", bin_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Connection URL format should be :\n");
    fprintf(stderr, " For TCP server: tcpin://<our_ip>:<port>\n");
    fprintf(stderr, " For TCP client: tcpout://<remote_ip>:<port>\n");
    fprintf(stderr, " For UDP server: udpin://<our_ip>:<port>\n");
    fprintf(stderr, " For UDP client: udpout://<remote_ip>:<port>\n");
    fprintf(stderr, " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n");
    fprintf(stderr, "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Actions:\n");
    fprintf(stderr, "  get_all:          Print all parameters\n");
    fprintf(stderr, "  get <param name>: Get one param\n");
    fprintf(stderr, "  set <param name> <value>: Set one param\n");
}

typedef enum {
    ACTION_GET_ALL,
    ACTION_GET,
    ACTION_SET,
} Action;

typedef struct {
    char* connection;
    Action action;
    char* param_name;
    char* value;
} Args;

bool parse_args(int argc, char** argv, Args* args)
{
    if (argc < 3) {
        fprintf(stderr, "Insufficient arguments\n");
        return false;
    }

    args->connection = argv[1];
    args->param_name = NULL;
    args->value = NULL;

    const char* action_str = argv[2];

    if (strcmp(action_str, "get_all") == 0) {
        if (argc != 3) {
            fprintf(stderr, "get_all takes no additional arguments\n");
            return false;
        }
        args->action = ACTION_GET_ALL;
    } else if (strcmp(action_str, "get") == 0) {
        if (argc != 4) {
            fprintf(stderr, "get requires a parameter name\n");
            return false;
        }
        args->action = ACTION_GET;
        args->param_name = argv[3];
    } else if (strcmp(action_str, "set") == 0) {
        if (argc != 5) {
            fprintf(stderr, "set requires a parameter name and value\n");
            return false;
        }
        args->action = ACTION_SET;
        args->param_name = argv[3];
        args->value = argv[4];
    } else {
        fprintf(stderr, "Unknown action: %s\n", action_str);
        return false;
    }

    return true;
}

void get_all(mavsdk_param_t param)
{
    mavsdk_param_all_params_t all_params;
    mavsdk_param_get_all_params(param, &all_params);

    printf("Int params: \n");
    for (size_t i = 0; i < all_params.int_params_size; i++) {
        printf("%s: %d\n", all_params.int_params[i].name, all_params.int_params[i].value);
    }

    printf("Float params: \n");
    for (size_t i = 0; i < all_params.float_params_size; i++) {
        printf("%s: %f\n", all_params.float_params[i].name, all_params.float_params[i].value);
    }

    // Clean up
    mavsdk_param_all_params_destroy(&all_params);
}

void get(mavsdk_param_t param, const char* name)
{
    // Try as float param first
    fprintf(stderr, "Try as float param...");
    float float_value;
    mavsdk_param_result_t result = mavsdk_param_get_param_float(param, (char*)name, &float_value);

    if (result == MAVSDK_PARAM_RESULT_SUCCESS) {
        fprintf(stderr, "Ok\n");
        printf("%s: %f\n", name, float_value);
        return;
    }

    if (result != MAVSDK_PARAM_RESULT_WRONG_TYPE) {
        fprintf(stderr, "Failed: %d\n", result);
        return;
    }

    printf("Wrong type\n");
    fprintf(stderr, "Try as int param next...");
    
    int32_t int_value;
    result = mavsdk_param_get_param_int(param, (char*)name, &int_value);

    if (result == MAVSDK_PARAM_RESULT_SUCCESS) {
        fprintf(stderr, "Ok\n");
        printf("%s: %d\n", name, int_value);
        return;
    }

    if (result != MAVSDK_PARAM_RESULT_WRONG_TYPE) {
        fprintf(stderr, "Failed: %d\n", result);
        return;
    }

    fprintf(stderr, "Failed: %d\n", result);
    fprintf(stderr, "Neither int or float worked, should maybe try custom? (not implemented)\n");
}

void set_param(mavsdk_param_t param, const char* name, const char* value)
{
    // Try to parse as integer first
    char* endptr;
    long int_value = strtol(value, &endptr, 10);
    
    if (*endptr == '\0') {
        // Successfully parsed as integer
        fprintf(stderr, "Setting %s as integer...", value);
        
        mavsdk_param_result_t result = mavsdk_param_set_param_int(param, (char*)name, (int32_t)int_value);
        
        if (result == MAVSDK_PARAM_RESULT_SUCCESS) {
            fprintf(stderr, "Ok\n");
            return;
        }
        
        if (result != MAVSDK_PARAM_RESULT_WRONG_TYPE) {
            fprintf(stderr, "Failed: %d\n", result);
            return;
        }
        
        // If we got the type wrong, we try as float next
    }
    
    // Try to parse as float
    float float_value = strtof(value, &endptr);
    
    if (*endptr == '\0') {
        // Successfully parsed as float
        fprintf(stderr, "Setting %s as float...", value);
        
        mavsdk_param_result_t result = mavsdk_param_set_param_float(param, (char*)name, float_value);
        
        if (result == MAVSDK_PARAM_RESULT_SUCCESS) {
            fprintf(stderr, "Ok\n");
            return;
        }
        
        if (result != MAVSDK_PARAM_RESULT_WRONG_TYPE) {
            fprintf(stderr, "Failed: %d\n", result);
            return;
        }
        
        fprintf(stderr, "Failed: %d\n", result);
    }
    
    fprintf(stderr, "Neither int or float worked, should maybe try custom? (not implemented)\n");
}

// Global flag for new system callback
static volatile bool system_found = false;
static mavsdk_system_t discovered_system = NULL;

static void on_new_system(void* user_data)
{
    mavsdk_t mavsdk = (mavsdk_t)user_data;
    
    size_t system_count = 0;
    mavsdk_system_t* systems = mavsdk_get_systems(mavsdk, &system_count);
    
    if (system_count > 0) {
        // Get the last system (newly discovered)
        discovered_system = systems[system_count - 1];
        system_found = true;
    }
    
    if (systems) {
        mavsdk_free_systems_array(systems);
    }
}

int main(int argc, char** argv)
{
    Args args;
    if (!parse_args(argc, argv, &args)) {
        print_usage(argv[0]);
        return 1;
    }

    // Create MAVSDK instance
    mavsdk_configuration_t config = 
        mavsdk_configuration_create_with_component_type(MAVSDK_COMPONENT_TYPE_GROUND_STATION);
    mavsdk_t mavsdk = mavsdk_create(config);

    // Add connection
    mavsdk_connection_result_t connection_result = 
        mavsdk_add_any_connection(mavsdk, args.connection);

    if (connection_result != MAVSDK_CONNECTION_RESULT_SUCCESS) {
        fprintf(stderr, "Connection failed: %d\n", connection_result);
        mavsdk_destroy(mavsdk);
        return 1;
    }

    printf("Waiting to discover system...\n");

    // Subscribe to new systems
    mavsdk_new_system_handle_t handle = 
        mavsdk_subscribe_on_new_system(mavsdk, on_new_system, mavsdk);

    // Wait for system discovery (up to 3 seconds)
    int wait_count = 0;
    while (!system_found && wait_count < 30) {
        sleep(1);
        wait_count++;
    }

    // Unsubscribe
    mavsdk_unsubscribe_on_new_system(mavsdk, handle);

    if (!system_found) {
        fprintf(stderr, "No autopilot found, exiting.\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }

    printf("Discovered autopilot\n");

    // Create param plugin
    mavsdk_param_t param = mavsdk_param_create(discovered_system);
    if (!param) {
        fprintf(stderr, "Failed to create param plugin\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }

    // Execute action
    switch (args.action) {
        case ACTION_GET_ALL:
            get_all(param);
            break;

        case ACTION_GET:
            get(param, args.param_name);
            break;

        case ACTION_SET:
            set_param(param, args.param_name, args.value);
            break;
    }

    // Cleanup
    mavsdk_param_destroy(param);
    mavsdk_destroy(mavsdk);

    return 0;
}
