#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp/ftp.h>





@interface MVSProgressData : NSObject



@property (nonatomic, assign) UInt32 bytesTransferred;


@property (nonatomic, assign) UInt32 totalBytes;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNext,
    MVSResultTimeout,
    MVSResultBusy,
    MVSResultFileIoError,
    MVSResultFileExists,
    MVSResultFileDoesNotExist,
    MVSResultFileProtected,
    MVSResultInvalidParameter,
    MVSResultUnsupported,
    MVSResultProtocolError,
};



@interface MVSFtp : NSObject





- (MVSResult)createDirectory
:(
NSString*)
remoteDir;
- (MVSResult)removeDirectory
:(
NSString*)
remoteDir;
- (MVSResult)removeFile
:(
NSString*)
remoteFilePath;
- (MVSResult)rename
:(
NSString*)
remoteFromPath :(
NSString*)
remoteToPath;

- (MVSResult)setRootDirectory
:(
NSString*)
rootDir;
- (MVSResult)setTargetCompid
:(
UInt32)
compid;



@end