#import "plugin-mission.h"

@implementation MVSMission

- (MVSResult)uploadMission:(MVSMissionPlan*)missionPlan {
    return MVSResultUnknown;
}
- (MVSResult)cancelMissionUpload {
    return MVSResultUnknown;
}

- (MVSResult)cancelMissionDownload {
    return MVSResultUnknown;
}
- (MVSResult)startMission {
    return MVSResultUnknown;
}
- (MVSResult)pauseMission {
    return MVSResultUnknown;
}
- (MVSResult)clearMission {
    return MVSResultUnknown;
}
- (MVSResult)setCurrentMissionItem:(NSInteger)index {
    return MVSResultUnknown;
}



- (MVSResult)setReturnToLaunchAfterMission:(BOOL)enable {
    return MVSResultUnknown;
}















@end