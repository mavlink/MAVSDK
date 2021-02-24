#ifndef plugin_telemetry_h
#define plugin_telemetry_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;



typedef NS_ENUM(NSInteger, MAVSDKTelemetryFixType)  {
    MAVSDKTelemetryFixTypeNoGps,
    MAVSDKTelemetryFixTypeNoFix,
    MAVSDKTelemetryFixTypeFix2D,
    MAVSDKTelemetryFixTypeFix3D,
    MAVSDKTelemetryFixTypeFixDgps,
    MAVSDKTelemetryFixTypeRtkFloat,
    MAVSDKTelemetryFixTypeRtkFixed,
};


typedef NS_ENUM(NSInteger, MAVSDKTelemetryFlightMode)  {
    MAVSDKTelemetryFlightModeUnknown,
    MAVSDKTelemetryFlightModeReady,
    MAVSDKTelemetryFlightModeTakeoff,
    MAVSDKTelemetryFlightModeHold,
    MAVSDKTelemetryFlightModeMission,
    MAVSDKTelemetryFlightModeReturnToLaunch,
    MAVSDKTelemetryFlightModeLand,
    MAVSDKTelemetryFlightModeOffboard,
    MAVSDKTelemetryFlightModeFollowMe,
    MAVSDKTelemetryFlightModeManual,
    MAVSDKTelemetryFlightModeAltctl,
    MAVSDKTelemetryFlightModePosctl,
    MAVSDKTelemetryFlightModeAcro,
    MAVSDKTelemetryFlightModeStabilized,
    MAVSDKTelemetryFlightModeRattitude,
};


typedef NS_ENUM(NSInteger, MAVSDKTelemetryStatusTextType)  {
    MAVSDKTelemetryStatusTextTypeDebug,
    MAVSDKTelemetryStatusTextTypeInfo,
    MAVSDKTelemetryStatusTextTypeNotice,
    MAVSDKTelemetryStatusTextTypeWarning,
    MAVSDKTelemetryStatusTextTypeError,
    MAVSDKTelemetryStatusTextTypeCritical,
    MAVSDKTelemetryStatusTextTypeAlert,
    MAVSDKTelemetryStatusTextTypeEmergency,
};


typedef NS_ENUM(NSInteger, MAVSDKTelemetryLandedState)  {
    MAVSDKTelemetryLandedStateUnknown,
    MAVSDKTelemetryLandedStateOnGround,
    MAVSDKTelemetryLandedStateInAir,
    MAVSDKTelemetryLandedStateTakingOff,
    MAVSDKTelemetryLandedStateLanding,
};




@interface MAVSDKTelemetryPosition : NSObject


@property (nonatomic, assign) double latitudeDeg; 
@property (nonatomic, assign) double longitudeDeg; 
@property (nonatomic, assign) float absoluteAltitudeM; 
@property (nonatomic, assign) float relativeAltitudeM; 


@end




@interface MAVSDKTelemetryQuaternion : NSObject


@property (nonatomic, assign) float w; 
@property (nonatomic, assign) float x; 
@property (nonatomic, assign) float y; 
@property (nonatomic, assign) float z; 
@property (nonatomic, assign) UInt64 timestampUs; 


@end




@interface MAVSDKTelemetryEulerAngle : NSObject


@property (nonatomic, assign) float rollDeg; 
@property (nonatomic, assign) float pitchDeg; 
@property (nonatomic, assign) float yawDeg; 
@property (nonatomic, assign) UInt64 timestampUs; 


@end




@interface MAVSDKTelemetryAngularVelocityBody : NSObject


@property (nonatomic, assign) float rollRadS; 
@property (nonatomic, assign) float pitchRadS; 
@property (nonatomic, assign) float yawRadS; 


@end




@interface MAVSDKTelemetryGpsInfo : NSObject


@property (nonatomic, assign) SInt32 numSatellites; 
@property (nonatomic, assign) MAVSDKTelemetryFixType fixType;


@end




@interface MAVSDKTelemetryBattery : NSObject


@property (nonatomic, assign) float voltageV; 
@property (nonatomic, assign) float remainingPercent; 


@end




