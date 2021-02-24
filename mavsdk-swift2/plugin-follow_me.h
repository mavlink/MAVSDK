#ifndef plugin_follow_me_h
#define plugin_follow_me_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;






@interface MAVSDKFollowMeConfig : NSObject



typedef NS_ENUM(NSInteger, MAVSDKFollowMeFollowDirection)  {
    MAVSDKFollowMeFollowDirectionNone,
    MAVSDKFollowMeFollowDirectionBehind,
    MAVSDKFollowMeFollowDirectionFront,
    MAVSDKFollowMeFollowDirectionFrontRight,
    MAVSDKFollowMeFollowDirectionFrontLeft,
};

@property (nonatomic, assign) float minHeightM; 
@property (nonatomic, assign) float followDistanceM; 
@property (nonatomic, assign) MAVSDKFollowMeFollowDirection followDirection;
@property (nonatomic, assign) float responsiveness; 


@end




@interface MAVSDKFollowMeTargetLocation : NSObject


@property (nonatomic, assign) double latitudeDeg; 
@property (nonatomic, assign) double longitudeDeg; 
@property (nonatomic, assign) float absoluteAltitudeM; 
@property (nonatomic, assign) float velocityXMS; 
@property (nonatomic, assign) float velocityYMS; 
@property (nonatomic, assign) float velocityZMS; 


@end






typedef NS_ENUM(NSInteger, MAVSDKFollowMeResult)  {
    MAVSDKFollowMeResultUnknown,
    MAVSDKFollowMeResultSuccess,
    MAVSDKFollowMeResultNoSystem,
    MAVSDKFollowMeResultConnectionError,
    MAVSDKFollowMeResultBusy,
    MAVSDKFollowMeResultCommandDenied,
    MAVSDKFollowMeResultTimeout,
    MAVSDKFollowMeResultNotActive,
    MAVSDKFollowMeResultSetConfigFailed,
};



@interface MAVSDKFollowMe : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;

- (MAVSDKFollowMeResult)setConfig :(MAVSDKFollowMeConfig*) config;

- (MAVSDKFollowMeResult)setTargetLocation :(MAVSDKFollowMeTargetLocation*) location;

- (MAVSDKFollowMeResult)start ;
- (MAVSDKFollowMeResult)stop ;


@end

#endif // plugin_follow_me_h