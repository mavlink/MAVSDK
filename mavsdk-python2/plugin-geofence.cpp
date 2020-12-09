#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/geofence/geofence.h>

namespace py = pybind11;
void init_geofence(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::Geofence> geofence(m, "Geofence");

    geofence.def("upload_geofence", &mavsdk::Geofence::upload_geofence);

    py::class_<mavsdk::Geofence::Point> point(geofence, "Point");
    point.def(py::init<>())
        .def_readwrite("latitude_deg", &mavsdk::Geofence::Point::latitude_deg)
        .def_readwrite("longitude_deg", &mavsdk::Geofence::Point::longitude_deg);

    py::class_<mavsdk::Geofence::Polygon> polygon(geofence, "Polygon");
    polygon.def(py::init<>())
        .def_readwrite("points", &mavsdk::Geofence::Polygon::points)
        .def_readwrite("fence_type", &mavsdk::Geofence::Polygon::fence_type);

    py::enum_<mavsdk::Geofence::Polygon::FenceType>(polygon, "FenceType")
        .value("Inclusion", mavsdk::Geofence::Polygon::FenceType::Inclusion)
        .value("Exclusion", mavsdk::Geofence::Polygon::FenceType::Exclusion);

    py::enum_<mavsdk::Geofence::Result>(geofence, "Result")
        .value("Unknown", mavsdk::Geofence::Result::Unknown)
        .value("Success", mavsdk::Geofence::Result::Success)
        .value("Error", mavsdk::Geofence::Result::Error)
        .value("TooManyGeofenceItems", mavsdk::Geofence::Result::TooManyGeofenceItems)
        .value("Busy", mavsdk::Geofence::Result::Busy)
        .value("Timeout", mavsdk::Geofence::Result::Timeout)
        .value("InvalidArgument", mavsdk::Geofence::Result::InvalidArgument);

    system.def(
        "geofence",
        &mavsdk::System::geofence,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}