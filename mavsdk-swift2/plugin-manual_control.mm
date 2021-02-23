#import "plugin-manual_control.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/manual_control/manual_control.h>

@implementation MAVSDKManualControl

mavsdk::ManualControl *manualControl;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    manualControl = new mavsdk::ManualControl(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKManualControlResult)startPositionControl {
    return (MAVSDKManualControlResult)manual_control->start_position_control();
}
- (MAVSDKManualControlResult)startAltitudeControl {
    return (MAVSDKManualControlResult)manual_control->start_altitude_control();
}
- (MAVSDKManualControlResult)setManualControlInput:(float)x :(float)y :(float)z :(float)r {
    return (MAVSDKManualControlResult)manual_control->set_manual_control_input(x, y, z, r);
}








@end