@interface MAVSDKTelemetryHealth : NSObject


@property (nonatomic, assign) BOOL isGyrometerCalibrationOk; 
@property (nonatomic, assign) BOOL isAccelerometerCalibrationOk; 
@property (nonatomic, assign) BOOL isMagnetometerCalibrationOk; 
@property (nonatomic, assign) BOOL isLevelCalibrationOk; 
@property (nonatomic, assign) BOOL isLocalPositionOk; 
@property (nonatomic, assign) BOOL isGlobalPositionOk; 
@property (nonatomic, assign) BOOL isHomePositionOk; 


@end




@interface MAVSDKTelemetryRcStatus : NSObject


@property (nonatomic, assign) BOOL wasAvailableOnce; 
@property (nonatomic, assign) BOOL isAvailable; 
@property (nonatomic, assign) float signalStrengthPercent; 


@end




@interface MAVSDKTelemetryStatusText : NSObject


@property (nonatomic, assign) MAVSDKTelemetryStatusTextType type;
@property (nonatomic, strong) NSString *text;


@end




@interface MAVSDKTelemetryActuatorControlTarget : NSObject


@property (nonatomic, assign) SInt32 group; 
@property (nonatomic, strong) NSMutableArray *controls;


@end




@interface MAVSDKTelemetryActuatorOutputStatus : NSObject


@property (nonatomic, assign) UInt32 active; 
@property (nonatomic, strong) NSMutableArray *actuator;


@end




@interface MAVSDKTelemetryCovariance : NSObject


@property (nonatomic, strong) NSMutableArray *covarianceMatrix;


@end




@interface MAVSDKTelemetryVelocityBody : NSObject


@property (nonatomic, assign) float xMS; 
@property (nonatomic, assign) float yMS; 
@property (nonatomic, assign) float zMS; 


@end




@interface MAVSDKTelemetryPositionBody : NSObject


@property (nonatomic, assign) float xM; 
@property (nonatomic, assign) float yM; 
@property (nonatomic, assign) float zM; 


@end





@interface MAVSDKTelemetryOdometry : NSObject



typedef NS_ENUM(NSInteger, MAVSDKTelemetryMavFrame)  {
    MAVSDKTelemetryMavFrameUndef,
    MAVSDKTelemetryMavFrameBodyNed,
    MAVSDKTelemetryMavFrameVisionNed,
    MAVSDKTelemetryMavFrameEstimNed,
};

@property (nonatomic, assign) UInt64 timeUsec; 
@property (nonatomic, assign) MAVSDKTelemetryMavFrame frameId;
@property (nonatomic, assign) MAVSDKTelemetryMavFrame childFrameId;
@property (nonatomic, strong) MAVSDKTelemetryPositionBody *positionBody;
@property (nonatomic, strong) MAVSDKTelemetryQuaternion *q;
@property (nonatomic, strong) MAVSDKTelemetryVelocityBody *velocityBody;
@property (nonatomic, strong) MAVSDKTelemetryAngularVelocityBody *angularVelocityBody;
@property (nonatomic, strong) MAVSDKTelemetryCovariance *poseCovariance;
@property (nonatomic, strong) MAVSDKTelemetryCovariance *velocityCovariance;


@end




@interface MAVSDKTelemetryDistanceSensor : NSObject


@property (nonatomic, assign) float minimumDistanceM; 
@property (nonatomic, assign) float maximumDistanceM; 
@property (nonatomic, assign) float currentDistanceM; 


@end




@interface MAVSDKTelemetryPositionNed : NSObject


@property (nonatomic, assign) float northM; 
@property (nonatomic, assign) float eastM; 
@property (nonatomic, assign) float downM; 


@end




@interface MAVSDKTelemetryVelocityNed : NSObject


@property (nonatomic, assign) float northMS; 
@property (nonatomic, assign) float eastMS; 
@property (nonatomic, assign) float downMS; 


@end




@interface MAVSDKTelemetryPositionVelocityNed : NSObject


@property (nonatomic, strong) MAVSDKTelemetryPositionNed *position;
@property (nonatomic, strong) MAVSDKTelemetryVelocityNed *velocity;


@end




