//
// Example how to customize MAVSDK log messages.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/log_callback.h>

#include <iostream>

using namespace mavsdk;

int main(int, char**)
{
    // Set our own custom log function.
    log::subscribe(
        [](log::Level level, const std::string& message, const std::string& file, int line) {
            std::cout << "My custom print function: " << message << std::endl;

            // Prevent unused warnings.
            (void)level;
            (void)file;
            (void)line;
            return true;
        });

    // Instantiate Mavsdk after. This will just print the version.
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    return 0;
}
