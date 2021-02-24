#ifndef plugin_geofence_h
#define plugin_geofence_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKGeofencePoint : NSObject


@property (nonatomic, assign) double latitudeDeg; 
@property (nonatomic, assign) double longitudeDeg; 


@end





@interface MAVSDKGeofencePolygon : NSObject



typedef NS_ENUM(NSInteger, MAVSDKGeofenceFenceType)  {
    MAVSDKGeofenceFenceTypeInclusion,
    MAVSDKGeofenceFenceTypeExclusion,
};

@property (nonatomic, strong) NSMutableArray *points;
@property (nonatomic, assign) MAVSDKGeofenceFenceType fenceType;


@end






typedef NS_ENUM(NSInteger, MAVSDKGeofenceResult)  {
    MAVSDKGeofenceResultUnknown,
    MAVSDKGeofenceResultSuccess,
    MAVSDKGeofenceResultError,
    MAVSDKGeofenceResultTooManyGeofenceItems,
    MAVSDKGeofenceResultBusy,
    MAVSDKGeofenceResultTimeout,
    MAVSDKGeofenceResultInvalidArgument,
};



@interface MAVSDKGeofence : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKGeofenceResult)uploadGeofence :( MAVSDKGeofencePolygon*) polygons;


@end

#endif // plugin_geofence_h