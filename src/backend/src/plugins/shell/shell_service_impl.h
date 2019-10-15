#include <cmath>
#include <array>
#include <limits>
#include "shell/shell.grpc.pb.h"
#include "plugins/shell/shell.h"

namespace mavsdk {
namespace backend {

template<typename Shell = Shell>
class ShellServiceImpl final : public mavsdk::rpc::shell::ShellService::Service {
public:
    ShellServiceImpl(Shell& shell) : _shell(shell) {}

    grpc::Status SetShellMessage(
        grpc::ServerContext* /* context */,
        const rpc::shell::SetShellMessageRequest* rpc_shell_message_request,
        rpc::shell::SetShellMessageResponse* response) override
    {
        std::promise<void> response_message_received_promise;
        auto response_message_received_future = response_message_received_promise.get_future();

        auto is_finished = std::make_shared<bool>(false);

        mavsdk::Shell::ShellMessage shell_message{};

        shell_message.need_response = rpc_shell_message_request->shell_message().need_response();
        shell_message.timeout = rpc_shell_message_request->shell_message().timeout_ms();
        shell_message.data = rpc_shell_message_request->shell_message().data();

        _shell.shell_command_response_async(
            [this, &response, &response_message_received_promise, is_finished](
                mavsdk::Shell::Result result) {
                auto rpc_shell_result = new rpc::shell::ShellResult();
                rpc_shell_result->set_result(
                    static_cast<rpc::shell::ShellResult::Result>(result.result_code));
                rpc_shell_result->set_result_str(_shell.result_code_str(result.result_code));

                rpc_shell_result->set_response_data(result.response.data);

                std::lock_guard<std::mutex> lock(_subscribe_mutex);
                if (!*is_finished) {
                    response->set_allocated_shell_result(rpc_shell_result);
                    _shell.shell_command_response_async(nullptr);
                    *is_finished = true;
                    response_message_received_promise.set_value();
                }
            });

        _shell.shell_command(shell_message);

        response_message_received_future.wait();

        return grpc::Status::OK;
    }

    void stop() {}

private:
    Shell& _shell;
    std::mutex _subscribe_mutex{};
};

} // namespace backend
} // namespace mavsdk
