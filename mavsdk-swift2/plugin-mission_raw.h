#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mission_raw/mission_raw.h>





@interface MVSMissionProgress : NSObject



@property (nonatomic, assign) NSInteger current;


@property (nonatomic, assign) NSInteger total;



@end




@interface MVSMissionItem : NSObject



@property (nonatomic, assign) UInt32 seq;


@property (nonatomic, assign) UInt32 frame;


@property (nonatomic, assign) UInt32 command;


@property (nonatomic, assign) UInt32 current;


@property (nonatomic, assign) UInt32 autocontinue;


@property (nonatomic, assign) float param1;


@property (nonatomic, assign) float param2;


@property (nonatomic, assign) float param3;


@property (nonatomic, assign) float param4;


@property (nonatomic, assign) NSInteger x;


@property (nonatomic, assign) NSInteger y;


@property (nonatomic, assign) float z;


@property (nonatomic, assign) UInt32 missionType;



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
    MVSResultTransferCancelled,
};



@interface MVSMissionRaw : NSObject

- (MVSResult)uploadMission
:(
MVSMissionItem*)
missionItems;
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




@end