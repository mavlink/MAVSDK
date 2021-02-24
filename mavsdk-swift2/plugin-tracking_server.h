#ifndef plugin_tracking_server_h
#define plugin_tracking_server_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;



typedef NS_ENUM(NSInteger, MAVSDKTrackingServerCommandAnswer)  {
    MAVSDKTrackingServerCommandAnswerAccepted,
    MAVSDKTrackingServerCommandAnswerTemporarilyRejected,
    MAVSDKTrackingServerCommandAnswerDenied,
    MAVSDKTrackingServerCommandAnswerUnsupported,
    MAVSDKTrackingServerCommandAnswerFailed,
};




@interface MAVSDKTrackingServerTrackPoint : NSObject


@property (nonatomic, assign) float pointX; 
@property (nonatomic, assign) float pointY; 
@property (nonatomic, assign) float radius; 


@end




@interface MAVSDKTrackingServerTrackRectangle : NSObject


@property (nonatomic, assign) float topLeftCornerX; 
@property (nonatomic, assign) float topLeftCornerY; 
@property (nonatomic, assign) float bottomRightCornerX; 
@property (nonatomic, assign) float bottomRightCornerY; 


@end






typedef NS_ENUM(NSInteger, MAVSDKTrackingServerResult)  {
    MAVSDKTrackingServerResultUnknown,
    MAVSDKTrackingServerResultSuccess,
    MAVSDKTrackingServerResultNoSystem,
    MAVSDKTrackingServerResultConnectionError,
};



@interface MAVSDKTrackingServer : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (void)setTrackingPointStatus :(MAVSDKTrackingServerTrackPoint*) trackedPoint;
- (void)setTrackingRectangleStatus :(MAVSDKTrackingServerTrackRectangle*) trackedRectangle;
- (void)setTrackingOffStatus ;



- (MAVSDKTrackingServerResult)respondTrackingPointCommand :(MAVSDKTrackingServerCommandAnswer) commandAnswer;
- (MAVSDKTrackingServerResult)respondTrackingRectangleCommand :(MAVSDKTrackingServerCommandAnswer) commandAnswer;
- (MAVSDKTrackingServerResult)respondTrackingOffCommand :(MAVSDKTrackingServerCommandAnswer) commandAnswer;


@end

#endif // plugin_tracking_server_h