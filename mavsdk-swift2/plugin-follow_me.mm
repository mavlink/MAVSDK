#import "plugin-follow_me.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/follow_me/follow_me.h>

@implementation MAVSDKFollowMe

mavsdk::FollowMe *followMe;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    followMe = new mavsdk::FollowMe(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}


- (MAVSDKFollowMeResult)setConfig:(MAVSDKFollowMeConfig*)config {
    //return (MAVSDKFollowMeResult)follow_me->set_config(translateToCppConfig(config));
    return MAVSDKFollowMeResultUnknown;
}

- (MAVSDKFollowMeResult)setTargetLocation:(MAVSDKFollowMeTargetLocation*)location {
    //return (MAVSDKFollowMeResult)follow_me->set_target_location(translateToCppLocation(location));
    return MAVSDKFollowMeResultUnknown;

}

- (MAVSDKFollowMeResult)start {
    //return (MAVSDKFollowMeResult)follow_me->start();
    return MAVSDKFollowMeResultUnknown;

}
- (MAVSDKFollowMeResult)stop {
    //return (MAVSDKFollowMeResult)follow_me->stop();
    return MAVSDKFollowMeResultUnknown;

}





MAVSDKFollowMeConfig* translateFromCppConfig(mavsdk::FollowMe::Config config)
{
    MAVSDKFollowMeConfig *obj = [[MAVSDKFollowMeConfig alloc] init];


        
    obj.minHeightM = config.min_height_m;
        
    
        
    obj.followDistanceM = config.follow_distance_m;
        
    
        
    obj.followDirection = (MAVSDKFollowMeFollowDirection)config.follow_direction;
        
    
        
    obj.responsiveness = config.responsiveness;
        
    
    return obj;
}

mavsdk::FollowMe::Config translateToCppConfig(MAVSDKFollowMeConfig* config)
{
    mavsdk::FollowMe::Config obj;


        
    obj.min_height_m = config.minHeightM;
        
    
        
    obj.follow_distance_m = config.followDistanceM;
        
    
        
    obj.follow_direction = (mavsdk::FollowMe::Config::FollowDirection)(config.followDirection);
        
    
        
    obj.responsiveness = config.responsiveness;
        
    
    return obj;
}



MAVSDKFollowMeTargetLocation* translateFromCppTargetLocation(mavsdk::FollowMe::TargetLocation target_location)
{
    MAVSDKFollowMeTargetLocation *obj = [[MAVSDKFollowMeTargetLocation alloc] init];


        
    obj.latitudeDeg = target_location.latitude_deg;
        
    
        
    obj.longitudeDeg = target_location.longitude_deg;
        
    
        
    obj.absoluteAltitudeM = target_location.absolute_altitude_m;
        
    
        
    obj.velocityXMS = target_location.velocity_x_m_s;
        
    
        
    obj.velocityYMS = target_location.velocity_y_m_s;
        
    
        
    obj.velocityZMS = target_location.velocity_z_m_s;
        
    
    return obj;
}

mavsdk::FollowMe::TargetLocation translateToCppTargetLocation(MAVSDKFollowMeTargetLocation* targetLocation)
{
    mavsdk::FollowMe::TargetLocation obj;


        
    obj.latitude_deg = targetLocation.latitudeDeg;
        
    
        
    obj.longitude_deg = targetLocation.longitudeDeg;
        
    
        
    obj.absolute_altitude_m = targetLocation.absoluteAltitudeM;
        
    
        
    obj.velocity_x_m_s = targetLocation.velocityXMS;
        
    
        
    obj.velocity_y_m_s = targetLocation.velocityYMS;
        
    
        
    obj.velocity_z_m_s = targetLocation.velocityZMS;
        
    
    return obj;
}






@end
