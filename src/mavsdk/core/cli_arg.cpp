#include "cli_arg.h"
#include "log.h"
#include <cctype>
#include <algorithm>
#include <limits>
#include <cstdint>

namespace mavsdk {

void CliArg::reset()
{
    _protocol = Protocol::None;
    _path.clear();
    _baudrate = 0;
    _port = 0;
}

bool CliArg::parse(const std::string& uri)
{
    reset();

    std::string rest(uri);
    if (!find_protocol(rest)) {
        return false;
    }

    if (!find_path(rest)) {
        return false;
    }

    if (_protocol == Protocol::Serial) {
        if (!find_baudrate(rest)) {
            return false;
        }
    } else {
        if (!find_port(rest)) {
            return false;
        }
    }

    return true;
}

bool CliArg::find_protocol(std::string& rest)
{
    const std::string udp = "udp";
    const std::string tcp = "tcp";
    const std::string serial = "serial";
    const std::string serial_flowcontrol = "serial_flowcontrol";
    const std::string delimiter = "://";

    if (rest.find(udp + delimiter) == 0) {
        _protocol = Protocol::Udp;
        rest.erase(0, udp.length() + delimiter.length());
        return true;
    } else if (rest.find(tcp + delimiter) == 0) {
        _protocol = Protocol::Tcp;
        rest.erase(0, tcp.length() + delimiter.length());
        return true;
    } else if (rest.find(serial + delimiter) == 0) {
        _protocol = Protocol::Serial;
        _flow_control_enabled = false;
        rest.erase(0, serial.length() + delimiter.length());
        return true;
    } else if (rest.find(serial_flowcontrol + delimiter) == 0) {
        _protocol = Protocol::Serial;
        _flow_control_enabled = true;
        rest.erase(0, serial_flowcontrol.length() + delimiter.length());
        return true;
    } else {
        LogWarn() << "Unknown protocol";
        return false;
    }
}

bool CliArg::find_path(std::string& rest)
{
    if (rest.length() == 0) {
        if (_protocol == Protocol::Udp || _protocol == Protocol::Tcp) {
            // We have to use the default path
            return true;
        } else {
            LogWarn() << "Path for serial device required.";
            return false;
        }
    }

    const std::string delimiter = ":";
    size_t pos = rest.find(delimiter);
    if (pos != std::string::npos) {
        _path = rest.substr(0, pos);
        rest.erase(0, pos + delimiter.length());
    } else {
        const auto path_is_only_numbers = std::all_of(
            rest.cbegin(), rest.cend(), [](unsigned char c) { return std::isdigit(c); });

        if (path_is_only_numbers) {
            LogWarn() << "Path can't be numbers only.";
            return false;
        } else {
            _path = rest;
            rest = "";
        }
    }

    if (_protocol == Protocol::Serial) {
        if (_path.find('/') == 0) {
            // A Linux/macOS path starting with '/' is ok.
            return true;
        } else if (_path.find("COM") == 0) {
            // On Windows a path starting with 'COM' is ok but needs to be followed by digits.
            if (_path.length() == 3) {
                LogWarn() << "COM port number missing";
                return false;
            }
            for (const auto& digit : _path.substr(3, _path.length() - 3)) {
                if (!std::isdigit(digit)) {
                    LogWarn() << "COM port number invalid.";
                    _path = "";
                    return false;
                }
            }
        } else {
            LogWarn() << "Invalid serial path";
            _path = "";
            return false;
        }
    }

    return true;
}

bool CliArg::find_port(std::string& rest)
{
    if (rest.length() == 0) {
        _port = 0;
        return true;
    }

    for (const auto& digit : rest) {
        if (!std::isdigit(digit)) {
            LogWarn() << "Non-numeric char found in port";
            return false;
        }
    }
    _port = std::stoi(rest);
    if (_port < 0) {
        LogWarn() << "Port can't be negative.";
        _port = 0;
        return false;
    } else if (_port > std::numeric_limits<uint16_t>::max()) {
        LogWarn() << "Port number to big.";
        _port = 0;
        return false;
    }
    return true;
}

bool CliArg::find_baudrate(std::string& rest)
{
    if (rest.length() == 0) {
        _port = 0;
        return true;
    }

    for (const auto& digit : rest) {
        if (!std::isdigit(digit)) {
            LogWarn() << "Non-numeric char found in baudrate";
            return false;
        }
    }
    _baudrate = std::stoi(rest);
    return true;
}

} // namespace mavsdk
