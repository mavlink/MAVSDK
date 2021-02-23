#ifndef plugin_shell_h
#define plugin_shell_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;







typedef NS_ENUM(NSInteger, MAVSDKShellResult)  {
    MAVSDKShellResultUnknown,
    MAVSDKShellResultSuccess,
    MAVSDKShellResultNoSystem,
    MAVSDKShellResultConnectionError,
    MAVSDKShellResultNoResponse,
    MAVSDKShellResultBusy,
};



@interface MAVSDKShell : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKShellResult)send :( NSString*) command;



@end

#endif // plugin_shell_h