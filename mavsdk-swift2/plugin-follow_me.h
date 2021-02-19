#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/follow_me/follow_me.h>






@interface MVSConfig : NSObject



typedef NS_ENUM(NSInteger, MVSFollowDirection)  {
    MVSFollowDirectionNone,
    MVSFollowDirectionBehind,
    MVSFollowDirectionFront,
    MVSFollowDirectionFrontRight,
    MVSFollowDirectionFrontLeft,
};


@property (nonatomic, assign) float minHeightM;


@property (nonatomic, assign) float followDistanceM;


@property (nonatomic, assign) MVSFollowDirection followDirection;


@property (nonatomic, assign) float responsiveness;



@end




@interface MVSTargetLocation : NSObject



@property (nonatomic, assign) double latitudeDeg;


@property (nonatomic, assign) double longitudeDeg;


@property (nonatomic, assign) float absoluteAltitudeM;


@property (nonatomic, assign) float velocityXMS;


@property (nonatomic, assign) float velocityYMS;


@property (nonatomic, assign) float velocityZMS;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultBusy,
    MVSResultCommandDenied,
    MVSResultTimeout,
    MVSResultNotActive,
    MVSResultSetConfigFailed,
};



@interface MVSFollowMe : NSObject


- (MVSResult)setConfig
:(
MVSConfig*)
config;

- (MVSResult)setTargetLocation
:(
MVSTargetLocation*)
location;

- (MVSResult)start
;
- (MVSResult)stop
;


@end