#import "plugin-mission.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mission/mission.h>

@implementation MAVSDKMission

mavsdk::Mission *mission;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    mission = new mavsdk::Mission(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKMissionResult)uploadMission:(MAVSDKMissionMissionPlan*)missionPlan {
    return (MAVSDKMissionResult)mission->upload_mission(translateToCppMissionPlan(missionPlan));
}
- (MAVSDKMissionResult)cancelMissionUpload {
    return (MAVSDKMissionResult)mission->cancel_mission_upload();
}

- (MAVSDKMissionResult)cancelMissionDownload {
    return (MAVSDKMissionResult)mission->cancel_mission_download();
}
- (MAVSDKMissionResult)startMission {
    return (MAVSDKMissionResult)mission->start_mission();
}
- (MAVSDKMissionResult)pauseMission {
    return (MAVSDKMissionResult)mission->pause_mission();
}
- (MAVSDKMissionResult)clearMission {
    return (MAVSDKMissionResult)mission->clear_mission();
}
- (MAVSDKMissionResult)setCurrentMissionItem:(NSInteger)index {
    return (MAVSDKMissionResult)mission->set_current_mission_item(index);
}



- (MAVSDKMissionResult)setReturnToLaunchAfterMission:(BOOL)enable {
    return (MAVSDKMissionResult)mission->set_return_to_launch_after_mission(enable);
}






MAVSDKMissionMissionItem* translateFromCppMissionItem(mavsdk::Mission::MissionItem mission_item)
{
    MAVSDKMissionMissionItem *obj = [[MAVSDKMissionMissionItem alloc] init];


        
    obj.latitudeDeg = mission_item.latitude_deg;
        
    
        
    obj.longitudeDeg = mission_item.longitude_deg;
        
    
        
    obj.relativeAltitudeM = mission_item.relative_altitude_m;
        
    
        
    obj.speedMS = mission_item.speed_m_s;
        
    
        
    obj.isFlyThrough = mission_item.is_fly_through;
        
    
        
    obj.gimbalPitchDeg = mission_item.gimbal_pitch_deg;
        
    
        
    obj.gimbalYawDeg = mission_item.gimbal_yaw_deg;
        
    
        
    obj.cameraAction = (MAVSDKMissionCameraAction)mission_item.camera_action;
        
    
        
    obj.loiterTimeS = mission_item.loiter_time_s;
        
    
        
    obj.cameraPhotoIntervalS = mission_item.camera_photo_interval_s;
        
    
    return obj;
}

mavsdk::Mission::MissionItem translateToCppMissionItem(MAVSDKMissionMissionItem* missionItem)
{
    mavsdk::Mission::MissionItem obj;


        
    obj.latitude_deg = missionItem.latitudeDeg;
        
    
        
    obj.longitude_deg = missionItem.longitudeDeg;
        
    
        
    obj.relative_altitude_m = missionItem.relativeAltitudeM;
        
    
        
    obj.speed_m_s = missionItem.speedMS;
        
    
        
    obj.is_fly_through = missionItem.isFlyThrough;
        
    
        
    obj.gimbal_pitch_deg = missionItem.gimbalPitchDeg;
        
    
        
    obj.gimbal_yaw_deg = missionItem.gimbalYawDeg;
        
    
        
    obj.camera_action = (mavsdk::Mission::MissionItem::CameraAction)(missionItem.cameraAction);
        
    
        
    obj.loiter_time_s = missionItem.loiterTimeS;
        
    
        
    obj.camera_photo_interval_s = missionItem.cameraPhotoIntervalS;
        
    
    return obj;
}



MAVSDKMissionMissionPlan* translateFromCppMissionPlan(mavsdk::Mission::MissionPlan mission_plan)
{
    MAVSDKMissionMissionPlan *obj = [[MAVSDKMissionMissionPlan alloc] init];


        
            for (const auto& elem : mission_plan.mission_items) {
                [obj.missionItems addObject:translateFromCppMissionItem(elem)];
            }
        
    
    return obj;
}

mavsdk::Mission::MissionPlan translateToCppMissionPlan(MAVSDKMissionMissionPlan* missionPlan)
{
    mavsdk::Mission::MissionPlan obj;


        
            for (MAVSDKMissionMissionItem *elem in missionPlan.missionItems) {
                obj.mission_items.push_back(translateToCppMissionItem(elem));
            }
        
    
    return obj;
}



MAVSDKMissionMissionProgress* translateFromCppMissionProgress(mavsdk::Mission::MissionProgress mission_progress)
{
    MAVSDKMissionMissionProgress *obj = [[MAVSDKMissionMissionProgress alloc] init];


        
    obj.current = mission_progress.current;
        
    
        
    obj.total = mission_progress.total;
        
    
    return obj;
}

mavsdk::Mission::MissionProgress translateToCppMissionProgress(MAVSDKMissionMissionProgress* missionProgress)
{
    mavsdk::Mission::MissionProgress obj;


        
    obj.current = missionProgress.current;
        
    
        
    obj.total = missionProgress.total;
        
    
    return obj;
}






@end