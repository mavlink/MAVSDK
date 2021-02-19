#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mocap/mocap.h>





@interface MVSPositionBody : NSObject



@property (nonatomic, assign) float xM;


@property (nonatomic, assign) float yM;


@property (nonatomic, assign) float zM;



@end




@interface MVSAngleBody : NSObject



@property (nonatomic, assign) float rollRad;


@property (nonatomic, assign) float pitchRad;


@property (nonatomic, assign) float yawRad;



@end




@interface MVSSpeedBody : NSObject



@property (nonatomic, assign) float xMS;


@property (nonatomic, assign) float yMS;


@property (nonatomic, assign) float zMS;



@end




@interface MVSAngularVelocityBody : NSObject



@property (nonatomic, assign) float rollRadS;


@property (nonatomic, assign) float pitchRadS;


@property (nonatomic, assign) float yawRadS;



@end




@interface MVSCovariance : NSObject



@property (nonatomic, assign) float covarianceMatrix;



@end




@interface MVSQuaternion : NSObject



@property (nonatomic, assign) float w;


@property (nonatomic, assign) float x;


@property (nonatomic, assign) float y;


@property (nonatomic, assign) float z;



@end




@interface MVSVisionPositionEstimate : NSObject



@property (nonatomic, assign) UInt64 timeUsec;


@property (nonatomic, strong) MVSPositionBody *positionBody;


@property (nonatomic, strong) MVSAngleBody *angleBody;


@property (nonatomic, strong) MVSCovariance *poseCovariance;



@end




@interface MVSAttitudePositionMocap : NSObject



@property (nonatomic, assign) UInt64 timeUsec;


@property (nonatomic, strong) MVSQuaternion *q;


@property (nonatomic, strong) MVSPositionBody *positionBody;


@property (nonatomic, strong) MVSCovariance *poseCovariance;



@end





@interface MVSOdometry : NSObject



typedef NS_ENUM(NSInteger, MVSMavFrame)  {
    MVSMavFrameMocapNed,
    MVSMavFrameLocalFrd,
};


@property (nonatomic, assign) UInt64 timeUsec;


@property (nonatomic, assign) MVSMavFrame frameId;


@property (nonatomic, strong) MVSPositionBody *positionBody;


@property (nonatomic, strong) MVSQuaternion *q;


@property (nonatomic, strong) MVSSpeedBody *speedBody;


@property (nonatomic, strong) MVSAngularVelocityBody *angularVelocityBody;


@property (nonatomic, strong) MVSCovariance *poseCovariance;


@property (nonatomic, strong) MVSCovariance *velocityCovariance;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultInvalidRequestData,
};



@interface MVSMocap : NSObject

- (MVSResult)setVisionPositionEstimate
:(
MVSVisionPositionEstimate*)
visionPositionEstimate;
- (MVSResult)setAttitudePositionMocap
:(
MVSAttitudePositionMocap*)
attitudePositionMocap;
- (MVSResult)setOdometry
:(
MVSOdometry*)
odometry;


@end