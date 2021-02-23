#ifndef plugin_manual_control_h
#define plugin_manual_control_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;







typedef NS_ENUM(NSInteger, MAVSDKManualControlResult)  {
    MAVSDKManualControlResultUnknown,
    MAVSDKManualControlResultSuccess,
    MAVSDKManualControlResultNoSystem,
    MAVSDKManualControlResultConnectionError,
    MAVSDKManualControlResultBusy,
    MAVSDKManualControlResultCommandDenied,
    MAVSDKManualControlResultTimeout,
    MAVSDKManualControlResultInputOutOfRange,
    MAVSDKManualControlResultInputNotSet,
};



@interface MAVSDKManualControl : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKManualControlResult)startPositionControl ;
- (MAVSDKManualControlResult)startAltitudeControl ;
- (MAVSDKManualControlResult)setManualControlInput :( float) x :( float) y :( float) z :( float) r;


@end

#endif // plugin_manual_control_h