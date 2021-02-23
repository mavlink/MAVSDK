#ifndef plugin_ftp_h
#define plugin_ftp_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKFtpProgressData : NSObject


@property (nonatomic, assign) UInt32 bytesTransferred;
@property (nonatomic, assign) UInt32 totalBytes;


@end






typedef NS_ENUM(NSInteger, MAVSDKFtpResult)  {
    MAVSDKFtpResultUnknown,
    MAVSDKFtpResultSuccess,
    MAVSDKFtpResultNext,
    MAVSDKFtpResultTimeout,
    MAVSDKFtpResultBusy,
    MAVSDKFtpResultFileIoError,
    MAVSDKFtpResultFileExists,
    MAVSDKFtpResultFileDoesNotExist,
    MAVSDKFtpResultFileProtected,
    MAVSDKFtpResultInvalidParameter,
    MAVSDKFtpResultUnsupported,
    MAVSDKFtpResultProtocolError,
};



@interface MAVSDKFtp : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;




- (MAVSDKFtpResult)createDirectory :( NSString*) remoteDir;
- (MAVSDKFtpResult)removeDirectory :( NSString*) remoteDir;
- (MAVSDKFtpResult)removeFile :( NSString*) remoteFilePath;
- (MAVSDKFtpResult)rename :( NSString*) remoteFromPath :( NSString*) remoteToPath;

- (MAVSDKFtpResult)setRootDirectory :( NSString*) rootDir;
- (MAVSDKFtpResult)setTargetCompid :( UInt32) compid;



@end

#endif // plugin_ftp_h