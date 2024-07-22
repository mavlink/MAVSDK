#include "cli_arg.h"
#include "log.h"
#include <cctype>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <string>
#include <charconv>

namespace mavsdk {

bool CliArg::parse(const std::string& uri)
{
    // Reset
    protocol = {};

    const std::string udp = "udp";
    const std::string udpin = "udpin";
    const std::string udpout = "udpout";
    const std::string tcp = "tcp";
    const std::string tcpin = "tcpin";
    const std::string tcpout = "tcpout";
    const std::string serial = "serial";
    const std::string serial_flowcontrol = "serial_flowcontrol";
    const std::string delimiter = "://";

    if (uri.find(udp + delimiter) == 0) {
        LogWarn() << "Connection using udp:// is deprecated, please use udpin:// or udpout://";
        return parse_udp(std::string_view(uri).substr(udp.size() + delimiter.size()));
    }

    if (uri.find(udpin + delimiter) == 0) {
        return parse_udpin(std::string_view(uri).substr(udpin.size() + delimiter.size()));
    }

    if (uri.find(udpout + delimiter) == 0) {
        return parse_udpout(std::string_view(uri).substr(udpout.size() + delimiter.size()));
    }

    if (uri.find(tcp + delimiter) == 0) {
        LogWarn() << "Connection using tcp:// is deprecated, please use tcpin:// or tcpout://";
        return parse_tcp(std::string_view(uri).substr(tcp.size() + delimiter.size()));
    }

    if (uri.find(tcpin + delimiter) == 0) {
        return parse_tcpin(std::string_view(uri).substr(tcpin.size() + delimiter.size()));
    }

    if (uri.find(tcpout + delimiter) == 0) {
        return parse_tcpout(std::string_view(uri).substr(tcpout.size() + delimiter.size()));
    }

    if (uri.find(serial + delimiter) == 0) {
        return parse_serial(std::string_view(uri).substr(serial.size() + delimiter.size()), false);
    }

    if (uri.find(serial_flowcontrol + delimiter) == 0) {
        return parse_serial(
            std::string_view(uri).substr(serial_flowcontrol.size() + delimiter.size()), true);
    }

    LogErr() << "Unknown protocol";
    return false;
}

bool CliArg::parse_udp(const std::string_view rest)
{
    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos == std::string::npos) {
        return false;
    }

    protocol = Udp{};
    auto& p = std::get<Udp>(protocol);
    p.host = rest.substr(0, pos);
    if (p.host.empty()) {
        p.host = "0.0.0.0";
        p.mode = Udp::Mode::In;
    } else if (p.host == "0.0.0.0") {
        p.mode = Udp::Mode::In;
    } else {
        p.mode = Udp::Mode::Out;
    }

    if (auto maybe_port = port_from_str(rest.substr(pos + 1))) {
        p.port = maybe_port.value();
        return true;
    } else {
        return false;
    }
}

bool CliArg::parse_udpin(const std::string_view rest)
{
    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos == std::string::npos) {
        return false;
    }

    protocol = Udp{};
    auto& p = std::get<Udp>(protocol);
    p.host = rest.substr(0, pos);
    p.mode = Udp::Mode::In;
    p.host = rest.substr(0, pos);
    p.mode = Udp::Mode::In;

    if (auto maybe_port = port_from_str(rest.substr(pos + 1))) {
        p.port = maybe_port.value();
        return true;
    } else {
        return false;
    }
}

bool CliArg::parse_udpout(const std::string_view rest)
{
    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos == std::string::npos) {
        return false;
    }

    protocol = Udp{};
    auto& p = std::get<Udp>(protocol);
    p.host = rest.substr(0, pos);
    p.mode = Udp::Mode::Out;

    if (p.host == "0.0.0.0") {
        LogErr() << "0.0.0.0 is invalid for UDP out address. "
                    "Can only listen on all interfaces, but not send.";
        return false;
    }

    if (auto maybe_port = port_from_str(rest.substr(pos + 1))) {
        p.port = maybe_port.value();
        return true;
    } else {
        return false;
    }
}

bool CliArg::parse_tcp(const std::string_view rest)
{
    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos == std::string::npos) {
        return false;
    }

    protocol = Tcp{};
    auto& p = std::get<Tcp>(protocol);
    p.host = rest.substr(0, pos);
    if (p.host.empty()) {
        p.host = "0.0.0.0";
        p.mode = Tcp::Mode::In;
    } else if (p.host == "0.0.0.0") {
        p.mode = Tcp::Mode::In;
    } else {
        p.mode = Tcp::Mode::Out;
    }

    if (auto maybe_port = port_from_str(rest.substr(pos + 1))) {
        p.port = maybe_port.value();
        return true;
    } else {
        return false;
    }
}

bool CliArg::parse_tcpin(const std::string_view rest)
{
    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos == std::string::npos) {
        return false;
    }

    protocol = Tcp{};
    auto& p = std::get<Tcp>(protocol);
    p.host = rest.substr(0, pos);
    p.mode = Tcp::Mode::In;

    if (auto maybe_port = port_from_str(rest.substr(pos + 1))) {
        p.port = maybe_port.value();
        return true;
    } else {
        return false;
    }
}

bool CliArg::parse_tcpout(const std::string_view rest)
{
    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos == std::string::npos) {
        return false;
    }

    protocol = Tcp{};
    auto& p = std::get<Tcp>(protocol);
    p.host = rest.substr(0, pos);
    p.mode = Tcp::Mode::Out;

    if (p.host == "0.0.0.0") {
        LogErr() << "0.0.0.0 is invalid for TCP out address. "
                    "Can only listen on all interfaces, but not send.";
        return false;
    }

    if (auto maybe_port = port_from_str(rest.substr(pos + 1))) {
        p.port = maybe_port.value();
        return true;
    } else {
        return false;
    }
}

std::optional<int> CliArg::port_from_str(std::string_view str)
{
    int value;
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

    if (static_cast<bool>(ec) || value < 1 || value > 65535) {
        return {};
    }
    return {value};
}

bool CliArg::parse_serial(const std::string_view rest, bool flow_control_enabled)
{
    protocol = Serial{};
    auto& p = std::get<Serial>(protocol);
    p.flow_control_enabled = flow_control_enabled;

    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos == std::string::npos) {
        return false;
    }

    p.path = rest.substr(0, pos);

    const auto path_is_only_numbers =
        std::all_of(p.path.begin(), p.path.end(), [](unsigned char c) { return std::isdigit(c); });

    if (path_is_only_numbers) {
        LogErr() << "Path can't be numbers only.";
        return false;
    }

    if (p.path.find('/') == 0) {
        // A Linux/macOS path needs to start with '/'.
    } else if (p.path.find("COM") == 0) {
        // On Windows a path starting with 'COM' is ok but needs to be followed by digits.
        for (const auto& digit : p.path.substr(3, p.path.length() - 3)) {
            if (!std::isdigit(digit)) {
                LogErr() << "COM port number invalid.";
                return false;
            }
        }

        if (p.path.length() == 3) {
            LogErr() << "COM port number missing";
            return false;
        }
    } else {
        LogErr() << "serial port needs to start with / or COM on Windows";
        return false;
    }

    auto baudrate_str = rest.substr(pos + 1);

    int value;
    auto [ptr, ec] =
        std::from_chars(baudrate_str.data(), baudrate_str.data() + baudrate_str.size(), value);

    if (static_cast<bool>(ec)) {
        return {};
    }

    if (value < 0) {
        LogErr() << "Baudrate can't be negative.";
        return false;
    }

    p.baudrate = value;

    return true;
}

} // namespace mavsdk
