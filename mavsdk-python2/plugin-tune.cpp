#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/tune/tune.h>

namespace py = pybind11;
void init_tune(py::module_ &m) {
    py::class_<mavsdk::Tune> tune(m, "Tune");


tune.def(py::init<std::shared_ptr<mavsdk::System>>())
    .def("play_tune", &mavsdk::Tune::play_tune)
;


    py::enum_<mavsdk::Tune::SongElement>(tune, "SongElement")
        .value("StyleLegato", mavsdk::Tune::SongElement::StyleLegato)
        .value("StyleNormal", mavsdk::Tune::SongElement::StyleNormal)
        .value("StyleStaccato", mavsdk::Tune::SongElement::StyleStaccato)
        .value("Duration1", mavsdk::Tune::SongElement::Duration1)
        .value("Duration2", mavsdk::Tune::SongElement::Duration2)
        .value("Duration4", mavsdk::Tune::SongElement::Duration4)
        .value("Duration8", mavsdk::Tune::SongElement::Duration8)
        .value("Duration16", mavsdk::Tune::SongElement::Duration16)
        .value("Duration32", mavsdk::Tune::SongElement::Duration32)
        .value("NoteA", mavsdk::Tune::SongElement::NoteA)
        .value("NoteB", mavsdk::Tune::SongElement::NoteB)
        .value("NoteC", mavsdk::Tune::SongElement::NoteC)
        .value("NoteD", mavsdk::Tune::SongElement::NoteD)
        .value("NoteE", mavsdk::Tune::SongElement::NoteE)
        .value("NoteF", mavsdk::Tune::SongElement::NoteF)
        .value("NoteG", mavsdk::Tune::SongElement::NoteG)
        .value("NotePause", mavsdk::Tune::SongElement::NotePause)
        .value("Sharp", mavsdk::Tune::SongElement::Sharp)
        .value("Flat", mavsdk::Tune::SongElement::Flat)
        .value("OctaveUp", mavsdk::Tune::SongElement::OctaveUp)
        .value("OctaveDown", mavsdk::Tune::SongElement::OctaveDown);




py::class_<mavsdk::Tune::TuneDescription> tune_description(tune, "TuneDescription");
tune_description
    .def(py::init<>())
    .def_readwrite("song_elements", &mavsdk::Tune::TuneDescription::song_elements)
    .def_readwrite("tempo", &mavsdk::Tune::TuneDescription::tempo);





py::enum_<mavsdk::Tune::Result>(tune, "Result")
    .value("Unknown", mavsdk::Tune::Result::Unknown)
    .value("Success", mavsdk::Tune::Result::Success)
    .value("InvalidTempo", mavsdk::Tune::Result::InvalidTempo)
    .value("TuneTooLong", mavsdk::Tune::Result::TuneTooLong)
    .value("Error", mavsdk::Tune::Result::Error);


}