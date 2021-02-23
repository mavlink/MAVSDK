#ifndef plugin_action_h
#define plugin_action_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;



typedef NS_ENUM(NSInteger, MAVSDKActionOrbitYawBehavior)  {
    MAVSDKActionOrbitYawBehaviorHoldFrontToCircleCenter,
    MAVSDKActionOrbitYawBehaviorHoldInitialHeading,
    MAVSDKActionOrbitYawBehaviorUncontrolled,
    MAVSDKActionOrbitYawBehaviorHoldFrontTangentToCircle,
    MAVSDKActionOrbitYawBehaviorRcControlled,
};






typedef NS_ENUM(NSInteger, MAVSDKActionResult)  {
    MAVSDKActionResultUnknown,
    MAVSDKActionResultSuccess,
    MAVSDKActionResultNoSystem,
    MAVSDKActionResultConnectionError,
    MAVSDKActionResultBusy,
    MAVSDKActionResultCommandDenied,
    MAVSDKActionResultCommandDeniedLandedStateUnknown,
    MAVSDKActionResultCommandDeniedNotLanded,
    MAVSDKActionResultTimeout,
    MAVSDKActionResultVtolTransitionSupportUnknown,
    MAVSDKActionResultNoVtolTransitionSupport,
    MAVSDKActionResultParameterError,
};



@interface MAVSDKAction : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKActionResult)arm ;
- (MAVSDKActionResult)disarm ;
- (MAVSDKActionResult)takeoff ;
- (MAVSDKActionResult)land ;
- (MAVSDKActionResult)reboot ;
- (MAVSDKActionResult)shutdown ;
- (MAVSDKActionResult)terminate ;
- (MAVSDKActionResult)kill ;
- (MAVSDKActionResult)returnToLaunch ;
- (MAVSDKActionResult)gotoLocation :( double) latitudeDeg :( double) longitudeDeg :( float) absoluteAltitudeM :( float) yawDeg;
- (MAVSDKActionResult)doOrbit :( float) radiusM :( float) velocityMs :( MAVSDKActionOrbitYawBehavior) yawBehavior :( double) latitudeDeg :( double) longitudeDeg :( double) absoluteAltitudeM;
- (MAVSDKActionResult)transitionToFixedwing ;
- (MAVSDKActionResult)transitionToMulticopter ;

- (MAVSDKActionResult)setTakeoffAltitude :( float) altitude;

- (MAVSDKActionResult)setMaximumSpeed :( float) speed;

- (MAVSDKActionResult)setReturnToLaunchAltitude :( float) relativeAltitudeM;


@end

#endif // plugin_action_h