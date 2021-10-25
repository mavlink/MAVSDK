#include "custom_action_impl.h"
#include "mavsdk_impl.h"

#include <fstream>

#ifdef __APPLE__
#include <spawn.h>
#include <sys/wait.h>

extern char** environ;
#endif

namespace mavsdk {

CustomActionImpl::CustomActionImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

CustomActionImpl::CustomActionImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

CustomActionImpl::~CustomActionImpl()
{
    _parent->unregister_plugin(this);
}

void CustomActionImpl::init()
{
    using namespace std::placeholders;

    _parent->register_mavlink_command_handler(
        MAV_CMD_WAYPOINT_USER_1, // TODO: use MAV_CMD_CUSTOM_ACTION when it is merged in upstream
                                 // MAVLink
        std::bind(&CustomActionImpl::process_custom_action_command, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_CANCEL,
        std::bind(&CustomActionImpl::process_command_cancellation, this, _1),
        this);
}

void CustomActionImpl::deinit()
{
    _parent->unregister_all_mavlink_command_handlers(this);
    _parent->unregister_all_mavlink_message_handlers(this);
}

void CustomActionImpl::enable() {}

void CustomActionImpl::disable() {}

mavlink_message_t
CustomActionImpl::process_custom_action_command(const MavlinkCommandReceiver::CommandLong& command)
{
    CustomAction::ActionToExecute action_to_exec;
    action_to_exec.id = static_cast<uint32_t>(command.params.param1);
    action_to_exec.timeout = command.params.param3;

    store_custom_action(action_to_exec);

    mavlink_message_t command_ack;

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_custom_action_command_subscription) {
        auto callback = _custom_action_command_subscription;
        auto arg1 = custom_action();

        _parent->call_user_callback([callback, arg1]() { callback(arg1); });

        // Send first ACK marking the command as being in progress
        mavlink_msg_command_ack_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &command_ack,
            MAV_CMD_WAYPOINT_USER_1, // TODO: use MAV_CMD_CUSTOM_ACTION when it is merged in
                                     // upstream MAVLink
            MAV_RESULT_IN_PROGRESS,
            0,
            action_to_exec.id, // Use the action ID in param4 to identify the action/process
            _parent->get_own_system_id(),
            _parent->get_autopilot_id());
    } else {
        // Send first ACK marking the command as being in progress
        mavlink_msg_command_ack_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &command_ack,
            MAV_CMD_WAYPOINT_USER_1, // TODO: use MAV_CMD_CUSTOM_ACTION when it is merged in
            // upstream MAVLink
            MAV_RESULT_TEMPORARILY_REJECTED,
            0,
            action_to_exec.id, // Use the action ID in param4 to identify the action/process
            _parent->get_own_system_id(),
            _parent->get_autopilot_id());
    }

    // The COMMAND_ACK is sent as a result fo the callback so to be processed and
    // sent on the server side.
    return command_ack;
}

void CustomActionImpl::process_command_cancellation(const mavlink_message_t& message)
{
    mavlink_command_cancel_t command_cancel;
    mavlink_msg_command_cancel_decode(&message, &command_cancel);

    if (command_cancel.command == MAV_CMD_WAYPOINT_USER_1) { // TODO: use MAV_CMD_CUSTOM_ACTION when
                                                             // it is merged in upstream MAVLink
        store_custom_action_cancellation(true);
    }

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_custom_action_command_cancel_subscription) {
        auto callback = _custom_action_command_cancel_subscription;
        auto arg1 = custom_action_cancellation();

        _parent->call_user_callback([callback, arg1]() { callback(arg1); });

        // Send ACK for cancellation
        mavlink_message_t command_ack;
        mavlink_msg_command_ack_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &command_ack,
            MAV_CMD_WAYPOINT_USER_1, // TODO: use MAV_CMD_CUSTOM_ACTION when it is merged in
                                     // upstream MAVLink
            MAV_RESULT_CANCELLED,
            0,
            0,
            _parent->get_own_system_id(),
            _parent->get_autopilot_id());

        _parent->send_message(command_ack);
    }
}

void CustomActionImpl::store_custom_action(CustomAction::ActionToExecute action_to_execute)
{
    std::lock_guard<std::mutex> lock(_custom_action_mutex);
    _custom_action = action_to_execute;
}

void CustomActionImpl::store_custom_action_cancellation(bool action_cancel)
{
    std::lock_guard<std::mutex> lock(_custom_action_cancellation_mutex);
    _custom_action_cancellation = action_cancel;
}