@interface MAVSDKTelemetryGroundTruth : NSObject


@property (nonatomic, assign) double latitudeDeg; 
@property (nonatomic, assign) double longitudeDeg; 
@property (nonatomic, assign) float absoluteAltitudeM; 


@end




@interface MAVSDKTelemetryFixedwingMetrics : NSObject


@property (nonatomic, assign) float airspeedMS; 
@property (nonatomic, assign) float throttlePercentage; 
@property (nonatomic, assign) float climbRateMS; 


@end




@interface MAVSDKTelemetryAccelerationFrd : NSObject


@property (nonatomic, assign) float forwardMS2; 
@property (nonatomic, assign) float rightMS2; 
@property (nonatomic, assign) float downMS2; 


@end




@interface MAVSDKTelemetryAngularVelocityFrd : NSObject


@property (nonatomic, assign) float forwardRadS; 
@property (nonatomic, assign) float rightRadS; 
@property (nonatomic, assign) float downRadS; 


@end




@interface MAVSDKTelemetryMagneticFieldFrd : NSObject


@property (nonatomic, assign) float forwardGauss; 
@property (nonatomic, assign) float rightGauss; 
@property (nonatomic, assign) float downGauss; 


@end




@interface MAVSDKTelemetryImu : NSObject


@property (nonatomic, strong) MAVSDKTelemetryAccelerationFrd *accelerationFrd;
@property (nonatomic, strong) MAVSDKTelemetryAngularVelocityFrd *angularVelocityFrd;
@property (nonatomic, strong) MAVSDKTelemetryMagneticFieldFrd *magneticFieldFrd;
@property (nonatomic, assign) float temperatureDegc; 


@end




@interface MAVSDKTelemetryGpsGlobalOrigin : NSObject


@property (nonatomic, assign) double latitudeDeg; 
@property (nonatomic, assign) double longitudeDeg; 
@property (nonatomic, assign) float altitudeM; 


@end






typedef NS_ENUM(NSInteger, MAVSDKTelemetryResult)  {
    MAVSDKTelemetryResultUnknown,
    MAVSDKTelemetryResultSuccess,
    MAVSDKTelemetryResultNoSystem,
    MAVSDKTelemetryResultConnectionError,
    MAVSDKTelemetryResultBusy,
    MAVSDKTelemetryResultCommandDenied,
    MAVSDKTelemetryResultTimeout,
};



@interface MAVSDKTelemetry : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;



























- (MAVSDKTelemetryResult)setRatePosition :( double) rateHz;
- (MAVSDKTelemetryResult)setRateHome :( double) rateHz;
- (MAVSDKTelemetryResult)setRateInAir :( double) rateHz;
- (MAVSDKTelemetryResult)setRateLandedState :( double) rateHz;
- (MAVSDKTelemetryResult)setRateAttitude :( double) rateHz;
- (MAVSDKTelemetryResult)setRateCameraAttitude :( double) rateHz;
- (MAVSDKTelemetryResult)setRateVelocityNed :( double) rateHz;
- (MAVSDKTelemetryResult)setRateGpsInfo :( double) rateHz;
- (MAVSDKTelemetryResult)setRateBattery :( double) rateHz;
- (MAVSDKTelemetryResult)setRateRcStatus :( double) rateHz;
- (MAVSDKTelemetryResult)setRateActuatorControlTarget :( double) rateHz;
- (MAVSDKTelemetryResult)setRateActuatorOutputStatus :( double) rateHz;
- (MAVSDKTelemetryResult)setRateOdometry :( double) rateHz;
- (MAVSDKTelemetryResult)setRatePositionVelocityNed :( double) rateHz;
- (MAVSDKTelemetryResult)setRateGroundTruth :( double) rateHz;
- (MAVSDKTelemetryResult)setRateFixedwingMetrics :( double) rateHz;
- (MAVSDKTelemetryResult)setRateImu :( double) rateHz;
- (MAVSDKTelemetryResult)setRateUnixEpochTime :( double) rateHz;
- (MAVSDKTelemetryResult)setRateDistanceSensor :( double) rateHz;



@end

#endif // plugin_telemetry_h