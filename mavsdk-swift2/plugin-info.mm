#import "plugin-info.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/info/info.h>

@implementation MAVSDKInfo

mavsdk::Info *info;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    info = new mavsdk::Info(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}











MAVSDKInfoFlightInfo* translateFromCppFlightInfo(mavsdk::Info::FlightInfo flight_info)
{
    MAVSDKInfoFlightInfo *obj = [[MAVSDKInfoFlightInfo alloc] init];


        
    obj.timeBootMs = flight_info.time_boot_ms;
        
    
        
    obj.flightUid = flight_info.flight_uid;
        
    
    return obj;
}

mavsdk::Info::FlightInfo translateToCppFlightInfo(MAVSDKInfoFlightInfo* flightInfo)
{
    mavsdk::Info::FlightInfo obj;


        
    obj.time_boot_ms = flightInfo.timeBootMs;
        
    
        
    obj.flight_uid = flightInfo.flightUid;
        
    
    return obj;
}



MAVSDKInfoIdentification* translateFromCppIdentification(mavsdk::Info::Identification identification)
{
    MAVSDKInfoIdentification *obj = [[MAVSDKInfoIdentification alloc] init];


        
    [NSString stringWithCString:identification.hardware_uid.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Info::Identification translateToCppIdentification(MAVSDKInfoIdentification* identification)
{
    mavsdk::Info::Identification obj;


        
    obj.hardware_uid = [identification.hardwareUid UTF8String];
        
    
    return obj;
}



MAVSDKInfoProduct* translateFromCppProduct(mavsdk::Info::Product product)
{
    MAVSDKInfoProduct *obj = [[MAVSDKInfoProduct alloc] init];


        
    obj.vendorId = product.vendor_id;
        
    
        
    [NSString stringWithCString:product.vendor_name.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    obj.productId = product.product_id;
        
    
        
    [NSString stringWithCString:product.product_name.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Info::Product translateToCppProduct(MAVSDKInfoProduct* product)
{
    mavsdk::Info::Product obj;


        
    obj.vendor_id = product.vendorId;
        
    
        
    obj.vendor_name = [product.vendorName UTF8String];
        
    
        
    obj.product_id = product.productId;
        
    
        
    obj.product_name = [product.productName UTF8String];
        
    
    return obj;
}



MAVSDKInfoVersion* translateFromCppVersion(mavsdk::Info::Version version)
{
    MAVSDKInfoVersion *obj = [[MAVSDKInfoVersion alloc] init];


        
    obj.flightSwMajor = version.flight_sw_major;
        
    
        
    obj.flightSwMinor = version.flight_sw_minor;
        
    
        
    obj.flightSwPatch = version.flight_sw_patch;
        
    
        
    obj.flightSwVendorMajor = version.flight_sw_vendor_major;
        
    
        
    obj.flightSwVendorMinor = version.flight_sw_vendor_minor;
        
    
        
    obj.flightSwVendorPatch = version.flight_sw_vendor_patch;
        
    
        
    obj.osSwMajor = version.os_sw_major;
        
    
        
    obj.osSwMinor = version.os_sw_minor;
        
    
        
    obj.osSwPatch = version.os_sw_patch;
        
    
        
    [NSString stringWithCString:version.flight_sw_git_hash.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    [NSString stringWithCString:version.os_sw_git_hash.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Info::Version translateToCppVersion(MAVSDKInfoVersion* version)
{
    mavsdk::Info::Version obj;


        
    obj.flight_sw_major = version.flightSwMajor;
        
    
        
    obj.flight_sw_minor = version.flightSwMinor;
        
    
        
    obj.flight_sw_patch = version.flightSwPatch;
        
    
        
    obj.flight_sw_vendor_major = version.flightSwVendorMajor;
        
    
        
    obj.flight_sw_vendor_minor = version.flightSwVendorMinor;
        
    
        
    obj.flight_sw_vendor_patch = version.flightSwVendorPatch;
        
    
        
    obj.os_sw_major = version.osSwMajor;
        
    
        
    obj.os_sw_minor = version.osSwMinor;
        
    
        
    obj.os_sw_patch = version.osSwPatch;
        
    
        
    obj.flight_sw_git_hash = [version.flightSwGitHash UTF8String];
        
    
        
    obj.os_sw_git_hash = [version.osSwGitHash UTF8String];
        
    
    return obj;
}






@end