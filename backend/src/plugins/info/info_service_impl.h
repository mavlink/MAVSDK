#include "plugins/info/info.h"
#include "info/info.grpc.pb.h"

namespace dronecode_sdk {
namespace backend {

template<typename Info = Info>
class InfoServiceImpl final : public rpc::info::InfoService::Service {
public:
    InfoServiceImpl(Info &info) : _info(info) {}

    grpc::Status GetVersion(grpc::ServerContext * /* context */,
                            const rpc::info::GetVersionRequest * /* request */,
                            rpc::info::GetVersionResponse *response) override
    {
        if (response != nullptr) {
            auto result_pair = _info.get_version();

            auto *rpc_info_result = new rpc::info::InfoResult();
            rpc_info_result->set_result(
                static_cast<rpc::info::InfoResult::Result>(result_pair.first));
            rpc_info_result->set_result_str(dronecode_sdk::Info::result_str(result_pair.first));

            auto *rpc_version = new rpc::info::Version();
            rpc_version->set_flight_sw_major(result_pair.second.flight_sw_major);
            rpc_version->set_flight_sw_minor(result_pair.second.flight_sw_minor);
            rpc_version->set_flight_sw_patch(result_pair.second.flight_sw_patch);
            rpc_version->set_flight_sw_vendor_major(result_pair.second.flight_sw_vendor_major);
            rpc_version->set_flight_sw_vendor_minor(result_pair.second.flight_sw_vendor_minor);
            rpc_version->set_flight_sw_vendor_patch(result_pair.second.flight_sw_vendor_patch);
            rpc_version->set_os_sw_major(result_pair.second.os_sw_major);
            rpc_version->set_os_sw_minor(result_pair.second.os_sw_minor);
            rpc_version->set_os_sw_patch(result_pair.second.os_sw_patch);

            response->set_allocated_info_result(rpc_info_result);
            response->set_allocated_version(rpc_version);
        }

        return grpc::Status::OK;
    }

private:
    Info &_info;
};

} // namespace backend
} // namespace dronecode_sdk
