#ifndef plugin_info_h
#define plugin_info_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;





@interface MAVSDKInfoFlightInfo : NSObject


@property (nonatomic, assign) UInt32 timeBootMs; 
@property (nonatomic, assign) UInt64 flightUid; 


@end




@interface MAVSDKInfoIdentification : NSObject


@property (nonatomic, strong) NSString *hardwareUid;


@end




@interface MAVSDKInfoProduct : NSObject


@property (nonatomic, assign) SInt32 vendorId; 
@property (nonatomic, strong) NSString *vendorName;
@property (nonatomic, assign) SInt32 productId; 
@property (nonatomic, strong) NSString *productName;


@end




@interface MAVSDKInfoVersion : NSObject


@property (nonatomic, assign) SInt32 flightSwMajor; 
@property (nonatomic, assign) SInt32 flightSwMinor; 
@property (nonatomic, assign) SInt32 flightSwPatch; 
@property (nonatomic, assign) SInt32 flightSwVendorMajor; 
@property (nonatomic, assign) SInt32 flightSwVendorMinor; 
@property (nonatomic, assign) SInt32 flightSwVendorPatch; 
@property (nonatomic, assign) SInt32 osSwMajor; 
@property (nonatomic, assign) SInt32 osSwMinor; 
@property (nonatomic, assign) SInt32 osSwPatch; 
@property (nonatomic, strong) NSString *flightSwGitHash;
@property (nonatomic, strong) NSString *osSwGitHash;


@end






typedef NS_ENUM(NSInteger, MAVSDKInfoResult)  {
    MAVSDKInfoResultUnknown,
    MAVSDKInfoResultSuccess,
    MAVSDKInfoResultInformationNotReceivedYet,
};



@interface MAVSDKInfo : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;







@end

#endif // plugin_info_h