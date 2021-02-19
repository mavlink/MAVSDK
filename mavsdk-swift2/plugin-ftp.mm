#import "plugin-ftp.h"

@implementation MVSFtp





- (MVSResult)createDirectory:(NSString*)remoteDir {
    return MVSResultUnknown;
}
- (MVSResult)removeDirectory:(NSString*)remoteDir {
    return MVSResultUnknown;
}
- (MVSResult)removeFile:(NSString*)remoteFilePath {
    return MVSResultUnknown;
}
- (MVSResult)rename:(NSString*)remoteFromPath :(NSString*)remoteToPath {
    return MVSResultUnknown;
}

- (MVSResult)setRootDirectory:(NSString*)rootDir {
    return MVSResultUnknown;
}
- (MVSResult)setTargetCompid:(UInt32)compid {
    return MVSResultUnknown;
}











@end