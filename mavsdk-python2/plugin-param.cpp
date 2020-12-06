#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>

namespace py = pybind11;
void init_param(py::module_ &m) {
    py::class_<mavsdk::Param> param(m, "Param");


param.def(py::init<std::shared_ptr<mavsdk::System>>())

    .def("set_param_int", &mavsdk::Param::set_param_int)

    .def("set_param_float", &mavsdk::Param::set_param_float)

;





py::class_<mavsdk::Param::IntParam> int_param(param, "IntParam");
int_param
    .def(py::init<>())
    .def_readwrite("name", &mavsdk::Param::IntParam::name)
    .def_readwrite("value", &mavsdk::Param::IntParam::value);




py::class_<mavsdk::Param::FloatParam> float_param(param, "FloatParam");
float_param
    .def(py::init<>())
    .def_readwrite("name", &mavsdk::Param::FloatParam::name)
    .def_readwrite("value", &mavsdk::Param::FloatParam::value);




py::class_<mavsdk::Param::AllParams> all_params(param, "AllParams");
all_params
    .def(py::init<>())
    .def_readwrite("int_params", &mavsdk::Param::AllParams::int_params)
    .def_readwrite("float_params", &mavsdk::Param::AllParams::float_params);





py::enum_<mavsdk::Param::Result>(param, "Result")
    .value("Unknown", mavsdk::Param::Result::Unknown)
    .value("Success", mavsdk::Param::Result::Success)
    .value("Timeout", mavsdk::Param::Result::Timeout)
    .value("ConnectionError", mavsdk::Param::Result::ConnectionError)
    .value("WrongType", mavsdk::Param::Result::WrongType)
    .value("ParamNameTooLong", mavsdk::Param::Result::ParamNameTooLong);


}