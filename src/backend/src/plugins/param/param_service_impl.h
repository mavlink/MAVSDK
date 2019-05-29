#include "param/param.grpc.pb.h"
#include "plugins/param/param.h"

namespace dronecode_sdk {
namespace backend {

template<typename Param = Param>
class ParamServiceImpl final : public rpc::param::ParamService::Service {
public:
    ParamServiceImpl(Param &param) : _param(param) {}

    grpc::Status GetIntParam(grpc::ServerContext * /* context */,
                             const rpc::param::GetIntParamRequest *request,
                             rpc::param::GetIntParamResponse *response) override
    {
        if (request != nullptr) {
            const auto requested_param = request->name();

            if (response != nullptr) {
                auto result_pair = _param.get_param_int(requested_param);

                auto *rpc_param_result = new rpc::param::ParamResult();
                rpc_param_result->set_result(
                    static_cast<rpc::param::ParamResult::Result>(result_pair.first));
                rpc_param_result->set_result_str(
                    dronecode_sdk::Param::result_str(result_pair.first));

                response->set_allocated_param_result(rpc_param_result);
                response->set_value(result_pair.second);
            }
        }

        return grpc::Status::OK;
    }

    grpc::Status SetIntParam(grpc::ServerContext * /* context */,
                             const rpc::param::SetIntParamRequest *request,
                             rpc::param::SetIntParamResponse *response) override
    {
        if (request != nullptr) {
            const auto requested_param_name = request->name();
            const auto requested_param_value = request->value();

            const auto param_result =
                _param.set_param_int(requested_param_name, requested_param_value);

            if (response != nullptr) {
                auto *rpc_param_result = new rpc::param::ParamResult();
                rpc_param_result->set_result(
                    static_cast<rpc::param::ParamResult::Result>(param_result));
                rpc_param_result->set_result_str(dronecode_sdk::Param::result_str(param_result));

                response->set_allocated_param_result(rpc_param_result);
            }
        }

        return grpc::Status::OK;
    }

    grpc::Status GetFloatParam(grpc::ServerContext * /* context */,
                               const rpc::param::GetFloatParamRequest *request,
                               rpc::param::GetFloatParamResponse *response) override
    {
        if (request != nullptr) {
            const auto requested_param = request->name();

            if (response != nullptr) {
                auto result_pair = _param.get_param_float(requested_param);

                auto *rpc_param_result = new rpc::param::ParamResult();
                rpc_param_result->set_result(
                    static_cast<rpc::param::ParamResult::Result>(result_pair.first));
                rpc_param_result->set_result_str(
                    dronecode_sdk::Param::result_str(result_pair.first));

                response->set_allocated_param_result(rpc_param_result);
                response->set_value(result_pair.second);
            }
        }

        return grpc::Status::OK;
    }

    grpc::Status SetFloatParam(grpc::ServerContext * /* context */,
                               const rpc::param::SetFloatParamRequest *request,
                               rpc::param::SetFloatParamResponse *response) override
    {
        if (request != nullptr) {
            const auto requested_param_name = request->name();
            const auto requested_param_value = request->value();

            const auto param_result =
                _param.set_param_float(requested_param_name, requested_param_value);

            if (response != nullptr) {
                auto *rpc_param_result = new rpc::param::ParamResult();
                rpc_param_result->set_result(
                    static_cast<rpc::param::ParamResult::Result>(param_result));
                rpc_param_result->set_result_str(dronecode_sdk::Param::result_str(param_result));

                response->set_allocated_param_result(rpc_param_result);
            }
        }

        return grpc::Status::OK;
    }

private:
    Param &_param;
};

} // namespace backend
} // namespace dronecode_sdk
