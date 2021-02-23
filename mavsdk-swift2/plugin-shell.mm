#import "plugin-shell.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/shell/shell.h>

@implementation MAVSDKShell

mavsdk::Shell *shell;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    shell = new mavsdk::Shell(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKShellResult)send:(NSString*)command {
    return (MAVSDKShellResult)shell->send([command UTF8String]);
}









@end