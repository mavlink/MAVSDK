#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace mavsdk {

class CliArg {
public:
    struct Udp {
        enum class Mode {
            Unknown,
            In,
            Out,
        };
        Mode mode{Mode::Unknown};
        std::string host{};
        int port{};
    };

    struct Tcp {
        enum class Mode {
            Unknown,
            In,
            Out,
        };
        Mode mode{Mode::Unknown};
        std::string host{};
        int port{};
    };

    struct Serial {
        std::string path{};
        int baudrate{};
        bool flow_control_enabled{false};
    };

    using Protocol = std::variant<std::monostate, Udp, Tcp, Serial>;

    bool parse(const std::string& uri);

    Protocol protocol{};

private:
    static std::optional<int> port_from_str(std::string_view str);

    bool parse_udp(const std::string_view rest);
    bool parse_udpin(const std::string_view rest);
    bool parse_udpout(const std::string_view rest);

    bool parse_tcp(const std::string_view rest);
    bool parse_tcpin(const std::string_view rest);
    bool parse_tcpout(const std::string_view rest);

    bool parse_serial(const std::string_view rest, bool flow_control_enabled);
};

} // namespace mavsdk
