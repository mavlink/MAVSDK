#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>



typedef NS_ENUM(NSInteger, MVSOrbitYawBehavior)  {
    MVSOrbitYawBehaviorHoldFrontToCircleCenter,
    MVSOrbitYawBehaviorHoldInitialHeading,
    MVSOrbitYawBehaviorUncontrolled,
    MVSOrbitYawBehaviorHoldFrontTangentToCircle,
    MVSOrbitYawBehaviorRcControlled,
};






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultBusy,
    MVSResultCommandDenied,
    MVSResultCommandDeniedLandedStateUnknown,
    MVSResultCommandDeniedNotLanded,
    MVSResultTimeout,
    MVSResultVtolTransitionSupportUnknown,
    MVSResultNoVtolTransitionSupport,
    MVSResultParameterError,
};



@interface MVSAction : NSObject

- (MVSResult)arm
;
- (MVSResult)disarm
;
- (MVSResult)takeoff
;
- (MVSResult)land
;
- (MVSResult)reboot
;
- (MVSResult)shutdown
;
- (MVSResult)terminate
;
- (MVSResult)kill
;
- (MVSResult)returnToLaunch
;
- (MVSResult)gotoLocation
:(
double)
latitudeDeg :(
double)
longitudeDeg :(
float)
absoluteAltitudeM :(
float)
yawDeg;
- (MVSResult)doOrbit
:(
float)
radiusM :(
float)
velocityMs :(
MVSOrbitYawBehavior*)
yawBehavior :(
double)
latitudeDeg :(
double)
longitudeDeg :(
double)
absoluteAltitudeM;
- (MVSResult)transitionToFixedwing
;
- (MVSResult)transitionToMulticopter
;

- (MVSResult)setTakeoffAltitude
:(
float)
altitude;

- (MVSResult)setMaximumSpeed
:(
float)
speed;

- (MVSResult)setReturnToLaunchAltitude
:(
float)
relativeAltitudeM;


@end