#include <future>

#include "calibration/calibration.grpc.pb.h"
#include "plugins/calibration/calibration.h"

namespace mavsdk {
namespace backend {

template<typename Calibration = Calibration>
class CalibrationServiceImpl final : public rpc::calibration::CalibrationService::Service {
public:
    CalibrationServiceImpl(Calibration& calibration) : _calibration(calibration) {}

    static std::unique_ptr<rpc::calibration::CalibrationResult>
    translateCalibrationResult(const mavsdk::Calibration::Result& calibration_result)
    {
        auto rpc_calibration_result = std::make_unique<rpc::calibration::CalibrationResult>(
            rpc::calibration::CalibrationResult());

        auto rpc_result =
            static_cast<rpc::calibration::CalibrationResult::Result>(calibration_result);
        rpc_calibration_result->set_result(rpc_result);
        rpc_calibration_result->set_result_str(mavsdk::Calibration::result_str(calibration_result));

        return rpc_calibration_result;
    }

    static std::unique_ptr<rpc::calibration::ProgressData>
    translateProgressData(const mavsdk::Calibration::ProgressData& progress_data)
    {
        auto rpc_progress_data =
            std::make_unique<rpc::calibration::ProgressData>(rpc::calibration::ProgressData());

        rpc_progress_data->set_has_progress(progress_data.has_progress);
        rpc_progress_data->set_progress(progress_data.progress);
        rpc_progress_data->set_has_status_text(progress_data.has_status_text);
        rpc_progress_data->set_status_text(progress_data.status_text);

        return rpc_progress_data;
    }

    grpc::Status SubscribeCalibrateGyro(
        grpc::ServerContext* /* context */,
        const rpc::calibration::SubscribeCalibrateGyroRequest* request,
        grpc::ServerWriter<rpc::calibration::CalibrateGyroResponse>* writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        auto is_finished = std::make_shared<bool>(false);

        _calibration.calibrate_gyro_async(
            [this, &writer, &stream_closed_promise, is_finished](
                const mavsdk::Calibration::Result result,
                const mavsdk::Calibration::ProgressData progress_data) {
                rpc::calibration::CalibrateGyroResponse rpc_response;
                rpc_response.set_allocated_calibration_result(
                    translateCalibrationResult(result).release());
                rpc_response.set_allocated_progress_data(
                    translateProgressData(progress_data).release());

                std::lock_guard<std::mutex> lock(_subscribe_mutex);
                if (!*is_finished && !writer->Write(rpc_response)) {
                    _calibration.calibrate_gyro_async(nullptr);
                    *is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCalibrateAccelerometer(
        grpc::ServerContext* /* context */,
        const rpc::calibration::SubscribeCalibrateAccelerometerRequest* request,
        grpc::ServerWriter<rpc::calibration::CalibrateAccelerometerResponse>* writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        auto is_finished = std::make_shared<bool>(false);

        _calibration.calibrate_accelerometer_async(
            [this, &writer, &stream_closed_promise, is_finished](
                const mavsdk::Calibration::Result result,
                const mavsdk::Calibration::ProgressData progress_data) {
                rpc::calibration::CalibrateAccelerometerResponse rpc_response;
                rpc_response.set_allocated_calibration_result(
                    translateCalibrationResult(result).release());
                rpc_response.set_allocated_progress_data(
                    translateProgressData(progress_data).release());

                std::lock_guard<std::mutex> lock(_subscribe_mutex);

                if (!*is_finished && !writer->Write(rpc_response)) {
                    _calibration.calibrate_accelerometer_async(nullptr);
                    *is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCalibrateMagnetometer(
        grpc::ServerContext* /* context */,
        const rpc::calibration::SubscribeCalibrateMagnetometerRequest* request,
        grpc::ServerWriter<rpc::calibration::CalibrateMagnetometerResponse>* writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        auto is_finished = std::make_shared<bool>(false);

        _calibration.calibrate_magnetometer_async(
            [this, &writer, &stream_closed_promise, is_finished](
                const mavsdk::Calibration::Result result,
                const mavsdk::Calibration::ProgressData progress_data) {
                rpc::calibration::CalibrateMagnetometerResponse rpc_response;
                rpc_response.set_allocated_calibration_result(
                    translateCalibrationResult(result).release());
                rpc_response.set_allocated_progress_data(
                    translateProgressData(progress_data).release());

                std::lock_guard<std::mutex> lock(_subscribe_mutex);
                if (!*is_finished && !writer->Write(rpc_response)) {
                    _calibration.calibrate_magnetometer_async(nullptr);
                    *is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCalibrateGimbalAccelerometer(
        grpc::ServerContext* /* context */,
        const rpc::calibration::SubscribeCalibrateGimbalAccelerometerRequest* request,
        grpc::ServerWriter<rpc::calibration::CalibrateGimbalAccelerometerResponse>* writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        auto is_finished = std::make_shared<bool>(false);

        _calibration.calibrate_gimbal_accelerometer_async(
            [this, &writer, &stream_closed_promise, is_finished](
                const mavsdk::Calibration::Result result,
                const mavsdk::Calibration::ProgressData progress_data) {
                rpc::calibration::CalibrateGimbalAccelerometerResponse rpc_response;
                rpc_response.set_allocated_calibration_result(
                    translateCalibrationResult(result).release());
                rpc_response.set_allocated_progress_data(
                    translateProgressData(progress_data).release());

                std::lock_guard<std::mutex> lock(_subscribe_mutex);
                if (!*is_finished && !writer->Write(rpc_response)) {
                    _calibration.calibrate_gimbal_accelerometer_async(nullptr);
                    *is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status Cancel(
        grpc::ServerContext* /* context */,
        const rpc::calibration::CancelRequest* /* request */,
        rpc::calibration::CancelResponse* /* response */) override
    {
        _calibration.cancel_calibration();
        return grpc::Status::OK;
    }

private:
    Calibration& _calibration;
    std::mutex _subscribe_mutex{};
};

} // namespace backend
} // namespace mavsdk
