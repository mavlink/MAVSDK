#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp/ftp.h>

namespace py = pybind11;
void init_ftp(py::module_ &m) {
    py::class_<mavsdk::Ftp> ftp(m, "Ftp");


ftp.def(py::init<std::shared_ptr<mavsdk::System>>())




    .def("create_directory", &mavsdk::Ftp::create_directory)
    .def("remove_directory", &mavsdk::Ftp::remove_directory)
    .def("remove_file", &mavsdk::Ftp::remove_file)
    .def("rename", &mavsdk::Ftp::rename)

    .def("set_root_directory", &mavsdk::Ftp::set_root_directory)
    .def("set_target_compid", &mavsdk::Ftp::set_target_compid)

;





py::class_<mavsdk::Ftp::ProgressData> progress_data(ftp, "ProgressData");
progress_data
    .def(py::init<>())
    .def_readwrite("bytes_transferred", &mavsdk::Ftp::ProgressData::bytes_transferred)
    .def_readwrite("total_bytes", &mavsdk::Ftp::ProgressData::total_bytes);





py::enum_<mavsdk::Ftp::Result>(ftp, "Result")
    .value("Unknown", mavsdk::Ftp::Result::Unknown)
    .value("Success", mavsdk::Ftp::Result::Success)
    .value("Next", mavsdk::Ftp::Result::Next)
    .value("Timeout", mavsdk::Ftp::Result::Timeout)
    .value("Busy", mavsdk::Ftp::Result::Busy)
    .value("FileIoError", mavsdk::Ftp::Result::FileIoError)
    .value("FileExists", mavsdk::Ftp::Result::FileExists)
    .value("FileDoesNotExist", mavsdk::Ftp::Result::FileDoesNotExist)
    .value("FileProtected", mavsdk::Ftp::Result::FileProtected)
    .value("InvalidParameter", mavsdk::Ftp::Result::InvalidParameter)
    .value("Unsupported", mavsdk::Ftp::Result::Unsupported)
    .value("ProtocolError", mavsdk::Ftp::Result::ProtocolError);


}