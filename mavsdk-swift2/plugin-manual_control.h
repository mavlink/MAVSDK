#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/manual_control/manual_control.h>







typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultBusy,
    MVSResultCommandDenied,
    MVSResultTimeout,
    MVSResultInputOutOfRange,
    MVSResultInputNotSet,
};



@interface MVSManualControl : NSObject

- (MVSResult)startPositionControl
;
- (MVSResult)startAltitudeControl
;
- (MVSResult)setManualControlInput
:(
float)
x :(
float)
y :(
float)
z :(
float)
r;


@end