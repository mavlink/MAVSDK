#import "plugin-geofence.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/geofence/geofence.h>

@implementation MAVSDKGeofence

mavsdk::Geofence *geofence;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    geofence = new mavsdk::Geofence(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKGeofenceResult)uploadGeofence:(MAVSDKGeofencePolygon*)polygons {
    //return (MAVSDKGeofenceResult)geofence->upload_geofence(translateToCppPolygons(polygons));
    return MAVSDKGeofenceResultUnknown;
}





MAVSDKGeofencePoint* translateFromCppPoint(mavsdk::Geofence::Point point)
{
    MAVSDKGeofencePoint *obj = [[MAVSDKGeofencePoint alloc] init];


        
    obj.latitudeDeg = point.latitude_deg;
        
    
        
    obj.longitudeDeg = point.longitude_deg;
        
    
    return obj;
}

mavsdk::Geofence::Point translateToCppPoint(MAVSDKGeofencePoint* point)
{
    mavsdk::Geofence::Point obj;


        
    obj.latitude_deg = point.latitudeDeg;
        
    
        
    obj.longitude_deg = point.longitudeDeg;
        
    
    return obj;
}



MAVSDKGeofencePolygon* translateFromCppPolygon(mavsdk::Geofence::Polygon polygon)
{
    MAVSDKGeofencePolygon *obj = [[MAVSDKGeofencePolygon alloc] init];


        
            for (const auto& elem : polygon.points) {
                [obj.points addObject:translateFromCppPoint(elem)];
            }
        
    
        
    obj.fenceType = (MAVSDKGeofenceFenceType)polygon.fence_type;
        
    
    return obj;
}

mavsdk::Geofence::Polygon translateToCppPolygon(MAVSDKGeofencePolygon* polygon)
{
    mavsdk::Geofence::Polygon obj;


        
            for (MAVSDKGeofencePoint *elem in polygon.points) {
                obj.points.push_back(translateToCppPoint(elem));
            }
        
    
        
    obj.fence_type = (mavsdk::Geofence::Polygon::FenceType)(polygon.fenceType);
        
    
    return obj;
}






@end
