#include <future>
#include <string>

#include "core/core.grpc.pb.h"
#include "mavsdk.h"

namespace mavsdk {
namespace mavsdk_server {

template<typename Mavsdk = Mavsdk>
class CoreServiceImpl final : public mavsdk::rpc::core::CoreService::Service {
public:
    CoreServiceImpl(Mavsdk& mavsdk) :
        _mavsdk(mavsdk),
        _stop_promise(std::promise<void>()),
        _stop_future(_stop_promise.get_future())
    {}

    grpc::Status SubscribeConnectionState(
        grpc::ServerContext* /* context */,
        const rpc::core::SubscribeConnectionStateRequest* /* request */,
        grpc::ServerWriter<rpc::core::ConnectionStateResponse>* writer) override
    {
        _mavsdk.subscribe_on_new_system(
            [this, writer]() { publish_system_state(writer, _connection_state_mutex); });

        // Publish the current state on subscribe
        publish_system_state(writer, _connection_state_mutex);

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SetMavlinkTimeout(
        grpc::ServerContext* /* context */,
        const rpc::core::SetMavlinkTimeoutRequest* request,
        rpc::core::SetMavlinkTimeoutResponse* /* response */) override
    {
        if (request != nullptr) {
            _mavsdk.set_timeout_s(request->timeout_s());
        }

        return grpc::Status::OK;
    }

    grpc::Status ListComponents(
        grpc::ServerContext* /* context */,
        const rpc::core::ListComponentsRequest* /* request */,
        rpc::core::ListComponentsResponse* response) override
    {
        auto systems = _mavsdk.systems();
        for (const auto& system : systems) {
            // Create a new SystemComponents message for each system.
            auto* system_components = response->add_systems();

            // Set the system_id for the current system.
            system_components->set_system_id(system->get_system_id());

            // Iterate over component IDs and add them to the current system's component list.
            for (const auto& component_id : system->component_ids()) {
                system_components->add_component_ids(component_id);
            }
        }
        return grpc::Status::OK;
    }

    void stop() { _stop_promise.set_value(); }

private:
    Mavsdk& _mavsdk;
    std::promise<void> _stop_promise;
    std::future<void> _stop_future;
    std::mutex _connection_state_mutex{};

    static mavsdk::rpc::core::ConnectionStateResponse
    createRpcConnectionStateResponse(const bool is_connected)
    {
        mavsdk::rpc::core::ConnectionStateResponse rpc_connection_state_response;

        auto* rpc_connection_state = rpc_connection_state_response.mutable_connection_state();
        rpc_connection_state->set_is_connected(is_connected);

        return rpc_connection_state_response;
    }

    void publish_system_state(
        grpc::ServerWriter<rpc::core::ConnectionStateResponse>* writer,
        std::mutex& connection_state_mutex)
    {
        auto systems = _mavsdk.systems();

        for (auto system : systems) {
            const auto rpc_connection_state_response =
                createRpcConnectionStateResponse(system->is_connected());

            std::lock_guard<std::mutex> lock(connection_state_mutex);
            writer->Write(rpc_connection_state_response);
        }
    }
};

} // namespace mavsdk_server
} // namespace mavsdk
