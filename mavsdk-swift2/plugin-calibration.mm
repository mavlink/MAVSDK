#import "plugin-calibration.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/calibration/calibration.h>








MAVSDKCalibrationProgressData* translateFromCppProgressData(mavsdk::Calibration::ProgressData progress_data)
{
    MAVSDKCalibrationProgressData *obj = [[MAVSDKCalibrationProgressData alloc] init];


        
    obj.hasProgress = progress_data.has_progress;
        
    
        
    obj.progress = progress_data.progress;
        
    
        
    obj.hasStatusText = progress_data.has_status_text;
        
    
        
    [NSString stringWithCString:progress_data.status_text.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Calibration::ProgressData translateToCppProgressData(MAVSDKCalibrationProgressData* progressData)
{
    mavsdk::Calibration::ProgressData obj;


        
    obj.has_progress = progressData.hasProgress;
        
    
        
    obj.progress = progressData.progress;
        
    
        
    obj.has_status_text = progressData.hasStatusText;
        
    
        
    obj.status_text = [progressData.statusText UTF8String];
        
    
    return obj;
}



@implementation MAVSDKCalibrationProgressData
@end




@implementation MAVSDKCalibration

mavsdk::Calibration *calibration;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    calibration = new mavsdk::Calibration(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}






- (void)cancel {
    calibration->cancel();
}


@end

