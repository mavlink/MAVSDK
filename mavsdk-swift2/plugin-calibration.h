#ifndef plugin_calibration_h
#define plugin_calibration_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;







typedef NS_ENUM(NSInteger, MAVSDKCalibrationResult)  {
    MAVSDKCalibrationResultUnknown,
    MAVSDKCalibrationResultSuccess,
    MAVSDKCalibrationResultNext,
    MAVSDKCalibrationResultFailed,
    MAVSDKCalibrationResultNoSystem,
    MAVSDKCalibrationResultConnectionError,
    MAVSDKCalibrationResultBusy,
    MAVSDKCalibrationResultCommandDenied,
    MAVSDKCalibrationResultTimeout,
    MAVSDKCalibrationResultCancelled,
    MAVSDKCalibrationResultFailedArmed,
};



@interface MAVSDKCalibrationProgressData : NSObject


@property (nonatomic, assign) BOOL hasProgress; 
@property (nonatomic, assign) float progress; 
@property (nonatomic, assign) BOOL hasStatusText; 
@property (nonatomic, strong) NSString *statusText;


@end




@interface MAVSDKCalibration : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;





- (void)cancel ;


@end

#endif // plugin_calibration_h