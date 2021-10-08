#pragma once

#include <string>

namespace mavsdk {

class CliArg {
public:
    enum class Protocol { None, Udp, Tcp, Serial };

    bool parse(const std::string& uri);

    [[nodiscard]] Protocol get_protocol() const { return _protocol; }

    [[nodiscard]] int get_port() const { return _port; }

    [[nodiscard]] int get_baudrate() const { return _baudrate; }

    [[nodiscard]] bool get_flow_control() const { return _flow_control_enabled; }

    [[nodiscard]] std::string get_path() const { return _path; }

private:
    void reset();
    bool find_protocol(std::string& rest);
    bool find_path(std::string& rest);
    bool find_port(std::string& rest);
    bool find_baudrate(std::string& rest);

    Protocol _protocol{Protocol::None};
    std::string _path{};
    int _port{0};
    int _baudrate{0};
    bool _flow_control_enabled{false};
};

} // namespace mavsdk
