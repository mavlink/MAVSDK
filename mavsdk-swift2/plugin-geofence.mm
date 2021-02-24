#import "plugin-geofence.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/geofence/geofence.h>




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



@implementation MAVSDKGeofencePoint
@end




MAVSDKGeofenceFenceType translateFromCppFenceType(mavsdk::Geofence::Polygon::FenceType fence_type)
{
    switch (fence_type) {
        default:
            NSLog(@"Unknown fence_type enum value: %d", static_cast<int>(fence_type));
        // FALLTHROUGH
        case mavsdk::Geofence::Polygon::FenceType::Inclusion:
            return MAVSDKGeofenceFenceTypeInclusion;
        case mavsdk::Geofence::Polygon::FenceType::Exclusion:
            return MAVSDKGeofenceFenceTypeExclusion;
    }
}

mavsdk::Geofence::Polygon::FenceType translateToCppFenceType(MAVSDKGeofenceFenceType fenceType)
{
    switch (fenceType) {
        default:
            NSLog(@"Unknown FenceType enum value: %d", static_cast<int>(fenceType));
        // FALLTHROUGH
        case MAVSDKGeofenceFenceTypeInclusion:
            return mavsdk::Geofence::Polygon::FenceType::Inclusion;
        case MAVSDKGeofenceFenceTypeExclusion:
            return mavsdk::Geofence::Polygon::FenceType::Exclusion;
    }
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
            
        
    
        
    obj.fence_type = translateToCppFenceType(polygon.fenceType);
        
    
    return obj;
}



@implementation MAVSDKGeofencePolygon
@end








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
    return (MAVSDKGeofenceResult)geofence->upload_geofence(translateToCppPolygons(polygons));
}


@end

