#import "plugin-failure.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/failure/failure.h>

@implementation MAVSDKFailure

mavsdk::Failure *failure;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    failure = new mavsdk::Failure(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKFailureResult)inject:(MAVSDKFailureFailureUnit)failureUnit :(MAVSDKFailureFailureType)failureType :(NSInteger)instance {
    return (MAVSDKFailureResult)failure->inject((mavsdk::Failure::FailureUnit)failureUnit, (mavsdk::Failure::FailureType)failureType, instance);
}












@end