#ifndef plugin_log_files_h
#define plugin_log_files_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKLogFilesProgressData : NSObject


@property (nonatomic, assign) float progress; 


@end




@interface MAVSDKLogFilesEntry : NSObject


@property (nonatomic, assign) UInt32 id; 
@property (nonatomic, strong) NSString *date;
@property (nonatomic, assign) UInt32 sizeBytes; 


@end






typedef NS_ENUM(NSInteger, MAVSDKLogFilesResult)  {
    MAVSDKLogFilesResultUnknown,
    MAVSDKLogFilesResultSuccess,
    MAVSDKLogFilesResultNext,
    MAVSDKLogFilesResultNoLogfiles,
    MAVSDKLogFilesResultTimeout,
    MAVSDKLogFilesResultInvalidArgument,
    MAVSDKLogFilesResultFileOpenFailed,
};



@interface MAVSDKLogFiles : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;




@end

#endif // plugin_log_files_h