#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/info/info.h>

namespace py = pybind11;
void init_info(py::module_ &m) {
    py::class_<mavsdk::Info> info(m, "Info");


info.def(py::init<std::shared_ptr<mavsdk::System>>())





;





py::class_<mavsdk::Info::FlightInfo> flight_info(info, "FlightInfo");
flight_info
    .def(py::init<>())
    .def_readwrite("time_boot_ms", &mavsdk::Info::FlightInfo::time_boot_ms)
    .def_readwrite("flight_uid", &mavsdk::Info::FlightInfo::flight_uid);




py::class_<mavsdk::Info::Identification> identification(info, "Identification");
identification
    .def(py::init<>())
    .def_readwrite("hardware_uid", &mavsdk::Info::Identification::hardware_uid);




py::class_<mavsdk::Info::Product> product(info, "Product");
product
    .def(py::init<>())
    .def_readwrite("vendor_id", &mavsdk::Info::Product::vendor_id)
    .def_readwrite("vendor_name", &mavsdk::Info::Product::vendor_name)
    .def_readwrite("product_id", &mavsdk::Info::Product::product_id)
    .def_readwrite("product_name", &mavsdk::Info::Product::product_name);




py::class_<mavsdk::Info::Version> version(info, "Version");
version
    .def(py::init<>())
    .def_readwrite("flight_sw_major", &mavsdk::Info::Version::flight_sw_major)
    .def_readwrite("flight_sw_minor", &mavsdk::Info::Version::flight_sw_minor)
    .def_readwrite("flight_sw_patch", &mavsdk::Info::Version::flight_sw_patch)
    .def_readwrite("flight_sw_vendor_major", &mavsdk::Info::Version::flight_sw_vendor_major)
    .def_readwrite("flight_sw_vendor_minor", &mavsdk::Info::Version::flight_sw_vendor_minor)
    .def_readwrite("flight_sw_vendor_patch", &mavsdk::Info::Version::flight_sw_vendor_patch)
    .def_readwrite("os_sw_major", &mavsdk::Info::Version::os_sw_major)
    .def_readwrite("os_sw_minor", &mavsdk::Info::Version::os_sw_minor)
    .def_readwrite("os_sw_patch", &mavsdk::Info::Version::os_sw_patch)
    .def_readwrite("flight_sw_git_hash", &mavsdk::Info::Version::flight_sw_git_hash)
    .def_readwrite("os_sw_git_hash", &mavsdk::Info::Version::os_sw_git_hash);





py::enum_<mavsdk::Info::Result>(info, "Result")
    .value("Unknown", mavsdk::Info::Result::Unknown)
    .value("Success", mavsdk::Info::Result::Success)
    .value("InformationNotReceivedYet", mavsdk::Info::Result::InformationNotReceivedYet);


}