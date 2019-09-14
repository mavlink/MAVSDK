#include "geofence/geofence.grpc.pb.h"
#include "plugins/geofence/geofence.h"

namespace mavsdk {
namespace backend {

template<typename Geofence = Geofence>
class GeofenceServiceImpl final : public rpc::geofence::GeofenceService::Service {
public:
    GeofenceServiceImpl(Geofence& geofence) : _geofence(geofence) {}

    grpc::Status UploadGeofence(
            grpc::ServerContext*,
            const rpc::geofence::UploadGeofenceRequest* request,
            rpc::geofence::UploadGeofenceResponse* response) override
    {
        std::promise<void> result_promise;
        const auto result_future = result_promise.get_future();

        const auto polygons = extractPolygons(request);
        uploadGeofence(polygons, response, result_promise);

        result_future.wait();
        return grpc::Status::OK;
    }

    static std::shared_ptr<typename Geofence::Polygon>
    translateRPCPolygon(const rpc::geofence::Polygon& rpc_polygon)
    {
        auto polygon = std::make_shared<typename Geofence::Polygon>();
        struct Geofence::Polygon::Point tmp;
        for (auto rpc_point : rpc_polygon.points()) {
            tmp.latitude_deg = rpc_point.latitude_deg();
            tmp.longitude_deg = rpc_point.longitude_deg();
            polygon->points.push_back(tmp);
        }

        switch (rpc_polygon.type()) {
            case rpc::geofence::Polygon::Type::Polygon_Type_INCLUSION:
                polygon->type = Geofence::Polygon::Type::INCLUSION;
                break;
            case rpc::geofence::Polygon::Type::Polygon_Type_EXCLUSION:
                polygon->type = Geofence::Polygon::Type::EXCLUSION;
                break;
        }

        return polygon;
    }

private:
    std::vector<std::shared_ptr<typename Geofence::Polygon>>
    extractPolygons(const rpc::geofence::UploadGeofenceRequest* request) const
    {
        std::vector<std::shared_ptr<typename Geofence::Polygon>> polygons;

        if (request != nullptr) {
            for (auto rpc_polygon : request->polygons()) {
                polygons.push_back(translateRPCPolygon(rpc_polygon));
            }
        }

        return polygons;
    }

    void uploadGeofence(
        const std::vector<std::shared_ptr<typename Geofence::Polygon>>& polygons,
        rpc::geofence::UploadGeofenceResponse* response,
        std::promise<void>& result_promise) const
    {
        _geofence.send_geofence_async(
            polygons, [this, response, &result_promise](const mavsdk::Geofence::Result result) {
                if (response != nullptr) {
                    auto rpc_geofence_result = generateRPCGeofenceResult(result);
                    response->set_allocated_geofence_result(rpc_geofence_result);
                    }

                    result_promise.set_value();
            });
    }

    rpc::geofence::GeofenceResult*
    generateRPCGeofenceResult(const mavsdk::Geofence::Result result) const
    {
        auto rpc_result = static_cast<rpc::geofence::GeofenceResult::Result>(result);
                auto rpc_geofence_result = new rpc::geofence::GeofenceResult();
        rpc_geofence_result->set_result(rpc_result);
        rpc_geofence_result->set_result_str(mavsdk::Geofence::result_str(result));

        return rpc_geofence_result;
    }

    Geofence& _geofence;
};

} // namespace backend
} // namespace mavsdk
