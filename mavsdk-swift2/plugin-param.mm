#import "plugin-param.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>

@implementation MAVSDKParam

mavsdk::Param *param;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    param = new mavsdk::Param(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}


- (MAVSDKParamResult)setParamInt:(NSString*)name :(NSInteger)value {
    return (MAVSDKParamResult)param->set_param_int([name UTF8String], value);
}

- (MAVSDKParamResult)setParamFloat:(NSString*)name :(float)value {
    return (MAVSDKParamResult)param->set_param_float([name UTF8String], value);
}






MAVSDKParamIntParam* translateFromCppIntParam(mavsdk::Param::IntParam int_param)
{
    MAVSDKParamIntParam *obj = [[MAVSDKParamIntParam alloc] init];


        
    [NSString stringWithCString:int_param.name.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    obj.value = int_param.value;
        
    
    return obj;
}

mavsdk::Param::IntParam translateToCppIntParam(MAVSDKParamIntParam* intParam)
{
    mavsdk::Param::IntParam obj;


        
    obj.name = [intParam.name UTF8String];
        
    
        
    obj.value = intParam.value;
        
    
    return obj;
}



MAVSDKParamFloatParam* translateFromCppFloatParam(mavsdk::Param::FloatParam float_param)
{
    MAVSDKParamFloatParam *obj = [[MAVSDKParamFloatParam alloc] init];


        
    [NSString stringWithCString:float_param.name.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    obj.value = float_param.value;
        
    
    return obj;
}

mavsdk::Param::FloatParam translateToCppFloatParam(MAVSDKParamFloatParam* floatParam)
{
    mavsdk::Param::FloatParam obj;


        
    obj.name = [floatParam.name UTF8String];
        
    
        
    obj.value = floatParam.value;
        
    
    return obj;
}



MAVSDKParamAllParams* translateFromCppAllParams(mavsdk::Param::AllParams all_params)
{
    MAVSDKParamAllParams *obj = [[MAVSDKParamAllParams alloc] init];


        
            for (const auto& elem : all_params.int_params) {
                [obj.intParams addObject:translateFromCppIntParam(elem)];
            }
        
    
        
            for (const auto& elem : all_params.float_params) {
                [obj.floatParams addObject:translateFromCppFloatParam(elem)];
            }
        
    
    return obj;
}

mavsdk::Param::AllParams translateToCppAllParams(MAVSDKParamAllParams* allParams)
{
    mavsdk::Param::AllParams obj;


        
            for (MAVSDKParamIntParam *elem in allParams.intParams) {
                obj.int_params.push_back(translateToCppIntParam(elem));
            }
        
    
        
            for (MAVSDKParamFloatParam *elem in allParams.floatParams) {
                obj.float_params.push_back(translateToCppFloatParam(elem));
            }
        
    
    return obj;
}






@end