#import "plugin-action.h"

@implementation MVSAction

- (MVSResult)arm {
    return MVSResultUnknown;
}
- (MVSResult)disarm {
    return MVSResultUnknown;
}
- (MVSResult)takeoff {
    return MVSResultUnknown;
}
- (MVSResult)land {
    return MVSResultUnknown;
}
- (MVSResult)reboot {
    return MVSResultUnknown;
}
- (MVSResult)shutdown {
    return MVSResultUnknown;
}
- (MVSResult)terminate {
    return MVSResultUnknown;
}
- (MVSResult)kill {
    return MVSResultUnknown;
}
- (MVSResult)returnToLaunch {
    return MVSResultUnknown;
}
- (MVSResult)gotoLocation:(double)latitudeDeg :(double)longitudeDeg :(float)absoluteAltitudeM :(float)yawDeg {
    return MVSResultUnknown;
}
- (MVSResult)doOrbit:(float)radiusM :(float)velocityMs :(MVSOrbitYawBehavior*)yawBehavior :(double)latitudeDeg :(double)longitudeDeg :(double)absoluteAltitudeM {
    return MVSResultUnknown;
}
- (MVSResult)transitionToFixedwing {
    return MVSResultUnknown;
}
- (MVSResult)transitionToMulticopter {
    return MVSResultUnknown;
}

- (MVSResult)setTakeoffAltitude:(float)altitude {
    return MVSResultUnknown;
}

- (MVSResult)setMaximumSpeed:(float)speed {
    return MVSResultUnknown;
}

- (MVSResult)setReturnToLaunchAltitude:(float)relativeAltitudeM {
    return MVSResultUnknown;
}










@end