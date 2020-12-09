#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/follow_me/follow_me.h>

namespace py = pybind11;
void init_follow_me(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::FollowMe> follow_me(m, "FollowMe");

    follow_me

        .def("set_config", &mavsdk::FollowMe::set_config)

        .def("set_target_location", &mavsdk::FollowMe::set_target_location)

        .def("start", &mavsdk::FollowMe::start)
        .def("stop", &mavsdk::FollowMe::stop);

    py::class_<mavsdk::FollowMe::Config> config(follow_me, "Config");
    config.def(py::init<>())
        .def_readwrite("min_height_m", &mavsdk::FollowMe::Config::min_height_m)
        .def_readwrite("follow_distance_m", &mavsdk::FollowMe::Config::follow_distance_m)
        .def_readwrite("follow_direction", &mavsdk::FollowMe::Config::follow_direction)
        .def_readwrite("responsiveness", &mavsdk::FollowMe::Config::responsiveness);

    py::enum_<mavsdk::FollowMe::Config::FollowDirection>(config, "FollowDirection")
        .value("None", mavsdk::FollowMe::Config::FollowDirection::None)
        .value("Behind", mavsdk::FollowMe::Config::FollowDirection::Behind)
        .value("Front", mavsdk::FollowMe::Config::FollowDirection::Front)
        .value("FrontRight", mavsdk::FollowMe::Config::FollowDirection::FrontRight)
        .value("FrontLeft", mavsdk::FollowMe::Config::FollowDirection::FrontLeft);

    py::class_<mavsdk::FollowMe::TargetLocation> target_location(follow_me, "TargetLocation");
    target_location.def(py::init<>())
        .def_readwrite("latitude_deg", &mavsdk::FollowMe::TargetLocation::latitude_deg)
        .def_readwrite("longitude_deg", &mavsdk::FollowMe::TargetLocation::longitude_deg)
        .def_readwrite(
            "absolute_altitude_m", &mavsdk::FollowMe::TargetLocation::absolute_altitude_m)
        .def_readwrite("velocity_x_m_s", &mavsdk::FollowMe::TargetLocation::velocity_x_m_s)
        .def_readwrite("velocity_y_m_s", &mavsdk::FollowMe::TargetLocation::velocity_y_m_s)
        .def_readwrite("velocity_z_m_s", &mavsdk::FollowMe::TargetLocation::velocity_z_m_s);

    py::enum_<mavsdk::FollowMe::Result>(follow_me, "Result")
        .value("Unknown", mavsdk::FollowMe::Result::Unknown)
        .value("Success", mavsdk::FollowMe::Result::Success)
        .value("NoSystem", mavsdk::FollowMe::Result::NoSystem)
        .value("ConnectionError", mavsdk::FollowMe::Result::ConnectionError)
        .value("Busy", mavsdk::FollowMe::Result::Busy)
        .value("CommandDenied", mavsdk::FollowMe::Result::CommandDenied)
        .value("Timeout", mavsdk::FollowMe::Result::Timeout)
        .value("NotActive", mavsdk::FollowMe::Result::NotActive)
        .value("SetConfigFailed", mavsdk::FollowMe::Result::SetConfigFailed);

    system.def(
        "follow_me",
        &mavsdk::System::follow_me,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}