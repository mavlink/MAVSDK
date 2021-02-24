#import "plugin-log_files.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/log_files/log_files.h>




MAVSDKLogFilesProgressData* translateFromCppProgressData(mavsdk::LogFiles::ProgressData progress_data)
{
    MAVSDKLogFilesProgressData *obj = [[MAVSDKLogFilesProgressData alloc] init];


        
    obj.progress = progress_data.progress;
        
    
    return obj;
}

mavsdk::LogFiles::ProgressData translateToCppProgressData(MAVSDKLogFilesProgressData* progressData)
{
    mavsdk::LogFiles::ProgressData obj;


        
    obj.progress = progressData.progress;
        
    
    return obj;
}



@implementation MAVSDKLogFilesProgressData
@end



MAVSDKLogFilesEntry* translateFromCppEntry(mavsdk::LogFiles::Entry entry)
{
    MAVSDKLogFilesEntry *obj = [[MAVSDKLogFilesEntry alloc] init];


        
    obj.id = entry.id;
        
    
        
    [NSString stringWithCString:entry.date.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    obj.sizeBytes = entry.size_bytes;
        
    
    return obj;
}

mavsdk::LogFiles::Entry translateToCppEntry(MAVSDKLogFilesEntry* entry)
{
    mavsdk::LogFiles::Entry obj;


        
    obj.id = entry.id;
        
    
        
    obj.date = [entry.date UTF8String];
        
    
        
    obj.size_bytes = entry.sizeBytes;
        
    
    return obj;
}



@implementation MAVSDKLogFilesEntry
@end








@implementation MAVSDKLogFiles

mavsdk::LogFiles *log_files;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    log_files = new mavsdk::LogFiles(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}





@end

