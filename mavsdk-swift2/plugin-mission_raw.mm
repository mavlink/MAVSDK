#import "plugin-mission_raw.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mission_raw/mission_raw.h>

@implementation MAVSDKMissionRaw

mavsdk::MissionRaw *missionRaw;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    missionRaw = new mavsdk::MissionRaw(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKMissionRawResult)uploadMission:(MAVSDKMissionRawMissionItem*)missionItems {
    //return (MAVSDKMissionRawResult)mission_raw->upload_mission(translateToCppMissionItems(missionItems));
    return MAVSDKMissionRawResultUnknown;
}
- (MAVSDKMissionRawResult)cancelMissionUpload {
    //return (MAVSDKMissionRawResult)mission_raw->cancel_mission_upload();
    return MAVSDKMissionRawResultUnknown;

}

- (MAVSDKMissionRawResult)cancelMissionDownload {
    //return (MAVSDKMissionRawResult)mission_raw->cancel_mission_download();
    return MAVSDKMissionRawResultUnknown;

}
- (MAVSDKMissionRawResult)startMission {
    //return (MAVSDKMissionRawResult)mission_raw->start_mission();
    return MAVSDKMissionRawResultUnknown;

}
- (MAVSDKMissionRawResult)pauseMission {
    //return (MAVSDKMissionRawResult)mission_raw->pause_mission();
    return MAVSDKMissionRawResultUnknown;

}
- (MAVSDKMissionRawResult)clearMission {
    //return (MAVSDKMissionRawResult)mission_raw->clear_mission();
    return MAVSDKMissionRawResultUnknown;

}
- (MAVSDKMissionRawResult)setCurrentMissionItem:(NSInteger)index {
    //return (MAVSDKMissionRawResult)mission_raw->set_current_mission_item(index);
    return MAVSDKMissionRawResultUnknown;

}







MAVSDKMissionRawMissionProgress* translateFromCppMissionProgress(mavsdk::MissionRaw::MissionProgress mission_progress)
{
    MAVSDKMissionRawMissionProgress *obj = [[MAVSDKMissionRawMissionProgress alloc] init];


        
    obj.current = mission_progress.current;
        
    
        
    obj.total = mission_progress.total;
        
    
    return obj;
}

mavsdk::MissionRaw::MissionProgress translateToCppMissionProgress(MAVSDKMissionRawMissionProgress* missionProgress)
{
    mavsdk::MissionRaw::MissionProgress obj;


        
    obj.current = missionProgress.current;
        
    
        
    obj.total = missionProgress.total;
        
    
    return obj;
}



MAVSDKMissionRawMissionItem* translateFromCppMissionItem(mavsdk::MissionRaw::MissionItem mission_item)
{
    MAVSDKMissionRawMissionItem *obj = [[MAVSDKMissionRawMissionItem alloc] init];


        
    obj.seq = mission_item.seq;
        
    
        
    obj.frame = mission_item.frame;
        
    
        
    obj.command = mission_item.command;
        
    
        
    obj.current = mission_item.current;
        
    
        
    obj.autocontinue = mission_item.autocontinue;
        
    
        
    obj.param1 = mission_item.param1;
        
    
        
    obj.param2 = mission_item.param2;
        
    
        
    obj.param3 = mission_item.param3;
        
    
        
    obj.param4 = mission_item.param4;
        
    
        
    obj.x = mission_item.x;
        
    
        
    obj.y = mission_item.y;
        
    
        
    obj.z = mission_item.z;
        
    
        
    obj.missionType = mission_item.mission_type;
        
    
    return obj;
}

mavsdk::MissionRaw::MissionItem translateToCppMissionItem(MAVSDKMissionRawMissionItem* missionItem)
{
    mavsdk::MissionRaw::MissionItem obj;


        
    obj.seq = missionItem.seq;
        
    
        
    obj.frame = missionItem.frame;
        
    
        
    obj.command = missionItem.command;
        
    
        
    obj.current = missionItem.current;
        
    
        
    obj.autocontinue = missionItem.autocontinue;
        
    
        
    obj.param1 = missionItem.param1;
        
    
        
    obj.param2 = missionItem.param2;
        
    
        
    obj.param3 = missionItem.param3;
        
    
        
    obj.param4 = missionItem.param4;
        
    
        
    obj.x = missionItem.x;
        
    
        
    obj.y = missionItem.y;
        
    
        
    obj.z = missionItem.z;
        
    
        
    obj.mission_type = missionItem.missionType;
        
    
    return obj;
}






@end
