#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/calibration/calibration.h>







typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNext,
    MVSResultFailed,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultBusy,
    MVSResultCommandDenied,
    MVSResultTimeout,
    MVSResultCancelled,
    MVSResultFailedArmed,
};



@interface MVSProgressData : NSObject



@property (nonatomic, assign) BOOL hasProgress;


@property (nonatomic, assign) float progress;


@property (nonatomic, assign) BOOL hasStatusText;


@property (nonatomic, strong) NSString *statusText;



@end




@interface MVSCalibration : NSObject






- (void)cancel
;


@end