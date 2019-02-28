#include "plugins/params_raw/params_raw.h"
#include "params/params.grpc.pb.h"

namespace dronecode_sdk {
namespace backend {

template<typename Params = Params>
class ParamsServiceImpl final : public rpc::params:ParamsService:Service {
public:
    ParamsServiceImpl(Params &params) : _params(params) {}

    grpc::Status GetIntParams(grpc::ServerContext * /* context */,
                            const rpc::params::GetIntParamsRequest *request,
                            rpc::params::GetIntParamsResponse *response) override
    {
        if (request != nullptr) {
            const auto requested_params = request->name();

            if (response != nullptr) {
                auto result_pair = _params.get_param_int(requested_params);

                auto *rpc_params_result = new rpc::params::ParamsResult();

                rcp_params_result->set_result(
                    static_cast<rpc::params::ParamsResult::Result>(result_pair.first));
                    rpc_params_result->set_result_str(dronecode_sdk::Params::result_str(result_pair.first));

                    response->set_allocated_params_result(rpc_params_result);
                    response->set_value(result_pair.second);
                )
            }
        }

        return grpc::Status::OK;
    }

private:
    Params &_params;
};

} // namespace backend
} // namespace dronecode_sdk