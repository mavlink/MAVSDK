#include "shell/shell.grpc.pb.h"
#include "plugins/shell/shell.h"

namespace mavsdk {
namespace backend {

template<typename Shell = Shell>
class ShellServiceImpl final : public mavsdk::rpc::shell::ShellService::Service {
public:
    ShellServiceImpl(Shell& shell) : _shell(shell) {}

    grpc::Status Send(
        grpc::ServerContext* /* context */,
        const rpc::shell::SendRequest* rpc_shell_message_request,
        rpc::shell::SendResponse* response) override
    {
        std::promise<void> response_message_received_promise;
        auto response_message_received_future = response_message_received_promise.get_future();

        auto is_finished = std::make_shared<bool>(false);

        mavsdk::Shell::ShellMessage shell_message{};

        shell_message.need_response = rpc_shell_message_request->shell_message().need_response();
        shell_message.timeout = rpc_shell_message_request->shell_message().timeout_ms();
        shell_message.data = rpc_shell_message_request->shell_message().data();

        mavsdk::Shell::Result set_callback_result = _shell.shell_command_response_async(
            [this, &response, &response_message_received_promise, is_finished](
                mavsdk::Shell::Result result, mavsdk::Shell::ShellMessage shell_response) {
                std::lock_guard<std::mutex> lock(_subscribe_mutex);
                if (!*is_finished) {
                    auto rpc_shell_result = get_allocated_shell_result(result);
                    response->set_allocated_shell_result(rpc_shell_result);
                    response->set_response_message_data(shell_response.data);
                    _shell.shell_command_response_async(nullptr);
                    *is_finished = true;
                    response_message_received_promise.set_value();
                }
            });

        if (set_callback_result != mavsdk::Shell::Result::SUCCESS) {
            std::lock_guard<std::mutex> lock(_subscribe_mutex);
            if (!*is_finished) {
                auto rpc_shell_result = get_allocated_shell_result(set_callback_result);
                response->set_allocated_shell_result(rpc_shell_result);
                *is_finished = true;
            }
            return grpc::Status::OK;
        }

        mavsdk::Shell::Result shell_command_result = _shell.shell_command(shell_message);

        if (shell_command_result != mavsdk::Shell::Result::SUCCESS) {
            std::lock_guard<std::mutex> lock(_subscribe_mutex);
            if (!*is_finished) {
                auto rpc_shell_result = get_allocated_shell_result(shell_command_result);
                response->set_allocated_shell_result(rpc_shell_result);
                *is_finished = true;
            }
            return grpc::Status::OK;
        }

        response_message_received_future.wait();

        return grpc::Status::OK;
    }

    rpc::shell::ShellResult* get_allocated_shell_result(mavsdk::Shell::Result result)
    {
        auto rpc_shell_result = new rpc::shell::ShellResult();
        rpc_shell_result->set_result(static_cast<rpc::shell::ShellResult::Result>(result));
        rpc_shell_result->set_result_str(_shell.result_code_str(result));
        return rpc_shell_result;
    }

    void stop() {}

private:
    Shell& _shell;
    std::mutex _subscribe_mutex{};
};

} // namespace backend
} // namespace mavsdk
