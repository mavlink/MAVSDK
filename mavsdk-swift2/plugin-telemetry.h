#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>



typedef NS_ENUM(NSInteger, MVSFixType)  {
    MVSFixTypeNoGps,
    MVSFixTypeNoFix,
    MVSFixTypeFix2D,
    MVSFixTypeFix3D,
    MVSFixTypeFixDgps,
    MVSFixTypeRtkFloat,
    MVSFixTypeRtkFixed,
};


typedef NS_ENUM(NSInteger, MVSFlightMode)  {
    MVSFlightModeUnknown,
    MVSFlightModeReady,
    MVSFlightModeTakeoff,
    MVSFlightModeHold,
    MVSFlightModeMission,
    MVSFlightModeReturnToLaunch,
    MVSFlightModeLand,
    MVSFlightModeOffboard,
    MVSFlightModeFollowMe,
    MVSFlightModeManual,
    MVSFlightModeAltctl,
    MVSFlightModePosctl,
    MVSFlightModeAcro,
    MVSFlightModeStabilized,
    MVSFlightModeRattitude,
};


typedef NS_ENUM(NSInteger, MVSStatusTextType)  {
    MVSStatusTextTypeDebug,
    MVSStatusTextTypeInfo,
    MVSStatusTextTypeNotice,
    MVSStatusTextTypeWarning,
    MVSStatusTextTypeError,
    MVSStatusTextTypeCritical,
    MVSStatusTextTypeAlert,
    MVSStatusTextTypeEmergency,
};


typedef NS_ENUM(NSInteger, MVSLandedState)  {
    MVSLandedStateUnknown,
    MVSLandedStateOnGround,
    MVSLandedStateInAir,
    MVSLandedStateTakingOff,
    MVSLandedStateLanding,
};




@interface MVSPosition : NSObject



@property (nonatomic, assign) double latitudeDeg;


@property (nonatomic, assign) double longitudeDeg;


@property (nonatomic, assign) float absoluteAltitudeM;


@property (nonatomic, assign) float relativeAltitudeM;



@end




@interface MVSQuaternion : NSObject



@property (nonatomic, assign) float w;


@property (nonatomic, assign) float x;


@property (nonatomic, assign) float y;


@property (nonatomic, assign) float z;


@property (nonatomic, assign) UInt64 timestampUs;



@end




@interface MVSEulerAngle : NSObject



@property (nonatomic, assign) float rollDeg;


@property (nonatomic, assign) float pitchDeg;


@property (nonatomic, assign) float yawDeg;


@property (nonatomic, assign) UInt64 timestampUs;



@end




@interface MVSAngularVelocityBody : NSObject



@property (nonatomic, assign) float rollRadS;


@property (nonatomic, assign) float pitchRadS;


@property (nonatomic, assign) float yawRadS;



@end




@interface MVSGpsInfo : NSObject



@property (nonatomic, assign) NSInteger numSatellites;


@property (nonatomic, assign) MVSFixType fixType;



@end




@interface MVSBattery : NSObject



@property (nonatomic, assign) float voltageV;


@property (nonatomic, assign) float remainingPercent;



@end




@interface MVSHealth : NSObject



@property (nonatomic, assign) BOOL isGyrometerCalibrationOk;


@property (nonatomic, assign) BOOL isAccelerometerCalibrationOk;


@property (nonatomic, assign) BOOL isMagnetometerCalibrationOk;


@property (nonatomic, assign) BOOL isLevelCalibrationOk;


@property (nonatomic, assign) BOOL isLocalPositionOk;


@property (nonatomic, assign) BOOL isGlobalPositionOk;


@property (nonatomic, assign) BOOL isHomePositionOk;



@end




@interface MVSRcStatus : NSObject



@property (nonatomic, assign) BOOL wasAvailableOnce;


@property (nonatomic, assign) BOOL isAvailable;


@property (nonatomic, assign) float signalStrengthPercent;



@end




@interface MVSStatusText : NSObject



@property (nonatomic, assign) MVSStatusTextType type;


@property (nonatomic, strong) NSString *text;



@end




@interface MVSActuatorControlTarget : NSObject



@property (nonatomic, assign) NSInteger group;


@property (nonatomic, assign) float controls;



@end




@interface MVSActuatorOutputStatus : NSObject



@property (nonatomic, assign) UInt32 active;


@property (nonatomic, assign) float actuator;



@end




@interface MVSCovariance : NSObject



@property (nonatomic, assign) float covarianceMatrix;



@end




@interface MVSVelocityBody : NSObject



@property (nonatomic, assign) float xMS;


@property (nonatomic, assign) float yMS;


@property (nonatomic, assign) float zMS;



@end




@interface MVSPositionBody : NSObject



@property (nonatomic, assign) float xM;


@property (nonatomic, assign) float yM;


@property (nonatomic, assign) float zM;



@end





@interface MVSOdometry : NSObject



