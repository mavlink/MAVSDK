#ifndef plugin_offboard_h
#define plugin_offboard_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKOffboardAttitude : NSObject


@property (nonatomic, assign) float rollDeg; 
@property (nonatomic, assign) float pitchDeg; 
@property (nonatomic, assign) float yawDeg; 
@property (nonatomic, assign) float thrustValue; 


@end




@interface MAVSDKOffboardActuatorControlGroup : NSObject


@property (nonatomic, strong) NSMutableArray *controls;


@end




@interface MAVSDKOffboardActuatorControl : NSObject


@property (nonatomic, strong) NSMutableArray *groups;


@end




@interface MAVSDKOffboardAttitudeRate : NSObject


@property (nonatomic, assign) float rollDegS; 
@property (nonatomic, assign) float pitchDegS; 
@property (nonatomic, assign) float yawDegS; 
@property (nonatomic, assign) float thrustValue; 


@end




@interface MAVSDKOffboardPositionNedYaw : NSObject


@property (nonatomic, assign) float northM; 
@property (nonatomic, assign) float eastM; 
@property (nonatomic, assign) float downM; 
@property (nonatomic, assign) float yawDeg; 


@end




@interface MAVSDKOffboardVelocityBodyYawspeed : NSObject


@property (nonatomic, assign) float forwardMS; 
@property (nonatomic, assign) float rightMS; 
@property (nonatomic, assign) float downMS; 
@property (nonatomic, assign) float yawspeedDegS; 


@end




@interface MAVSDKOffboardVelocityNedYaw : NSObject


@property (nonatomic, assign) float northMS; 
@property (nonatomic, assign) float eastMS; 
@property (nonatomic, assign) float downMS; 
@property (nonatomic, assign) float yawDeg; 


@end






typedef NS_ENUM(NSInteger, MAVSDKOffboardResult)  {
    MAVSDKOffboardResultUnknown,
    MAVSDKOffboardResultSuccess,
    MAVSDKOffboardResultNoSystem,
    MAVSDKOffboardResultConnectionError,
    MAVSDKOffboardResultBusy,
    MAVSDKOffboardResultCommandDenied,
    MAVSDKOffboardResultTimeout,
    MAVSDKOffboardResultNoSetpointSet,
};



@interface MAVSDKOffboard : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKOffboardResult)start ;
- (MAVSDKOffboardResult)stop ;

- (MAVSDKOffboardResult)setAttitude :(MAVSDKOffboardAttitude*) attitude;
- (MAVSDKOffboardResult)setActuatorControl :(MAVSDKOffboardActuatorControl*) actuatorControl;
- (MAVSDKOffboardResult)setAttitudeRate :(MAVSDKOffboardAttitudeRate*) attitudeRate;
- (MAVSDKOffboardResult)setPositionNed :(MAVSDKOffboardPositionNedYaw*) positionNedYaw;
- (MAVSDKOffboardResult)setVelocityBody :(MAVSDKOffboardVelocityBodyYawspeed*) velocityBodyYawspeed;
- (MAVSDKOffboardResult)setVelocityNed :(MAVSDKOffboardVelocityNedYaw*) velocityNedYaw;
- (MAVSDKOffboardResult)setPositionVelocityNed :(MAVSDKOffboardPositionNedYaw*) positionNedYaw :(MAVSDKOffboardVelocityNedYaw*) velocityNedYaw;


@end

#endif // plugin_offboard_h