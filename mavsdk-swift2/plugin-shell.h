#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/shell/shell.h>







typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultNoResponse,
    MVSResultBusy,
};



@interface MVSShell : NSObject

- (MVSResult)send
:(
NSString*)
command;



@end