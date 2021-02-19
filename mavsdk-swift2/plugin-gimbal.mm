#import "plugin-gimbal.h"

@implementation MVSGimbal

- (MVSResult)setPitchAndYaw:(float)pitchDeg :(float)yawDeg {
    return MVSResultUnknown;
}
- (MVSResult)setPitchRateAndYawRate:(float)pitchRateDegS :(float)yawRateDegS {
    return MVSResultUnknown;
}
- (MVSResult)setMode:(MVSGimbalMode*)gimbalMode {
    return MVSResultUnknown;
}
- (MVSResult)setRoiLocation:(double)latitudeDeg :(double)longitudeDeg :(float)altitudeM {
    return MVSResultUnknown;
}
- (MVSResult)takeControl:(MVSControlMode*)controlMode {
    return MVSResultUnknown;
}
- (MVSResult)releaseControl {
    return MVSResultUnknown;
}















@end