typedef NS_ENUM(NSInteger, MVSMavFrame)  {
    MVSMavFrameUndef,
    MVSMavFrameBodyNed,
    MVSMavFrameVisionNed,
    MVSMavFrameEstimNed,
};


@property (nonatomic, assign) UInt64 timeUsec;


@property (nonatomic, assign) MVSMavFrame frameId;


@property (nonatomic, assign) MVSMavFrame childFrameId;


@property (nonatomic, strong) MVSPositionBody *positionBody;


@property (nonatomic, strong) MVSQuaternion *q;


@property (nonatomic, strong) MVSVelocityBody *velocityBody;


@property (nonatomic, strong) MVSAngularVelocityBody *angularVelocityBody;


@property (nonatomic, strong) MVSCovariance *poseCovariance;


@property (nonatomic, strong) MVSCovariance *velocityCovariance;



@end




@interface MVSDistanceSensor : NSObject



@property (nonatomic, assign) float minimumDistanceM;


@property (nonatomic, assign) float maximumDistanceM;


@property (nonatomic, assign) float currentDistanceM;



@end




@interface MVSPositionNed : NSObject



@property (nonatomic, assign) float northM;


@property (nonatomic, assign) float eastM;


@property (nonatomic, assign) float downM;



@end




@interface MVSVelocityNed : NSObject



@property (nonatomic, assign) float northMS;


@property (nonatomic, assign) float eastMS;


@property (nonatomic, assign) float downMS;



@end




@interface MVSPositionVelocityNed : NSObject



@property (nonatomic, strong) MVSPositionNed *position;


@property (nonatomic, strong) MVSVelocityNed *velocity;



@end




@interface MVSGroundTruth : NSObject



@property (nonatomic, assign) double latitudeDeg;


@property (nonatomic, assign) double longitudeDeg;


@property (nonatomic, assign) float absoluteAltitudeM;



@end




@interface MVSFixedwingMetrics : NSObject



@property (nonatomic, assign) float airspeedMS;


@property (nonatomic, assign) float throttlePercentage;


@property (nonatomic, assign) float climbRateMS;



@end




@interface MVSAccelerationFrd : NSObject



@property (nonatomic, assign) float forwardMS2;


@property (nonatomic, assign) float rightMS2;


@property (nonatomic, assign) float downMS2;



@end




@interface MVSAngularVelocityFrd : NSObject



@property (nonatomic, assign) float forwardRadS;


@property (nonatomic, assign) float rightRadS;


@property (nonatomic, assign) float downRadS;



@end




@interface MVSMagneticFieldFrd : NSObject



@property (nonatomic, assign) float forwardGauss;


@property (nonatomic, assign) float rightGauss;


@property (nonatomic, assign) float downGauss;



@end




@interface MVSImu : NSObject



@property (nonatomic, strong) MVSAccelerationFrd *accelerationFrd;


@property (nonatomic, strong) MVSAngularVelocityFrd *angularVelocityFrd;


@property (nonatomic, strong) MVSMagneticFieldFrd *magneticFieldFrd;


@property (nonatomic, assign) float temperatureDegc;



@end




@interface MVSGpsGlobalOrigin : NSObject



@property (nonatomic, assign) double latitudeDeg;


@property (nonatomic, assign) double longitudeDeg;


@property (nonatomic, assign) float altitudeM;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultBusy,
    MVSResultCommandDenied,
    MVSResultTimeout,
};



@interface MVSTelemetry : NSObject




























- (MVSResult)setRatePosition
:(
double)
rateHz;
- (MVSResult)setRateHome
:(
double)
rateHz;
- (MVSResult)setRateInAir
:(
double)
rateHz;
- (MVSResult)setRateLandedState
:(
double)
rateHz;
- (MVSResult)setRateAttitude
:(
double)
rateHz;
- (MVSResult)setRateCameraAttitude
:(
double)
rateHz;
- (MVSResult)setRateVelocityNed
:(
double)
rateHz;
- (MVSResult)setRateGpsInfo
:(
double)
rateHz;
- (MVSResult)setRateBattery
:(
double)
rateHz;
- (MVSResult)setRateRcStatus
:(
double)
rateHz;
- (MVSResult)setRateActuatorControlTarget
:(
double)
rateHz;
- (MVSResult)setRateActuatorOutputStatus
:(
double)
rateHz;
- (MVSResult)setRateOdometry
:(
double)
rateHz;
- (MVSResult)setRatePositionVelocityNed
:(
double)
rateHz;
- (MVSResult)setRateGroundTruth
:(
double)
rateHz;
- (MVSResult)setRateFixedwingMetrics
:(
double)
rateHz;
- (MVSResult)setRateImu
:(
double)
rateHz;
- (MVSResult)setRateUnixEpochTime
:(
double)
rateHz;
- (MVSResult)setRateDistanceSensor
:(
double)
rateHz;



@end