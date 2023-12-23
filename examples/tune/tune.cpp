//
// Example how to play a tune using MAVSDK.
//

#include <cstdint>
#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/tune/tune.h>
#include <iostream>
#include <thread>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    // Instantiate plugin.
    Tune tune(system.value());

    std::vector<Tune::SongElement> song_elements;
    song_elements.push_back(Tune::SongElement::Duration4);
    song_elements.push_back(Tune::SongElement::NoteG);
    song_elements.push_back(Tune::SongElement::NoteA);
    song_elements.push_back(Tune::SongElement::NoteB);
    song_elements.push_back(Tune::SongElement::Flat);
    song_elements.push_back(Tune::SongElement::OctaveUp);
    song_elements.push_back(Tune::SongElement::Duration1);
    song_elements.push_back(Tune::SongElement::NoteE);
    song_elements.push_back(Tune::SongElement::Flat);
    song_elements.push_back(Tune::SongElement::OctaveDown);
    song_elements.push_back(Tune::SongElement::Duration4);
    song_elements.push_back(Tune::SongElement::NotePause);
    song_elements.push_back(Tune::SongElement::NoteF);
    song_elements.push_back(Tune::SongElement::NoteG);
    song_elements.push_back(Tune::SongElement::NoteA);
    song_elements.push_back(Tune::SongElement::OctaveUp);
    song_elements.push_back(Tune::SongElement::Duration2);
    song_elements.push_back(Tune::SongElement::NoteD);
    song_elements.push_back(Tune::SongElement::NoteD);
    song_elements.push_back(Tune::SongElement::OctaveDown);
    song_elements.push_back(Tune::SongElement::Duration4);
    song_elements.push_back(Tune::SongElement::NotePause);
    song_elements.push_back(Tune::SongElement::NoteE);
    song_elements.push_back(Tune::SongElement::Flat);
    song_elements.push_back(Tune::SongElement::NoteF);
    song_elements.push_back(Tune::SongElement::NoteG);
    song_elements.push_back(Tune::SongElement::OctaveUp);
    song_elements.push_back(Tune::SongElement::Duration1);
    song_elements.push_back(Tune::SongElement::NoteC);
    song_elements.push_back(Tune::SongElement::OctaveDown);
    song_elements.push_back(Tune::SongElement::Duration4);
    song_elements.push_back(Tune::SongElement::NotePause);
    song_elements.push_back(Tune::SongElement::NoteA);
    song_elements.push_back(Tune::SongElement::OctaveUp);
    song_elements.push_back(Tune::SongElement::NoteC);
    song_elements.push_back(Tune::SongElement::OctaveDown);
    song_elements.push_back(Tune::SongElement::NoteB);
    song_elements.push_back(Tune::SongElement::Flat);
    song_elements.push_back(Tune::SongElement::Duration2);
    song_elements.push_back(Tune::SongElement::NoteG);

    const int tempo = 200;

    Tune::TuneDescription tune_description;
    tune_description.song_elements = song_elements;
    tune_description.tempo = tempo;

    const auto result = tune.play_tune(tune_description);
    if (result != Tune::Result::Success) {
        std::cerr << "Tune result: " << result << '\n';
        return 1;
    }

    return 0;
}
