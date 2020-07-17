#include "calibration_impl.h"

#include <functional>
#include <string>

#include "global_include.h"
#include "px4_custom_mode.h"
#include "system.h"

namespace mavsdk {

using namespace std::placeholders;

CalibrationImpl::CalibrationImpl(System& system) : PluginImplBase(system)
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

void CalibrationImpl::enable() {}

void CalibrationImpl::disable() {}

void CalibrationImpl::calibrate_gyro_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::FailedArmed, progress_data);
        return;
    }

    if (_state != State::None) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::Busy, progress_data);
        return;
    }

    _state = State::GyroCalibration;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param1 = 1.0f; // Gyro
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _parent->send_command_async(
        command, std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::call_callback(
    const CalibrationCallback& callback,
    const Calibration::Result& result,
    const Calibration::ProgressData progress_data)
{
    if (callback) {
        _parent->call_user_callback(
            [callback, result, progress_data]() { callback(result, progress_data); });
    }
}

void CalibrationImpl::calibrate_accelerometer_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::FailedArmed, progress_data);
        return;
    }

    if (_state != State::None) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::Busy, progress_data);
        return;
    }

    _state = State::AccelerometerCalibration;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param5 = 1.0f; // Accel
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _parent->send_command_async(
        command, std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::calibrate_magnetometer_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::FailedArmed, progress_data);
        return;
    }

    if (_state != State::None) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::Busy, progress_data);
        return;
    }

    _state = State::MagnetometerCalibration;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param2 = 1.0f; // Mag
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _parent->send_command_async(
        command, std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::calibrate_level_horizon_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::FailedArmed, progress_data);
        return;
    }

    if (_state != State::None) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::Busy, progress_data);
        return;
    }

    _state = State::AccelerometerCalibration;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param5 = 2.0f; // Board Level
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _parent->send_command_async(
        command, std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::calibrate_gimbal_accelerometer_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_parent->is_armed()) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::FailedArmed, progress_data);
        return;
    }

    if (_state != State::None) {
        Calibration::ProgressData progress_data;
        call_callback(callback, Calibration::Result::Busy, progress_data);
        return;
    }

    _state = State::GimbalAccelerometerCalibration;
    _calibration_callback = callback;

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.params.param5 = 1.0f; // Accel
    command.target_component_id = MAV_COMP_ID_GIMBAL;
    _parent->send_command_async(
        command, std::bind(&CalibrationImpl::command_result_callback, this, _1, _2));
}

void CalibrationImpl::cancel() const
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    uint8_t target_component_id = MAV_COMP_ID_AUTOPILOT1;

    switch (_state) {
        case State::None:
            LogWarn() << "No calibration to cancel";
            return;
        case State::GyroCalibration:
            break;
        case State::AccelerometerCalibration:
            break;
        case State::MagnetometerCalibration:
            break;
        case State::LevelHorizonCalibration:
            break;
        case State::GimbalAccelerometerCalibration:
            target_component_id = MAV_COMP_ID_GIMBAL;
            break;
    }

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    // All params 0 signal cancellation of a calibration.
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.0f);
    command.target_component_id = target_component_id;
    // We don't care about the result, the initial callback should get notified about it.
    _parent->send_command_async(command, nullptr);
}

void CalibrationImpl::command_result_callback(
    MAVLinkCommands::Result command_result, float progress)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_state == State::None) {
        // It might be someone else like a ground station trying to do a
        // calibration. We silently ignore it.
        return;
    }

    // If we get a progress result here, it is the new interface and we can
    // use the progress info. If we get an ack, we need to translate that to
    // a first progress update, and then parse the statustexts for progress.
    switch (command_result) {
        case MAVLinkCommands::Result::Success:
            // Silently ignore.
            break;

        case MAVLinkCommands::Result::NoSystem:
            // FALLTHROUGH
        case MAVLinkCommands::Result::ConnectionError:
            // FALLTHROUGH
        case MAVLinkCommands::Result::Busy:
            // FALLTHROUGH
        case MAVLinkCommands::Result::CommandDenied:
            // FALLTHROUGH
        case MAVLinkCommands::Result::UnknownError:
            // FALLTHROUGH
        case MAVLinkCommands::Result::Timeout: {
            // Report all error cases.
            const auto timeout_result = calibration_result_from_command_result(command_result);
            call_callback(_calibration_callback, timeout_result, Calibration::ProgressData());
            _calibration_callback = nullptr;
            _state = State::None;
            break;
        }

        case MAVLinkCommands::Result::InProgress: {
            const auto progress_result = calibration_result_from_command_result(command_result);
            Calibration::ProgressData progress_data;
            progress_data.has_progress = true;
            progress_data.progress = progress;

            call_callback(_calibration_callback, progress_result, progress_data);
            break;
        }
    };
}

