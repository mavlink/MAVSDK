#include <cmath>
#include <limits>
#include <array>

#include "plugins/shell/shell.h"
#include "shell_impl.h"

namespace mavsdk {

Shell::Shell(System& system) : PluginBase(), _impl{new ShellImpl(system)} {}

Shell::~Shell() {}

void Shell::shell_command(Shell::ShellMessage shell_message)
{
    _impl->shell_command(shell_message);
}

const char* Shell::result_code_str(Result::ResultCode result)
{
    switch (result) {
        case Result::ResultCode::SUCCESS:
            return "Success";
        case Result::ResultCode::NO_SYSTEM:
            return "No system";
        case Result::ResultCode::CONNECTION_ERROR:
            return "Connection error";
        case Result::ResultCode::NO_RESPONSE:
            return "Response does not received";
        case Result::ResultCode::DATA_TOO_LONG:
            return "Request Data too long";
        case Result::ResultCode::UNKNOWN:
        default:
            return "Unknown";
    }
}

void Shell::shell_command_response_async(result_callback_t callback)
{
    _impl->shell_command_response_async(callback);
}

bool operator==(const Shell::ShellMessage& lhs, const Shell::ShellMessage& rhs)
{
    return lhs.need_response == rhs.need_response && lhs.timeout == rhs.timeout &&
           lhs.data == rhs.data;
}

std::ostream& operator<<(std::ostream& str, Shell::ShellMessage const& shell_message)
{
    return str << "[need_response: " << shell_message.need_response
               << ", timeout: " << shell_message.timeout << ", data: \"" << shell_message.data
               << "\"";
}

} // namespace mavsdk