#include "geofence/geofence.grpc.pb.h"
#include "plugins/geofence/geofence.h"

namespace mavsdk {
namespace backend {

template<typename Geofence = Geofence>
class GeofenceServiceImpl final : public rpc::geofence::GeofenceService::Service {
public:
    GeofenceServiceImpl(Geofence& geofence) : _geofence(geofence) {}

    grpc::Status UploadGeofence(
        grpc::ServerContext* /*  context */,
        const rpc::geofence::UploadGeofenceRequest* request,
        rpc::geofence::UploadGeofenceResponse* response) override
    {
        const auto requested_geofence_lat = request->latitude_deg();
        const auto requested_geofence_lon = request->longitude_deg();

        std::promise<void> result_promise;
        const auto result_future = result_promise.get_future();

        const auto polygons = extractPolygons(request);
        uploadGeofence(polygons, response, result_promise);

        result_future.wait();
        return grpc::Status::OK;
    }

    static std::shared_ptr<Polygon>
    translateRPCPolygon(const rpc::geofence::Polygon& rpc_polygon)
    {
        auto polygon = std::make_shared<Polygon>();
        struct Polygon::Point tmp;
        for (auto rpc_point : rpc_polygon.points()) {
            tmp.latitude_deg = rpc_point.latitude_deg();
            tmp.longitude_deg = rpc_point.longitude_deg();
            polygon->points.push_back(tmp);
        }

        switch (rpc_polygon.type()) {
            case rpc::geofence::Polygon::Type::Polygon_Type_INCLUSION:
                polygon->type = Polygon::Type::INCLUSION;
                break;
            case rpc::geofence::Polygon::Type::Polygon_Type_EXCLUSION:
                polygon->type = Polygon::Type::EXCLUSION;
                break;
        }

        return polygon;
    }

private:
    std::vector<std::shared_ptr<Polygon>>
    extractPolygons(const rpc::geofence::UploadGeofenceRequest* request) const
    {
        std::vector<std::shared_ptr<MissionItem>> polygons;

        if (request != nullptr) {
            for (auto rpc_polygon : request->polygons()) {
                polygons.push_back(translateRPCPolygon(rpc_polygon));
            }
        }

        return polygons;
    }


    void uploadGeofence(
        const std::vector<std::shared_ptr<Polygon>>& polygons,
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
}
