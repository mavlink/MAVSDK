#ifndef plugin_mission_raw_h
#define plugin_mission_raw_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKMissionRawMissionProgress : NSObject


@property (nonatomic, assign) SInt32 current; 
@property (nonatomic, assign) SInt32 total; 


@end




@interface MAVSDKMissionRawMissionItem : NSObject


@property (nonatomic, assign) UInt32 seq; 
@property (nonatomic, assign) UInt32 frame; 
@property (nonatomic, assign) UInt32 command; 
@property (nonatomic, assign) UInt32 current; 
@property (nonatomic, assign) UInt32 autocontinue; 
@property (nonatomic, assign) float param1; 
@property (nonatomic, assign) float param2; 
@property (nonatomic, assign) float param3; 
@property (nonatomic, assign) float param4; 
@property (nonatomic, assign) SInt32 x; 
@property (nonatomic, assign) SInt32 y; 
@property (nonatomic, assign) float z; 
@property (nonatomic, assign) UInt32 missionType; 


@end






typedef NS_ENUM(NSInteger, MAVSDKMissionRawResult)  {
    MAVSDKMissionRawResultUnknown,
    MAVSDKMissionRawResultSuccess,
    MAVSDKMissionRawResultError,
    MAVSDKMissionRawResultTooManyMissionItems,
    MAVSDKMissionRawResultBusy,
    MAVSDKMissionRawResultTimeout,
    MAVSDKMissionRawResultInvalidArgument,
    MAVSDKMissionRawResultUnsupported,
    MAVSDKMissionRawResultNoMissionAvailable,
    MAVSDKMissionRawResultTransferCancelled,
};



@interface MAVSDKMissionRaw : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKMissionRawResult)uploadMission :( MAVSDKMissionRawMissionItem*) missionItems;
- (MAVSDKMissionRawResult)cancelMissionUpload ;

- (MAVSDKMissionRawResult)cancelMissionDownload ;
- (MAVSDKMissionRawResult)startMission ;
- (MAVSDKMissionRawResult)pauseMission ;
- (MAVSDKMissionRawResult)clearMission ;
- (MAVSDKMissionRawResult)setCurrentMissionItem :( SInt32) index;




@end

#endif // plugin_mission_raw_h