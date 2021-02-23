#ifndef plugin_mission_h
#define plugin_mission_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;






@interface MAVSDKMissionMissionItem : NSObject



typedef NS_ENUM(NSInteger, MAVSDKMissionCameraAction)  {
    MAVSDKMissionCameraActionNone,
    MAVSDKMissionCameraActionTakePhoto,
    MAVSDKMissionCameraActionStartPhotoInterval,
    MAVSDKMissionCameraActionStopPhotoInterval,
    MAVSDKMissionCameraActionStartVideo,
    MAVSDKMissionCameraActionStopVideo,
};

@property (nonatomic, assign) double latitudeDeg;
@property (nonatomic, assign) double longitudeDeg;
@property (nonatomic, assign) float relativeAltitudeM;
@property (nonatomic, assign) float speedMS;
@property (nonatomic, assign) BOOL isFlyThrough;
@property (nonatomic, assign) float gimbalPitchDeg;
@property (nonatomic, assign) float gimbalYawDeg;
@property (nonatomic, assign) MAVSDKMissionCameraAction cameraAction;
@property (nonatomic, assign) float loiterTimeS;
@property (nonatomic, assign) double cameraPhotoIntervalS;


@end




@interface MAVSDKMissionMissionPlan : NSObject


@property (nonatomic, strong) NSMutableArray *missionItems; 


@end




@interface MAVSDKMissionMissionProgress : NSObject


@property (nonatomic, assign) NSInteger current;
@property (nonatomic, assign) NSInteger total;


@end






typedef NS_ENUM(NSInteger, MAVSDKMissionResult)  {
    MAVSDKMissionResultUnknown,
    MAVSDKMissionResultSuccess,
    MAVSDKMissionResultError,
    MAVSDKMissionResultTooManyMissionItems,
    MAVSDKMissionResultBusy,
    MAVSDKMissionResultTimeout,
    MAVSDKMissionResultInvalidArgument,
    MAVSDKMissionResultUnsupported,
    MAVSDKMissionResultNoMissionAvailable,
    MAVSDKMissionResultFailedToOpenQgcPlan,
    MAVSDKMissionResultFailedToParseQgcPlan,
    MAVSDKMissionResultUnsupportedMissionCmd,
    MAVSDKMissionResultTransferCancelled,
};



@interface MAVSDKMission : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKMissionResult)uploadMission :( MAVSDKMissionMissionPlan*) missionPlan;
- (MAVSDKMissionResult)cancelMissionUpload ;

- (MAVSDKMissionResult)cancelMissionDownload ;
- (MAVSDKMissionResult)startMission ;
- (MAVSDKMissionResult)pauseMission ;
- (MAVSDKMissionResult)clearMission ;
- (MAVSDKMissionResult)setCurrentMissionItem :( NSInteger) index;



- (MAVSDKMissionResult)setReturnToLaunchAfterMission :( BOOL) enable;



@end

#endif // plugin_mission_h