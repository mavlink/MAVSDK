#import "plugin-ftp.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp/ftp.h>

@implementation MAVSDKFtp

mavsdk::Ftp *ftp;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    ftp = new mavsdk::Ftp(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}





- (MAVSDKFtpResult)createDirectory:(NSString*)remoteDir {
    return (MAVSDKFtpResult)ftp->create_directory([remoteDir UTF8String]);
}
- (MAVSDKFtpResult)removeDirectory:(NSString*)remoteDir {
    return (MAVSDKFtpResult)ftp->remove_directory([remoteDir UTF8String]);
}
- (MAVSDKFtpResult)removeFile:(NSString*)remoteFilePath {
    return (MAVSDKFtpResult)ftp->remove_file([remoteFilePath UTF8String]);
}
- (MAVSDKFtpResult)rename:(NSString*)remoteFromPath :(NSString*)remoteToPath {
    return (MAVSDKFtpResult)ftp->rename([remoteFromPath UTF8String], [remoteToPath UTF8String]);
}

- (MAVSDKFtpResult)setRootDirectory:(NSString*)rootDir {
    return (MAVSDKFtpResult)ftp->set_root_directory([rootDir UTF8String]);
}
- (MAVSDKFtpResult)setTargetCompid:(UInt32)compid {
    return (MAVSDKFtpResult)ftp->set_target_compid(compid);
}






MAVSDKFtpProgressData* translateFromCppProgressData(mavsdk::Ftp::ProgressData progress_data)
{
    MAVSDKFtpProgressData *obj = [[MAVSDKFtpProgressData alloc] init];


        
    obj.bytesTransferred = progress_data.bytes_transferred;
        
    
        
    obj.totalBytes = progress_data.total_bytes;
        
    
    return obj;
}

mavsdk::Ftp::ProgressData translateToCppProgressData(MAVSDKFtpProgressData* progressData)
{
    mavsdk::Ftp::ProgressData obj;


        
    obj.bytes_transferred = progressData.bytesTransferred;
        
    
        
    obj.total_bytes = progressData.totalBytes;
        
    
    return obj;
}






@end