#ifndef plugin_param_h
#define plugin_param_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKParamIntParam : NSObject


@property (nonatomic, strong) NSString *name;
@property (nonatomic, assign) SInt32 value; 


@end




@interface MAVSDKParamFloatParam : NSObject


@property (nonatomic, strong) NSString *name;
@property (nonatomic, assign) float value; 


@end




@interface MAVSDKParamAllParams : NSObject


@property (nonatomic, strong) NSMutableArray *intParams;
@property (nonatomic, strong) NSMutableArray *floatParams;


@end






typedef NS_ENUM(NSInteger, MAVSDKParamResult)  {
    MAVSDKParamResultUnknown,
    MAVSDKParamResultSuccess,
    MAVSDKParamResultTimeout,
    MAVSDKParamResultConnectionError,
    MAVSDKParamResultWrongType,
    MAVSDKParamResultParamNameTooLong,
};



@interface MAVSDKParam : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;

- (MAVSDKParamResult)setParamInt :( NSString*) name :( SInt32) value;

- (MAVSDKParamResult)setParamFloat :( NSString*) name :( float) value;



@end

#endif // plugin_param_h