#include "plugins/calibration/calibration.h"
#include "calibration_impl.h"
#include "calibration_messages.h"

namespace dronecode_sdk {

Calibration::Calibration(System &system) : PluginBase(), _impl(new CalibrationImpl(system)) {}

Calibration::~Calibration()
{
    delete _impl;
}

void Calibration::calibrate_gyro_async(calibration_callback_t callback)
{
    _impl->calibrate_gyro_async(callback);
}

void Calibration::calibrate_accelerometer_async(calibration_callback_t callback)
{
    _impl->calibrate_accelerometer_async(callback);
}

void Calibration::calibrate_magnetometer_async(calibration_callback_t callback)
{
    _impl->calibrate_magnetometer_async(callback);
}

void Calibration::calibrate_gimbal_accelerometer_async(calibration_callback_t callback)
{
    _impl->calibrate_gimbal_accelerometer_async(callback);
}

const char *Calibration::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::IN_PROGRESS:
            return "In progress";
        case Result::INSTRUCTION:
            return "Instruction";
        case Result::FAILED:
            return "Failed";
        case Result::NO_SYSTEM:
            return "No system";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::BUSY:
            return "Busy";
        case Result::COMMAND_DENIED:
            return "Command denied";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::CANCELLED:
            return "CANCELLED";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronecode_sdk
