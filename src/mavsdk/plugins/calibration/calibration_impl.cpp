#include "calibration_impl.h"

#include <functional>
#include <string>

#include "px4_custom_mode.h"
#include "system.h"

namespace mavsdk {

CalibrationImpl::CalibrationImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

CalibrationImpl::CalibrationImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

CalibrationImpl::~CalibrationImpl()
{
    _system_impl->unregister_plugin(this);
}

void CalibrationImpl::init()
{
    _system_impl->register_statustext_handler(
        [this](const MavlinkStatustextHandler::Statustext& statustext) {
            receive_statustext(statustext);
        },
        this);
}

void CalibrationImpl::deinit()
{
    _system_impl->unregister_statustext_handler(this);
}

void CalibrationImpl::enable() {}

void CalibrationImpl::disable() {}

void CalibrationImpl::calibrate_gyro_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_system_impl->is_armed()) {
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

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    command.params.maybe_param1 = 1.0f; // Gyro
    command.params.maybe_param2 = 0.0f;
    command.params.maybe_param3 = 0.0f;
    command.params.maybe_param4 = 0.0f;
    command.params.maybe_param5 = 0.0f;
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _system_impl->send_command_async(
        command, [this](MavlinkCommandSender::Result command_result, float progress) {
            command_result_callback(command_result, progress);
        });
}

void CalibrationImpl::call_callback(
    const CalibrationCallback& callback,
    const Calibration::Result& result,
    const Calibration::ProgressData progress_data)
{
    if (callback) {
        _system_impl->call_user_callback(
            [callback, result, progress_data]() { callback(result, progress_data); });
    }
}

void CalibrationImpl::calibrate_accelerometer_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_system_impl->is_armed()) {
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

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    command.params.maybe_param1 = 0.0f;
    command.params.maybe_param2 = 0.0f;
    command.params.maybe_param3 = 0.0f;
    command.params.maybe_param4 = 0.0f;
    command.params.maybe_param5 = 1.0f; // Accel
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _system_impl->send_command_async(
        command, [this](MavlinkCommandSender::Result command_result, float progress) {
            command_result_callback(command_result, progress);
        });
}

void CalibrationImpl::calibrate_magnetometer_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_system_impl->is_armed()) {
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

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    command.params.maybe_param1 = 0.0f;
    command.params.maybe_param2 = 1.0f; // Mag
    command.params.maybe_param3 = 0.0f;
    command.params.maybe_param4 = 0.0f;
    command.params.maybe_param5 = 0.0f;
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _system_impl->send_command_async(
        command, [this](MavlinkCommandSender::Result command_result, float progress) {
            command_result_callback(command_result, progress);
        });
}

void CalibrationImpl::calibrate_level_horizon_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_system_impl->is_armed()) {
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

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    command.params.maybe_param1 = 0.0f;
    command.params.maybe_param2 = 0.0f;
    command.params.maybe_param3 = 0.0f;
    command.params.maybe_param4 = 0.0f;
    command.params.maybe_param5 = 2.0f; // Board Level
    command.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    _system_impl->send_command_async(
        command, [this](MavlinkCommandSender::Result command_result, float progress) {
            command_result_callback(command_result, progress);
        });
}

void CalibrationImpl::calibrate_gimbal_accelerometer_async(const CalibrationCallback& callback)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    if (_system_impl->is_armed()) {
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

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    command.params.maybe_param1 = 0.0f;
    command.params.maybe_param2 = 0.0f;
    command.params.maybe_param3 = 0.0f;
    command.params.maybe_param4 = 0.0f;
    command.params.maybe_param5 = 1.0f; // Accel
    command.target_component_id = MAV_COMP_ID_GIMBAL;
    _system_impl->send_command_async(
        command, [this](MavlinkCommandSender::Result command_result, float progress) {
            command_result_callback(command_result, progress);
            if (command_result == MavlinkCommandSender::Result::Success) {
                // The gimbal which implements this so far actually uses acks
                // with progress and final ack, so we need to finish at the end.
                report_done();
            }
        });
}

Calibration::Result CalibrationImpl::cancel()
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);

    uint8_t target_component_id = MAV_COMP_ID_AUTOPILOT1;

    switch (_state) {
        case State::None:
            LogWarn() << "No calibration to cancel";
            return Calibration::Result::Success;
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

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_PREFLIGHT_CALIBRATION;
    // All params 0 signal cancellation of a calibration.
    command.params.maybe_param1 = 0.0f;
    command.params.maybe_param2 = 0.0f;
    command.params.maybe_param3 = 0.0f;
    command.params.maybe_param4 = 0.0f;
    command.params.maybe_param5 = 0.0f;
    command.params.maybe_param6 = 0.0f;
    command.params.maybe_param7 = 0.0f;
    command.target_component_id = target_component_id;

    auto prom = std::promise<Calibration::Result>();
    auto fut = prom.get_future();
    _system_impl->send_command_async(
        command, [&prom](MavlinkCommandSender::Result command_result, float) {
            if (command_result != MavlinkCommandSender::Result::Success) {
                prom.set_value(Calibration::Result::ConnectionError);
            } else {
                prom.set_value(Calibration::Result::Success);
            }
        });

    return fut.get();
}

void CalibrationImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, float progress)
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
        case MavlinkCommandSender::Result::Success:
            // Silently ignore.
            break;

        case MavlinkCommandSender::Result::NoSystem:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::ConnectionError:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Busy:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Denied:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Unsupported:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Timeout:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::TemporarilyRejected:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Failed:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Cancelled:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::UnknownError: {
            // Report all error cases.
            const auto timeout_result = calibration_result_from_command_result(command_result);
            call_callback(_calibration_callback, timeout_result, Calibration::ProgressData());
            _calibration_callback = nullptr;
            _state = State::None;
            break;
        }

        case MavlinkCommandSender::Result::InProgress: {
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
CalibrationImpl::calibration_result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return Calibration::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return Calibration::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return Calibration::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
            return Calibration::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            // Fallthrough
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return Calibration::Result::CommandDenied;
        case MavlinkCommandSender::Result::Failed:
            return Calibration::Result::Failed;
        case MavlinkCommandSender::Result::Cancelled:
            return Calibration::Result::Cancelled;
        case MavlinkCommandSender::Result::Timeout:
            return Calibration::Result::Timeout;
        case MavlinkCommandSender::Result::InProgress:
            return Calibration::Result::Next;
        case MavlinkCommandSender::Result::Unsupported:
            return Calibration::Result::Unsupported;
        default:
            return Calibration::Result::Unknown;
    }
}

void CalibrationImpl::receive_statustext(const MavlinkStatustextHandler::Statustext& statustext)
{
    std::lock_guard<std::mutex> lock(_calibration_mutex);
    if (_state == State::None) {
        return;
    }

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
                    _system_impl->param_changed("CAL_GYRO0_ID");
                    break;
                case State::AccelerometerCalibration:
                    _system_impl->param_changed("CAL_ACC0_ID");
                    break;
                case State::MagnetometerCalibration:
                    _system_impl->param_changed("CAL_MAG0_ID");
                    break;
                case State::LevelHorizonCalibration:
                    _system_impl->param_changed("SENS_BOARD_X_OFF");
                    _system_impl->param_changed("SENS_BOARD_Y_OFF");
                    _system_impl->param_changed("SENS_BOARD_Z_OFF");
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