CustomAction::Result CustomActionImpl::respond_custom_action(
    CustomAction::ActionToExecute action_to_execute, CustomAction::Result result) const
{
    auto prom = std::promise<CustomAction::Result>();
    auto fut = prom.get_future();

    respond_custom_action_async(
        action_to_execute, result, [&prom](CustomAction::Result action_result) {
            prom.set_value(action_result);
        });

    return fut.get();
}

void CustomActionImpl::respond_custom_action_async(
    CustomAction::ActionToExecute action_to_execute,
    CustomAction::Result result,
    const CustomAction::ResultCallback& callback) const
{
    // Send ACKs based on the action status
    mavlink_message_t command_ack;
    mavlink_msg_command_ack_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &command_ack,
        MAV_CMD_WAYPOINT_USER_1, // TODO: use MAV_CMD_CUSTOM_ACTION when it is merged in upstream
                                 // MAVLink
        mavlink_command_result_from_custom_action_result(result),
        static_cast<uint8_t>(
            action_to_execute.progress), // Set the command progress when applicable
        action_to_execute.id, // Use the action ID in param4 to identify the action/process
        _parent->get_own_system_id(),
        _parent->get_autopilot_id());

    auto msg_result = _parent->send_message(command_ack);

    const CustomAction::Result action_result =
        msg_result ? CustomAction::Result::Success : CustomAction::Result::Error;

    if (callback) {
        auto temp_callback = callback;
        _parent->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

CustomAction::ActionToExecute CustomActionImpl::custom_action() const
{
    std::lock_guard<std::mutex> lock(_custom_action_mutex);
    return _custom_action;
}

void CustomActionImpl::subscribe_custom_action(CustomAction::CustomActionCallback callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _custom_action_command_subscription = callback;
}

bool CustomActionImpl::custom_action_cancellation() const
{
    std::lock_guard<std::mutex> lock(_custom_action_cancellation_mutex);
    return _custom_action_cancellation;
}

void CustomActionImpl::subscribe_custom_action_cancellation(
    CustomAction::CustomActionCancellationCallback callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _custom_action_command_cancel_subscription = callback;
}

CustomAction::Result
CustomActionImpl::set_custom_action(CustomAction::ActionToExecute& action) const
{
    auto prom = std::promise<CustomAction::Result>();
    auto fut = prom.get_future();

    set_custom_action_async(
        action, [&prom](CustomAction::Result result) { prom.set_value(result); });

    return fut.get();
}

void CustomActionImpl::set_custom_action_async(
    CustomAction::ActionToExecute& action, const CustomAction::ResultCallback& callback) const
{
    MavlinkCommandSender::CommandLong command{*_parent};

    command.command = MAV_CMD_WAYPOINT_USER_1; // TODO: use MAV_CMD_CUSTOM_ACTION when it is merged
                                               // in upstream MAVLink
    command.params.param1 = static_cast<float>(action.id); // Action ID
    command.params.param2 = 0; // Action execution control (N/A)
    command.params.param3 = action.timeout; // Action timeout
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

std::pair<CustomAction::Result, CustomAction::ActionMetadata>
CustomActionImpl::custom_action_metadata(
    CustomAction::ActionToExecute& action, std::string& file) const
{
    auto prom = std::promise<std::pair<CustomAction::Result, CustomAction::ActionMetadata>>();
    auto fut = prom.get_future();

    custom_action_metadata_async(
        action, file, [&prom](CustomAction::Result result, CustomAction::ActionMetadata metadata) {
            prom.set_value(
                std::pair<CustomAction::Result, CustomAction::ActionMetadata>(result, metadata));
        });

    return fut.get();
}

void CustomActionImpl::custom_action_metadata_async(
    CustomAction::ActionToExecute& action,
    const std::string& file,
    const CustomAction::CustomActionMetadataCallback& callback) const
{
    CustomAction::ActionMetadata action_metadata{};
    CustomAction::Result parsing_result = CustomAction::Result::Unknown;

    auto result = std::pair<CustomAction::Result, CustomAction::ActionMetadata>(
        CustomAction::Result::Unknown, action_metadata);

    std::ifstream metadata_file(file);
    if (!metadata_file) {
        LogErr() << "Unable to open JSON file: " << file;
        parsing_result = CustomAction::Result::Error;
    }

    std::stringstream ss;
    ss << metadata_file.rdbuf();
    metadata_file.close();
    const auto raw_json = ss.str();

    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    JSONCPP_STRING err;
    const bool ok =
        reader->parse(raw_json.c_str(), raw_json.c_str() + raw_json.length(), &root, &err);
    if (!ok) {
        LogErr() << "Parse error: " << err;
        parsing_result = CustomAction::Result::Error;
    }

    // Get the metadata specific to that action
    std::stringstream action_id;
    action_id << "action_" << action.id;
    auto action_root = root[action_id.str()];

    action_metadata.id = action.id;
    action_metadata.action_name = action_root["name"].asString();
    action_metadata.action_description = action_root["description"].asString();
    if (action_root["action_complete_condition"].asString() != "") {
        if (action_root["action_complete_condition"].asString() == "ON_LAST_STAGE_COMPLETE") {
            if (action_root["stages"].size() == 0) {
                LogErr() << "No stages set for action #" << action.id
                         << " although \"ON_STAGE_COMPLETE\" is set.";
                parsing_result = CustomAction::Result::Error;
            } else {
                action_metadata.action_complete_condition =
                    CustomAction::ActionMetadata::ActionCompleteCondition::OnLastStageComplete;
            }
        } else if (action_root["action_complete_condition"].asString() == "ON_TIMEOUT") {
            if (action_root["global_timeout"].isNull()) {
                LogErr() << "No global timeout set for action #" << action.id;
                parsing_result = CustomAction::Result::Error;
            } else {
                action_metadata.action_complete_condition =
                    CustomAction::ActionMetadata::ActionCompleteCondition::OnTimeout;
            }
        } else if (action_root["action_complete_condition"].asString() == "ON_RESULT_SUCCESS") {
            action_metadata.action_complete_condition =
                CustomAction::ActionMetadata::ActionCompleteCondition::OnResultSuccess;
        } else if (
            action_root["action_complete_condition"].asString() == "ON_CUSTOM_CONDITION_TRUE") {
            action_metadata.action_complete_condition =
                CustomAction::ActionMetadata::ActionCompleteCondition::OnCustomConditionTrue;
        } else if (
            action_root["action_complete_condition"].asString() == "ON_CUSTOM_CONDITION_FALSE") {
            action_metadata.action_complete_condition =
                CustomAction::ActionMetadata::ActionCompleteCondition::OnCustomConditionFalse;
        } else {
            LogErr()
                << "Uknown condition string for action complete was set for action #" << action.id
                << ". Valid conditions are \"ON_STAGE_COMPLETE\", \"ON_TIMEOUT\", \"ON_RESULT_SUCCESS\", \"ON_CUSTOM_CONDITION_TRUE\" and \"ON_CUSTOM_CONDITION_FALSE\".";
            parsing_result = CustomAction::Result::Error;
        }

    } else {
        LogErr() << "No condition for action completion was set for action #" << action.id;
        parsing_result = CustomAction::Result::Error;
    }

    action_metadata.global_timeout = action_root["global_timeout"].isNull() ?
                                         double(NAN) :
                                         action_root["global_timeout"].asDouble();

    // If the action triggers a global script, pass it instead to the client
    if (action_root["global_script"].asString() != "") {
        action_metadata.global_script = action_root["global_script"].asString();
        parsing_result = CustomAction::Result::Success;
    } else {
        // Get the action stages
        if (action_root["stages"].size() > 0) {
            for (Json::Value::ArrayIndex i = 0; i != action_root["stages"].size(); i++) {
                auto stage_id = action_root["stages"][i];
                CustomAction::Stage stage{};

                parsing_result = CustomAction::Result::Success;

                // If it is to run a script, pass the script to the client side
                if (stage_id["script"].asString() != "") {
                    stage.script = stage_id["script"].asString();

                } else if (
                    stage_id["parameter_set"]["parameter_name"].asString() !=
                    "") { // Else, if a parameter is to be set, pass the parameter to the client
                          // side
                    CustomAction::Parameter param{};

                    param.name = stage_id["parameter_set"]["parameter_name"].asString();

                    if (stage_id["parameter_set"]["parameter_type"].asString() == "") {
                        LogErr() << "Parameter "
                                 << stage_id["parameter_set"]["parameter_name"].asString()
                                 << " set for stage " << i << " of action #" << action.id
                                 << " does not contain a type.";
                        parsing_result = CustomAction::Result::Error;
                        break;
                    } else {
                        if (stage_id["parameter_set"]["parameter_type"].asString() == "INT") {
                            param.type = CustomAction::Parameter::ParameterType::Int;
                        } else if (
                            stage_id["parameter_set"]["parameter_type"].asString() == "FLOAT") {
                            param.type = CustomAction::Parameter::ParameterType::Float;
                        } else {
                            LogErr() << "Invalid parameter type for stage " << i << " of action #"
                                     << action.id << ". Valid ones are \"INT\" and \"FLOAT\"";
                            parsing_result = CustomAction::Result::Error;
                            break;
                        }
                    }

                    if (stage_id["parameter_set"]["parameter_value"].asString() == "") {
                        LogErr() << "Parameter "
                                 << stage_id["parameter_set"]["parameter_name"].asString()
                                 << " set for stage " << i << " of action #" << action.id
                                 << " does not contain a value.";
                        parsing_result = CustomAction::Result::Error;
                        break;
                    } else {
                        param.value =
                            std::stof(stage_id["parameter_set"]["parameter_value"].asString());
                    }

                    stage.parameter_set = param;
                } else { // Else, pass the command to the client side
                    CustomAction::Command cmd{};
                    if (stage_id["cmd"]["type"].asString() == "LONG") {
                        cmd.type = CustomAction::Command::CommandType::Long;
                    } else if (stage_id["cmd"]["type"].asString() == "INT") {
                        cmd.type = CustomAction::Command::CommandType::Int;
                    } else {
                        LogErr() << "Invalid command type. Valid ones are \"INT\" and \"LONG\"";
                        parsing_result = CustomAction::Result::Error;
                        break;
                    }

                    cmd.target_system_id = stage_id["cmd"]["target_system"].asInt();
                    cmd.target_component_id = stage_id["cmd"]["target_system"].asInt();
                    cmd.frame = stage_id["cmd"]["frame"].asInt();
                    cmd.command = stage_id["cmd"]["command"].asInt();
                    cmd.param1 = stage_id["cmd"]["param1"].isNull() ?
                                     double(NAN) :
                                     stage_id["cmd"]["param1"].asDouble();
                    cmd.param2 = stage_id["cmd"]["param2"].isNull() ?
                                     double(NAN) :
                                     stage_id["cmd"]["param2"].asDouble();
                    cmd.param3 = stage_id["cmd"]["param3"].isNull() ?
                                     double(NAN) :
                                     stage_id["cmd"]["param3"].asDouble();
                    cmd.param4 = stage_id["cmd"]["param4"].isNull() ?
                                     double(NAN) :
                                     stage_id["cmd"]["param4"].asDouble();
                    cmd.param5 = stage_id["cmd"]["param5"].isNull() ?
                                     double(NAN) :
                                     stage_id["cmd"]["param5"].asDouble();
                    cmd.param6 = stage_id["cmd"]["param6"].isNull() ?
                                     double(NAN) :
                                     stage_id["cmd"]["param6"].asDouble();
                    cmd.param7 = stage_id["cmd"]["param7"].isNull() ?
                                     double(NAN) :
                                     stage_id["cmd"]["param7"].asDouble();
                    cmd.is_local = stage_id["cmd"]["is_local"].asBool();

                    stage.command = cmd;
                }

                if (stage_id["state_transition_condition"].asString() != "") {
                    if (stage_id["state_transition_condition"].asString() == "ON_RESULT_SUCCESS") {
                        stage.state_transition_condition =
                            CustomAction::Stage::StateTransitionCondition::OnResultSuccess;
                    } else if (stage_id["state_transition_condition"].asString() == "ON_TIMEOUT") {
                        if (stage_id["timeout"].isNull()) {
                            LogErr()
                                << "No timeout set for stage " << i << " of action #" << action.id;
                            parsing_result = CustomAction::Result::Error;
                            break;
                        }

                        stage.state_transition_condition =
                            CustomAction::Stage::StateTransitionCondition::OnTimeout;
                    } else if (
                        stage_id["state_transition_condition"].asString() ==
                        "ON_LANDING_COMPLETE") {
                        stage.state_transition_condition =
                            CustomAction::Stage::StateTransitionCondition::OnLandingComplete;
                    } else if (
                        stage_id["state_transition_condition"].asString() ==
                        "ON_TAKEOFF_COMPLETE") {
                        stage.state_transition_condition =
                            CustomAction::Stage::StateTransitionCondition::OnTakeoffComplete;
                    } else if (
                        stage_id["state_transition_condition"].asString() == "ON_MODE_CHANGE") {
                        stage.state_transition_condition =
                            CustomAction::Stage::StateTransitionCondition::OnModeChange;
                    } else if (
                        stage_id["state_transition_condition"].asString() ==
                        "ON_CUSTOM_CONDITION_TRUE") {
                        stage.state_transition_condition =
                            CustomAction::Stage::StateTransitionCondition::OnCustomConditionTrue;
                    } else if (
                        stage_id["state_transition_condition"].asString() ==
                        "ON_CUSTOM_CONDITION_FALSE") {
                        stage.state_transition_condition =
                            CustomAction::Stage::StateTransitionCondition::OnCustomConditionFalse;
                    } else {
                        LogErr()
                            << "Uknown condition string for state transition was set for stage "
                            << i << " of action #" << action.id
                            << ". Valid conditions are \"ON_RESULT_SUCCESS\", \"ON_TIMEOUT\", \"ON_LANDING_COMPLETE\", \"ON_TAKEOFF_COMPLETE\", \"ON_MODE_CHANGE\", \"ON_CUSTOM_CONDITION_TRUE\" and \"ON_CUSTOM_CONDITION_FALSE\".";
                        parsing_result = CustomAction::Result::Error;
                    }

                } else {
                    LogErr() << "No condition for state transition was set for stage " << i
                             << " of action #" << action.id;
                    parsing_result = CustomAction::Result::Error;
                    break;
                }

                stage.timeout =
                    stage_id["timeout"].isNull() ? double(NAN) : stage_id["timeout"].asDouble();

                action_metadata.stages.push_back(stage);
            }
        } else {
            LogErr() << "No global script or action stages were set for action #" << action.id;
            parsing_result = CustomAction::Result::Error;
        }
    }

    result.first = parsing_result;
    result.second = action_metadata;

    if (callback) {
        auto temp_callback = callback;
        _parent->call_user_callback(
            [temp_callback, result]() { temp_callback(result.first, result.second); });
    }
}

CustomAction::Result CustomActionImpl::execute_custom_action_stage(CustomAction::Stage& stage) const
{
    auto prom = std::promise<CustomAction::Result>();
    auto fut = prom.get_future();

    execute_custom_action_stage_async(
        stage, [&prom](CustomAction::Result result) { prom.set_value(result); });

    return fut.get();
}

void CustomActionImpl::execute_custom_action_stage_async(
    CustomAction::Stage& stage, const CustomAction::ResultCallback& callback) const
{
    // Process script
    if (stage.script != "") {
        CustomAction::Result result =
            custom_action_result_from_script_result(exec_command(stage.script));

        if (callback) {
            auto temp_callback = callback;
            _parent->call_user_callback([temp_callback, result]() { temp_callback(result); });
        }
    } else if (stage.parameter_set.name != "") { // Set parameter
        CustomAction::Result result{};
        if (stage.parameter_set.type == CustomAction::Parameter::ParameterType::Int) { // INT
            MAVLinkParameters::Result param_result = _parent->set_param_int(
                stage.parameter_set.name, static_cast<int>(stage.parameter_set.value));
            result = custom_action_result_from_mavlink_parameters_result(param_result);
        } else if (
            stage.parameter_set.type == CustomAction::Parameter::ParameterType::Float) { // FLOAT
            MAVLinkParameters::Result param_result =
                _parent->set_param_float(stage.parameter_set.name, stage.parameter_set.value);
            result = custom_action_result_from_mavlink_parameters_result(param_result);
        }

        if (callback) {
            auto temp_callback = callback;
            _parent->call_user_callback([temp_callback, result]() { temp_callback(result); });
        }

    } else { // Process command
        if (stage.command.type == CustomAction::Command::CommandType::Long) { // LONG
            MavlinkCommandSender::CommandLong command{*_parent};
            command.target_system_id = stage.command.target_system_id;
            command.target_component_id = stage.command.target_component_id;
            command.command = stage.command.command;
            command.params.param1 = stage.command.param1;
            command.params.param2 = stage.command.param2;
            command.params.param3 = stage.command.param3;
            command.params.param4 = stage.command.param4;
            command.params.param5 = stage.command.param5;
            command.params.param6 = stage.command.param6;
            command.params.param7 = stage.command.param7;

            // Send command to the target system and component IDs
            _parent->send_command_async(
                command, [this, callback](MavlinkCommandSender::Result cmd_result, float) {
                    command_result_callback(cmd_result, callback);
                });
        } else if (stage.command.type == CustomAction::Command::CommandType::Int) { // INT
            MavlinkCommandSender::CommandInt command{};
            command.target_system_id = stage.command.target_system_id;
            command.target_component_id = stage.command.target_component_id;
            command.frame = static_cast<MAV_FRAME>(stage.command.frame);
            command.command = stage.command.command;
            command.params.param1 = stage.command.param1;
            command.params.param2 = stage.command.param2;
            command.params.param3 = stage.command.param3;
            command.params.param4 = stage.command.param4;
            command.params.x = stage.command.is_local ?
                                   static_cast<int32_t>(std::round(stage.command.param5 * 1e4)) :
                                   static_cast<int32_t>(std::round(stage.command.param5 * 1e7));
            command.params.y = stage.command.is_local ?
                                   static_cast<int32_t>(std::round(stage.command.param6 * 1e4)) :
                                   static_cast<int32_t>(std::round(stage.command.param6 * 1e7));
            command.params.z = stage.command.param7;

            // Send command to the target system and component IDs
            _parent->send_command_async(
                command, [this, callback](MavlinkCommandSender::Result cmd_result, float) {
                    command_result_callback(cmd_result, callback);
                });
        }
    }
}

CustomAction::Result
CustomActionImpl::execute_custom_action_global_script(std::string& global_script) const
{
    auto prom = std::promise<CustomAction::Result>();
    auto fut = prom.get_future();

    execute_custom_action_global_script_async(
        global_script, [&prom](CustomAction::Result result) { prom.set_value(result); });

    return fut.get();
}

void CustomActionImpl::execute_custom_action_global_script_async(
    std::string& global_script, const CustomAction::ResultCallback& callback) const
{
    if (global_script != "") {
        CustomAction::Result result =
            custom_action_result_from_script_result(exec_command(global_script));

        if (callback) {
            auto temp_callback = callback;
            _parent->call_user_callback([temp_callback, result]() { temp_callback(result); });
        }
    }
}

int CustomActionImpl::exec_command(const std::string& cmd_str)
{
    const char* cmd = cmd_str.c_str();

#ifdef __APPLE__
    pid_t pid;
    char* argv[] = {"sh", "-c", (char*)cmd, NULL};
    int status;

    status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
    if (status == 0) {
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
#else
    if (!system(NULL)) {
        return -1;
    }

    return system(cmd);
#endif
}

CustomAction::Result
CustomActionImpl::custom_action_result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return CustomAction::Result::Success;
        case MavlinkCommandSender::Result::Timeout:
            return CustomAction::Result::Timeout;
        case MavlinkCommandSender::Result::NoSystem:
        case MavlinkCommandSender::Result::ConnectionError:
        case MavlinkCommandSender::Result::Busy:
        case MavlinkCommandSender::Result::CommandDenied:
        default:
            return CustomAction::Result::Error;
    }
}

MAV_RESULT
CustomActionImpl::mavlink_command_result_from_custom_action_result(CustomAction::Result result)
{
    switch (result) {
        case CustomAction::Result::Unknown:
        case CustomAction::Result::Error:
        case CustomAction::Result::Timeout:
            return MAV_RESULT_FAILED;
        case CustomAction::Result::Success:
            return MAV_RESULT_ACCEPTED;
        case CustomAction::Result::Unsupported:
            return MAV_RESULT_UNSUPPORTED;
        case CustomAction::Result::InProgress:
            return MAV_RESULT_IN_PROGRESS;
        default:
            return MAV_RESULT_FAILED;
    }
}

CustomAction::Result CustomActionImpl::custom_action_result_from_mavlink_parameters_result(
    MAVLinkParameters::Result result)
{
    switch (result) {
        case MAVLinkParameters::Result::Success:
            return CustomAction::Result::Success;
        case MAVLinkParameters::Result::Timeout:
            return CustomAction::Result::Timeout;
        case MAVLinkParameters::Result::ParamNameTooLong:
        case MAVLinkParameters::Result::WrongType:
        case MAVLinkParameters::Result::ConnectionError:
        default:
            return CustomAction::Result::Error;
    }
}

CustomAction::Result CustomActionImpl::custom_action_result_from_script_result(int result)
{
    switch (result) {
        case 0:
            return CustomAction::Result::Success;
        case -1:
        case 1:
        default:
            return CustomAction::Result::Error;
    }
}

void CustomActionImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, const CustomAction::ResultCallback& callback) const
{
    CustomAction::Result action_result = custom_action_result_from_command_result(command_result);

    if (callback) {
        auto temp_callback = callback;
        _parent->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

} // namespace mavsdk
