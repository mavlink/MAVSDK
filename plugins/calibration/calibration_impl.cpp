#include "calibration_impl.h"
#include "system.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <functional>
#include <string>

namespace dronecode_sdk {

using namespace std::placeholders;

CalibrationImpl::CalibrationImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

CalibrationImpl::~CalibrationImpl()
{
    _parent->unregister_plugin(this);
}

void CalibrationImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_STATUSTEXT, std::bind(&CalibrationImpl::process_statustext, this, _1), this);
}

void CalibrationImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void CalibrationImpl::enable()
{
    // The calibration check should eventually be better than this.
    // For now, we just do the same as QGC does.
    _parent->get_param_int_async(std::string("CAL_GYRO0_ID"),
                                 std::bind(&CalibrationImpl::receive_param_cal_gyro, this, _1, _2));

    _parent->get_param_int_async(
        std::string("CAL_ACC0_ID"),
        std::bind(&CalibrationImpl::receive_param_cal_accel, this, _1, _2));

    _parent->get_param_int_async(std::string("CAL_MAG0_ID"),
                                 std::bind(&CalibrationImpl::receive_param_cal_mag, this, _1, _2));
}

void CalibrationImpl::disable() {}

void CalibrationImpl::calibrate_gyro_async(const Calibration::calibration_callback_t &callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        report_failed("System is armed.");
        return;
    }

    if (_state != State::NONE) {
        Calibration::ProgressData progress_data(false, NAN, false, "");
        call_user_callback(callback, Calibration::Result::BUSY, progress_data);
        return;
    }

    _state = State::GYRO_CALIBRATION;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param1 = 1.0f; // Gyro
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _parent->send_command_async(command,
                                std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::call_user_callback(const Calibration::calibration_callback_t &callback,
                                         const Calibration::Result &result,
                                         const Calibration::ProgressData &progress_data)
{
    if (callback) {
        _parent->call_user_callback(
            [callback, result, progress_data]() { callback(result, progress_data); });
    }
}

void CalibrationImpl::calibrate_accelerometer_async(
    const Calibration::calibration_callback_t &callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        report_failed("System is armed.");
        return;
    }

    if (_state != State::NONE) {
        Calibration::ProgressData progress_data(false, NAN, false, "");
        call_user_callback(callback, Calibration::Result::BUSY, progress_data);
        return;
    }

    _state = State::ACCELEROMETER_CALIBRATION;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param5 = 1.0f; // Accel
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _parent->send_command_async(command,
                                std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::calibrate_magnetometer_async(
    const Calibration::calibration_callback_t &callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        report_failed("System is armed.");
        return;
    }

    if (_state != State::NONE) {
        Calibration::ProgressData progress_data(false, NAN, false, "");
        call_user_callback(callback, Calibration::Result::BUSY, progress_data);
        return;
    }

    _state = State::MAGNETOMETER_CALIBRATION;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param2 = 1.0f; // Mag
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _parent->send_command_async(command,
                                std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::calibrate_gimbal_accelerometer_async(
    const Calibration::calibration_callback_t &callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        report_failed("System is armed.");
        return;
    }

    if (_state != State::NONE) {
        Calibration::ProgressData progress_data(false, NAN, false, "");
        call_user_callback(callback, Calibration::Result::BUSY, progress_data);
        return;
    }

    _state = State::GIMBAL_ACCELEROMETER_CALIBRATION;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param5 = 1.0f; // Accel
    command.target_component_id = MAV_COMP_ID_GIMBAL;
    _parent->send_command_async(command,
                                std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

bool CalibrationImpl::is_gyro_calibration_ok() const
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);
    return _is_gyro_ok;
}

bool CalibrationImpl::is_accelerometer_calibration_ok() const
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);
    return _is_accelerometer_ok;
}

bool CalibrationImpl::is_magnetometer_calibration_ok() const
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);
    return _is_magnetometer_ok;
}

void CalibrationImpl::command_result_callback(MAVLinkCommands::Result command_result,
                                              float progress)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_state == State::NONE) {
        // It might be someone else like a ground station trying to do a
        // calibration. We silently ignore it.
        return;
    }

    // If we get a progress result here, it is the new interface and we can
    // use the progress info. If we get an ack, we need to translate that to
    // a first progress update, and then parse the statustexts for progress.
    switch (command_result) {
        case MAVLinkCommands::Result::SUCCESS:
            // Silently ignore.
            break;

        case MAVLinkCommands::Result::NO_SYSTEM:
            // FALLTHROUGH
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            // FALLTHROUGH
        case MAVLinkCommands::Result::BUSY:
            // FALLTHROUGH
        case MAVLinkCommands::Result::COMMAND_DENIED:
            // FALLTHROUGH
        case MAVLinkCommands::Result::UNKNOWN_ERROR:
            // FALLTHROUGH
        case MAVLinkCommands::Result::TIMEOUT: {
            // Report all error cases.
            const auto timeout_result = calibration_result_from_command_result(command_result);
            call_user_callback(_calibration_callback,
                               timeout_result,
                               Calibration::ProgressData(false, NAN, false, ""));
            _calibration_callback = nullptr;
            _state = State::NONE;
            break;
        }

        case MAVLinkCommands::Result::IN_PROGRESS: {
            const auto progress_result = calibration_result_from_command_result(command_result);
            call_user_callback(_calibration_callback,
                               progress_result,
                               Calibration::ProgressData(true, progress, false, ""));
            break;
        }
    };
}

