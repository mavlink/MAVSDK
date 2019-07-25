#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <vector>

#include "geofence/geofence_service_impl.h"
#include "geofence/mocks/geofence_mock.h"

namespace {

namespace dc = mavsdk;
namespace rpc = mavsdk::rpc::mission;

using testing::_;
using testing::DoDefault;
using testing::NiceMock;
using testing::Return;

using MockGeofence = NiceMock<dc::testing::MockGeofence>;
using MockGeofenceImpl = dc::backend::GeofenceServiceImpl<MockGeofence>;

using UploadGeofenceRequest = dc::rpc::geofence::UploadGeofenceRequest;
using UploadGeofenceResponse = dc::rpc::geofence::UploadGeofenceResponse;

std::vector<std::shared_ptr<dc::Polygon>> generateListOfOnePolygon();

}
