#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/log_files/log_files.h>





@interface MVSProgressData : NSObject



@property (nonatomic, assign) float progress;



@end




@interface MVSEntry : NSObject



@property (nonatomic, assign) UInt32 id;


@property (nonatomic, strong) NSString *date;


@property (nonatomic, assign) UInt32 sizeBytes;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNext,
    MVSResultNoLogfiles,
    MVSResultTimeout,
    MVSResultInvalidArgument,
    MVSResultFileOpenFailed,
};



@interface MVSLogFiles : NSObject





@end