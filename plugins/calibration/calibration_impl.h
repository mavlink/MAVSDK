#pragma once

#include "plugins/calibration/calibration.h"
#include "system.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "calibration_statustext_parser.h"

namespace dronecode_sdk {

class CalibrationImpl : public PluginImplBase {
public:
    CalibrationImpl(System &system);
    ~CalibrationImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void calibrate_gyro_async(const Calibration::calibration_callback_t &callback);
    void calibrate_accelerometer_async(const Calibration::calibration_callback_t &callback);
    void calibrate_magnetometer_async(const Calibration::calibration_callback_t &callback);
    void calibrate_gimbal_accelerometer_async(const Calibration::calibration_callback_t &callback);

    bool is_gyro_calibration_ok() const;
    bool is_accelerometer_calibration_ok() const;
    bool is_magnetometer_calibration_ok() const;

private:
    void call_user_callback(const Calibration::calibration_callback_t &callback,
                            const Calibration::Result &result,
                            const Calibration::ProgressData &progress_data);
    void process_statustext(const mavlink_message_t &message);

    void command_result_callback(MAVLinkCommands::Result command_result, float progress);

    static Calibration::Result
    calibration_result_from_command_result(MAVLinkCommands::Result result);

    void receive_param_cal_gyro(MAVLinkParameters::Result result, int value);
    void receive_param_cal_accel(MAVLinkParameters::Result result, int value);
    void receive_param_cal_mag(MAVLinkParameters::Result result, int value);

    void set_gyro_calibration(bool ok);
    void set_accelerometer_calibration(bool ok);
    void set_magnetometer_calibration(bool ok);

    void report_started();
    void report_done();
    void report_warning(const std::string &warning);
    void report_failed(const std::string &failed);
    void report_cancelled();
    void report_progress(float progress);
    void report_instruction(const std::string &instruction);

    CalibrationStatustextParser _parser{};

    mutable std::mutex _calibration_mutex{};

    bool _is_gyro_ok = false;
    bool _is_accelerometer_ok = false;
    bool _is_magnetometer_ok = false;

    std::atomic<bool> _is_gyro_running = {false};
    std::atomic<bool> _is_accelerometer_running = {false};
    std::atomic<bool> _is_magnetometer_running = {false};

    enum class State {
        NONE,
        GYRO_CALIBRATION,
        ACCELEROMETER_CALIBRATION,
        MAGNETOMETER_CALIBRATION,
        GIMBAL_ACCELEROMETER_CALIBRATION
    } _state{State::NONE};

    Calibration::calibration_callback_t _calibration_callback{nullptr};
};

} // namespace dronecode_sdk
