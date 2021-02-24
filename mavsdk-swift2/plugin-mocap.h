#ifndef plugin_mocap_h
#define plugin_mocap_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKMocapPositionBody : NSObject


@property (nonatomic, assign) float xM; 
@property (nonatomic, assign) float yM; 
@property (nonatomic, assign) float zM; 


@end




@interface MAVSDKMocapAngleBody : NSObject


@property (nonatomic, assign) float rollRad; 
@property (nonatomic, assign) float pitchRad; 
@property (nonatomic, assign) float yawRad; 


@end




@interface MAVSDKMocapSpeedBody : NSObject


@property (nonatomic, assign) float xMS; 
@property (nonatomic, assign) float yMS; 
@property (nonatomic, assign) float zMS; 


@end




@interface MAVSDKMocapAngularVelocityBody : NSObject


@property (nonatomic, assign) float rollRadS; 
@property (nonatomic, assign) float pitchRadS; 
@property (nonatomic, assign) float yawRadS; 


@end




@interface MAVSDKMocapCovariance : NSObject


@property (nonatomic, strong) NSMutableArray *covarianceMatrix;


@end




@interface MAVSDKMocapQuaternion : NSObject


@property (nonatomic, assign) float w; 
@property (nonatomic, assign) float x; 
@property (nonatomic, assign) float y; 
@property (nonatomic, assign) float z; 


@end




@interface MAVSDKMocapVisionPositionEstimate : NSObject


@property (nonatomic, assign) UInt64 timeUsec; 
@property (nonatomic, strong) MAVSDKMocapPositionBody *positionBody;
@property (nonatomic, strong) MAVSDKMocapAngleBody *angleBody;
@property (nonatomic, strong) MAVSDKMocapCovariance *poseCovariance;


@end




@interface MAVSDKMocapAttitudePositionMocap : NSObject


@property (nonatomic, assign) UInt64 timeUsec; 
@property (nonatomic, strong) MAVSDKMocapQuaternion *q;
@property (nonatomic, strong) MAVSDKMocapPositionBody *positionBody;
@property (nonatomic, strong) MAVSDKMocapCovariance *poseCovariance;


@end





@interface MAVSDKMocapOdometry : NSObject



typedef NS_ENUM(NSInteger, MAVSDKMocapMavFrame)  {
    MAVSDKMocapMavFrameMocapNed,
    MAVSDKMocapMavFrameLocalFrd,
};

@property (nonatomic, assign) UInt64 timeUsec; 
@property (nonatomic, assign) MAVSDKMocapMavFrame frameId;
@property (nonatomic, strong) MAVSDKMocapPositionBody *positionBody;
@property (nonatomic, strong) MAVSDKMocapQuaternion *q;
@property (nonatomic, strong) MAVSDKMocapSpeedBody *speedBody;
@property (nonatomic, strong) MAVSDKMocapAngularVelocityBody *angularVelocityBody;
@property (nonatomic, strong) MAVSDKMocapCovariance *poseCovariance;
@property (nonatomic, strong) MAVSDKMocapCovariance *velocityCovariance;


@end






typedef NS_ENUM(NSInteger, MAVSDKMocapResult)  {
    MAVSDKMocapResultUnknown,
    MAVSDKMocapResultSuccess,
    MAVSDKMocapResultNoSystem,
    MAVSDKMocapResultConnectionError,
    MAVSDKMocapResultInvalidRequestData,
};



@interface MAVSDKMocap : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKMocapResult)setVisionPositionEstimate :(MAVSDKMocapVisionPositionEstimate*) visionPositionEstimate;
- (MAVSDKMocapResult)setAttitudePositionMocap :(MAVSDKMocapAttitudePositionMocap*) attitudePositionMocap;
- (MAVSDKMocapResult)setOdometry :(MAVSDKMocapOdometry*) odometry;


@end

#endif // plugin_mocap_h