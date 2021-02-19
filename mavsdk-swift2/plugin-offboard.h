#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/offboard/offboard.h>





@interface MVSAttitude : NSObject



@property (nonatomic, assign) float rollDeg;


@property (nonatomic, assign) float pitchDeg;


@property (nonatomic, assign) float yawDeg;


@property (nonatomic, assign) float thrustValue;



@end




@interface MVSActuatorControlGroup : NSObject



@property (nonatomic, assign) float controls;



@end




@interface MVSActuatorControl : NSObject



@property (nonatomic, strong) NSMutableArray *groups;



@end




@interface MVSAttitudeRate : NSObject



@property (nonatomic, assign) float rollDegS;


@property (nonatomic, assign) float pitchDegS;


@property (nonatomic, assign) float yawDegS;


@property (nonatomic, assign) float thrustValue;



@end




@interface MVSPositionNedYaw : NSObject



@property (nonatomic, assign) float northM;


@property (nonatomic, assign) float eastM;


@property (nonatomic, assign) float downM;


@property (nonatomic, assign) float yawDeg;



@end




@interface MVSVelocityBodyYawspeed : NSObject



@property (nonatomic, assign) float forwardMS;


@property (nonatomic, assign) float rightMS;


@property (nonatomic, assign) float downMS;


@property (nonatomic, assign) float yawspeedDegS;



@end




@interface MVSVelocityNedYaw : NSObject



@property (nonatomic, assign) float northMS;


@property (nonatomic, assign) float eastMS;


@property (nonatomic, assign) float downMS;


@property (nonatomic, assign) float yawDeg;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultBusy,
    MVSResultCommandDenied,
    MVSResultTimeout,
    MVSResultNoSetpointSet,
};



@interface MVSOffboard : NSObject

- (MVSResult)start
;
- (MVSResult)stop
;

- (MVSResult)setAttitude
:(
MVSAttitude*)
attitude;
- (MVSResult)setActuatorControl
:(
MVSActuatorControl*)
actuatorControl;
- (MVSResult)setAttitudeRate
:(
MVSAttitudeRate*)
attitudeRate;
- (MVSResult)setPositionNed
:(
MVSPositionNedYaw*)
positionNedYaw;
- (MVSResult)setVelocityBody
:(
MVSVelocityBodyYawspeed*)
velocityBodyYawspeed;
- (MVSResult)setVelocityNed
:(
MVSVelocityNedYaw*)
velocityNedYaw;
- (MVSResult)setPositionVelocityNed
:(
MVSPositionNedYaw*)
positionNedYaw :(
MVSVelocityNedYaw*)
velocityNedYaw;


@end