Calibration::Result
CalibrationImpl::calibration_result_from_command_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Calibration::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return Calibration::Result::NO_SYSTEM;
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return Calibration::Result::CONNECTION_ERROR;
        case MAVLinkCommands::Result::BUSY:
            return Calibration::Result::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return Calibration::Result::COMMAND_DENIED;
        case MAVLinkCommands::Result::TIMEOUT:
            return Calibration::Result::TIMEOUT;
        case MAVLinkCommands::Result::IN_PROGRESS:
            return Calibration::Result::IN_PROGRESS;
        default:
            return Calibration::Result::UNKNOWN;
    }
}

void CalibrationImpl::receive_param_cal_gyro(MAVLinkParameters::Result result, int value)
{
    if (result != MAVLinkParameters::Result::SUCCESS) {
        LogErr() << "Error: Param for gyro cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_gyro_calibration(ok);
}

void CalibrationImpl::receive_param_cal_accel(MAVLinkParameters::Result result, int value)
{
    if (result != MAVLinkParameters::Result::SUCCESS) {
        LogErr() << "Error: Param for accel cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_accelerometer_calibration(ok);
}

void CalibrationImpl::receive_param_cal_mag(MAVLinkParameters::Result result, int value)
{
    if (result != MAVLinkParameters::Result::SUCCESS) {
        LogErr() << "Error: Param for mag cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_magnetometer_calibration(ok);
}

void CalibrationImpl::set_gyro_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    _is_gyro_ok = ok;
}

void CalibrationImpl::set_accelerometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    _is_accelerometer_ok = ok;
}

void CalibrationImpl::set_magnetometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    _is_magnetometer_ok = ok;
}

void CalibrationImpl::process_statustext(const mavlink_message_t &message)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);
    if (_state == State::NONE) {
        return;
    }

    mavlink_statustext_t statustext;
    mavlink_msg_statustext_decode(&message, &statustext);

    _parser.reset();
    _parser.parse(statustext.text);

    switch (_parser.get_status()) {
        case CalibrationStatustextParser::Status::NONE:
            // Ignore it.
            break;
        case CalibrationStatustextParser::Status::STARTED:
            report_started();
            break;
        case CalibrationStatustextParser::Status::DONE:
            report_done();
            break;
        case CalibrationStatustextParser::Status::FAILED:
            report_failed(_parser.get_failed_message());
            break;
        case CalibrationStatustextParser::Status::CANCELLED:
            report_cancelled();
            break;
        case CalibrationStatustextParser::Status::PROGRESS:
            report_progress(_parser.get_progress());
            break;
        case CalibrationStatustextParser::Status::INSTRUCTION:
            report_instruction(_parser.get_instruction());
            break;
    }

    switch (_parser.get_status()) {
        case CalibrationStatustextParser::Status::DONE:
            // FALLTHROUGH
        case CalibrationStatustextParser::Status::FAILED:
            // FALLTHROUGH
        case CalibrationStatustextParser::Status::CANCELLED:
            _calibration_callback = nullptr;
            _state = State::NONE;
            break;
        default:
            break;
    }
}

void CalibrationImpl::report_started()
{
    report_progress(0.0f);
}

void CalibrationImpl::report_done()
{
    const Calibration::ProgressData progress_data(false, NAN, false, "");
    call_user_callback(_calibration_callback, Calibration::Result::SUCCESS, progress_data);
}

void CalibrationImpl::report_failed(const std::string &failed)
{
    LogErr() << "Calibration failed: " << failed;
    const Calibration::ProgressData progress_data(false, NAN, false, "");
    call_user_callback(_calibration_callback, Calibration::Result::FAILED, progress_data);
}

void CalibrationImpl::report_cancelled()
{
    LogWarn() << "Calibration was cancelled";
    const Calibration::ProgressData progress_data(false, NAN, false, "");
    call_user_callback(_calibration_callback, Calibration::Result::CANCELLED, progress_data);
}

void CalibrationImpl::report_progress(float progress)
{
    const Calibration::ProgressData progress_data(true, progress, false, "");
    call_user_callback(_calibration_callback, Calibration::Result::IN_PROGRESS, progress_data);
}

void CalibrationImpl::report_instruction(const std::string &instruction)
{
    const Calibration::ProgressData progress_data(false, NAN, true, instruction);
    call_user_callback(_calibration_callback, Calibration::Result::INSTRUCTION, progress_data);
}

} // namespace dronecode_sdk
