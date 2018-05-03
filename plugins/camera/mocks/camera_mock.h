#include <gmock/gmock.h>

#include "camera/camera.h"

namespace dronecore {
namespace testing {

class MockCamera
{
public:
    MOCK_CONST_METHOD0(take_photo, Camera::Result());
};

} // namespace testing
} // namespace dronecore
