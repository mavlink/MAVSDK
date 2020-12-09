#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/failure/failure.h>

namespace py = pybind11;
void init_failure(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::Failure> failure(m, "Failure");

    failure.def("inject", &mavsdk::Failure::inject);

    py::enum_<mavsdk::Failure::FailureUnit>(failure, "FailureUnit")
        .value("SensorGyro", mavsdk::Failure::FailureUnit::SensorGyro)
        .value("SensorAccel", mavsdk::Failure::FailureUnit::SensorAccel)
        .value("SensorMag", mavsdk::Failure::FailureUnit::SensorMag)
        .value("SensorBaro", mavsdk::Failure::FailureUnit::SensorBaro)
        .value("SensorGps", mavsdk::Failure::FailureUnit::SensorGps)
        .value("SensorOpticalFlow", mavsdk::Failure::FailureUnit::SensorOpticalFlow)
        .value("SensorVio", mavsdk::Failure::FailureUnit::SensorVio)
        .value("SensorDistanceSensor", mavsdk::Failure::FailureUnit::SensorDistanceSensor)
        .value("SensorAirspeed", mavsdk::Failure::FailureUnit::SensorAirspeed)
        .value("SystemBattery", mavsdk::Failure::FailureUnit::SystemBattery)
        .value("SystemMotor", mavsdk::Failure::FailureUnit::SystemMotor)
        .value("SystemServo", mavsdk::Failure::FailureUnit::SystemServo)
        .value("SystemAvoidance", mavsdk::Failure::FailureUnit::SystemAvoidance)
        .value("SystemRcSignal", mavsdk::Failure::FailureUnit::SystemRcSignal)
        .value("SystemMavlinkSignal", mavsdk::Failure::FailureUnit::SystemMavlinkSignal);

    py::enum_<mavsdk::Failure::FailureType>(failure, "FailureType")
        .value("Ok", mavsdk::Failure::FailureType::Ok)
        .value("Off", mavsdk::Failure::FailureType::Off)
        .value("Stuck", mavsdk::Failure::FailureType::Stuck)
        .value("Garbage", mavsdk::Failure::FailureType::Garbage)
        .value("Wrong", mavsdk::Failure::FailureType::Wrong)
        .value("Slow", mavsdk::Failure::FailureType::Slow)
        .value("Delayed", mavsdk::Failure::FailureType::Delayed)
        .value("Intermittent", mavsdk::Failure::FailureType::Intermittent);

    py::enum_<mavsdk::Failure::Result>(failure, "Result")
        .value("Unknown", mavsdk::Failure::Result::Unknown)
        .value("Success", mavsdk::Failure::Result::Success)
        .value("NoSystem", mavsdk::Failure::Result::NoSystem)
        .value("ConnectionError", mavsdk::Failure::Result::ConnectionError)
        .value("Unsupported", mavsdk::Failure::Result::Unsupported)
        .value("Denied", mavsdk::Failure::Result::Denied)
        .value("Disabled", mavsdk::Failure::Result::Disabled)
        .value("Timeout", mavsdk::Failure::Result::Timeout);

    system.def(
        "failure",
        &mavsdk::System::failure,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}