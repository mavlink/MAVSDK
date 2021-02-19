#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mission/mission.h>






@interface MVSMissionItem : NSObject



typedef NS_ENUM(NSInteger, MVSCameraAction)  {
    MVSCameraActionNone,
    MVSCameraActionTakePhoto,
    MVSCameraActionStartPhotoInterval,
    MVSCameraActionStopPhotoInterval,
    MVSCameraActionStartVideo,
    MVSCameraActionStopVideo,
};


@property (nonatomic, assign) double latitudeDeg;


@property (nonatomic, assign) double longitudeDeg;


@property (nonatomic, assign) float relativeAltitudeM;


@property (nonatomic, assign) float speedMS;


@property (nonatomic, assign) BOOL isFlyThrough;


@property (nonatomic, assign) float gimbalPitchDeg;


@property (nonatomic, assign) float gimbalYawDeg;


@property (nonatomic, assign) MVSCameraAction cameraAction;


@property (nonatomic, assign) float loiterTimeS;


@property (nonatomic, assign) double cameraPhotoIntervalS;



@end




@interface MVSMissionPlan : NSObject



@property (nonatomic, strong) NSMutableArray *missionItems;



@end




@interface MVSMissionProgress : NSObject



@property (nonatomic, assign) NSInteger current;


@property (nonatomic, assign) NSInteger total;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultError,
    MVSResultTooManyMissionItems,
    MVSResultBusy,
    MVSResultTimeout,
    MVSResultInvalidArgument,
    MVSResultUnsupported,
    MVSResultNoMissionAvailable,
    MVSResultFailedToOpenQgcPlan,
    MVSResultFailedToParseQgcPlan,
    MVSResultUnsupportedMissionCmd,
    MVSResultTransferCancelled,
};



@interface MVSMission : NSObject

- (MVSResult)uploadMission
:(
MVSMissionPlan*)
missionPlan;
- (MVSResult)cancelMissionUpload
;

- (MVSResult)cancelMissionDownload
;
- (MVSResult)startMission
;
- (MVSResult)pauseMission
;
- (MVSResult)clearMission
;
- (MVSResult)setCurrentMissionItem
:(
NSInteger)
index;



- (MVSResult)setReturnToLaunchAfterMission
:(
BOOL)
enable;



@end