Calibration::Result
CalibrationImpl::calibration_result_from_command_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::Success:
            return Calibration::Result::Success;
        case MAVLinkCommands::Result::NoSystem:
            return Calibration::Result::NoSystem;
        case MAVLinkCommands::Result::ConnectionError:
            return Calibration::Result::ConnectionError;
        case MAVLinkCommands::Result::Busy:
            return Calibration::Result::Busy;
        case MAVLinkCommands::Result::CommandDenied:
            return Calibration::Result::CommandDenied;
        case MAVLinkCommands::Result::Timeout:
            return Calibration::Result::Timeout;
        case MAVLinkCommands::Result::InProgress:
            return Calibration::Result::Next;
        default:
            return Calibration::Result::Unknown;
    }
}

void CalibrationImpl::process_statustext(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);
    if (_state == State::None) {
        return;
    }

    mavlink_statustext_t statustext;
    mavlink_msg_statustext_decode(&message, &statustext);

    _parser.reset();
    _parser.parse(statustext.text);

    switch (_parser.get_status()) {
        case CalibrationStatustextParser::Status::None:
            // Ignore it.
            break;
        case CalibrationStatustextParser::Status::Started:
            report_started();
            break;
        case CalibrationStatustextParser::Status::Done:
            report_done();
            break;
        case CalibrationStatustextParser::Status::Failed:
            report_failed(_parser.get_failed_message());
            break;
        case CalibrationStatustextParser::Status::Cancelled:
            report_cancelled();
            break;
        case CalibrationStatustextParser::Status::Progress:
            report_progress(_parser.get_progress());
            break;
        case CalibrationStatustextParser::Status::Instruction:
            report_instruction(_parser.get_instruction());
            break;
    }

    // In case we succeed or fail we need to notify that params
    // might have changed.
    switch (_parser.get_status()) {
        case CalibrationStatustextParser::Status::Done:
            // FALLTHROUGH
        case CalibrationStatustextParser::Status::Failed:
            // FALLTHROUGH
        case CalibrationStatustextParser::Status::Cancelled:
            switch (_state) {
                case State::None:
                    break;
                case State::GyroCalibration:
                    _parent->param_changed("CAL_GYRO0_ID");
                    break;
                case State::AccelerometerCalibration:
                    _parent->param_changed("CAL_ACC0_ID");
                    break;
                case State::MagnetometerCalibration:
                    _parent->param_changed("CAL_MAG0_ID");
                    break;
                case State::LevelHorizonCalibration:
                    _parent->param_changed("SENS_BOARD_X_OFF");
                    _parent->param_changed("SENS_BOARD_Y_OFF");
                    _parent->param_changed("SENS_BOARD_Z_OFF");
                    break;
                case State::GimbalAccelerometerCalibration:
                    break;
            }

        default:
            break;
    }

    switch (_parser.get_status()) {
        case CalibrationStatustextParser::Status::Done:
            // FALLTHROUGH
        case CalibrationStatustextParser::Status::Failed:
            // FALLTHROUGH
        case CalibrationStatustextParser::Status::Cancelled:
            _calibration_callback = nullptr;
            _state = State::None;
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
    const Calibration::ProgressData progress_data;
    call_callback(_calibration_callback, Calibration::Result::Success, progress_data);
}

void CalibrationImpl::report_failed(const std::string& failed)
{
    LogErr() << "Calibration failed: " << failed;
    const Calibration::ProgressData progress_data;
    call_callback(_calibration_callback, Calibration::Result::Failed, progress_data);
}

void CalibrationImpl::report_cancelled()
{
    LogWarn() << "Calibration was cancelled";
    const Calibration::ProgressData progress_data;
    call_callback(_calibration_callback, Calibration::Result::Cancelled, progress_data);
}

void CalibrationImpl::report_progress(float progress)
{
    Calibration::ProgressData progress_data;
    progress_data.has_progress = true;
    progress_data.progress = progress;
    call_callback(_calibration_callback, Calibration::Result::Next, progress_data);
}

void CalibrationImpl::report_instruction(const std::string& instruction)
{
    Calibration::ProgressData progress_data;
    progress_data.has_status_text = true;
    progress_data.status_text = instruction;
    call_callback(_calibration_callback, Calibration::Result::Next, progress_data);
}

} // namespace mavsdk
