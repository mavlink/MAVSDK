#pragma once

#include <string>

namespace dronecore {

class CliArg {
public:
    enum class Protocol { NONE, UDP, TCP, SERIAL };

    bool parse(const std::string &uri);

    Protocol get_protocol() { return _protocol; }

    int get_port() const { return _port; }

    int get_baudrate() const { return _baudrate; }

    std::string get_path() const { return _path; }

private:
    void reset();
    bool find_protocol(std::string &rest);
    bool find_path(std::string &rest);
    bool find_port(std::string &rest);
    bool find_baudrate(std::string &rest);

    Protocol _protocol{Protocol::NONE};
    std::string _path{};
    int _port{0};
    int _baudrate{0};
};

} // namespace dronecore
