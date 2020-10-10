#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/tune/tune.h>
#include <iostream>
#include <thread>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}

int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    std::string connection_url;
    ConnectionResult connection_result;

    bool discovered_system = false;
    if (argc == 2) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: " << connection_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    auto system = mavsdk.systems().at(0);

    std::cout << "Waiting to discover system..." << std::endl;
    mavsdk.subscribe_on_change([&mavsdk, &discovered_system]() {
        const auto system = mavsdk.systems().at(0);

        if (system->is_connected()) {
            std::cout << "Discovered system" << std::endl;
            discovered_system = true;
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a system after around 2
    // seconds.
    sleep_for(seconds(2));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

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

    Tune tune(system);
    const int tempo = 200;

    Tune::TuneDescription tune_description;
    tune_description.song_elements = song_elements;
    tune_description.tempo = tempo;

    tune.play_tune_async(tune_description, [](const Tune::Result result) {
        std::cout << NORMAL_CONSOLE_TEXT << "Tune sent with result: " << result << std::endl;
    });

    return 0;
}
