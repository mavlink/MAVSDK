#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/geofence/geofence.h>





@interface MVSPoint : NSObject



@property (nonatomic, assign) double latitudeDeg;


@property (nonatomic, assign) double longitudeDeg;



@end





@interface MVSPolygon : NSObject



typedef NS_ENUM(NSInteger, MVSFenceType)  {
    MVSFenceTypeInclusion,
    MVSFenceTypeExclusion,
};


@property (nonatomic, strong) NSMutableArray *points;


@property (nonatomic, assign) MVSFenceType fenceType;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultError,
    MVSResultTooManyGeofenceItems,
    MVSResultBusy,
    MVSResultTimeout,
    MVSResultInvalidArgument,
};



@interface MVSGeofence : NSObject

- (MVSResult)uploadGeofence
:(
MVSPolygon*)
polygons;


@end