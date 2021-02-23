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


@property (nonatomic, assign) NSInteger vendorId;
@property (nonatomic, strong) NSString *vendorName;
@property (nonatomic, assign) NSInteger productId;
@property (nonatomic, strong) NSString *productName;


@end




@interface MAVSDKInfoVersion : NSObject


@property (nonatomic, assign) NSInteger flightSwMajor;
@property (nonatomic, assign) NSInteger flightSwMinor;
@property (nonatomic, assign) NSInteger flightSwPatch;
@property (nonatomic, assign) NSInteger flightSwVendorMajor;
@property (nonatomic, assign) NSInteger flightSwVendorMinor;
@property (nonatomic, assign) NSInteger flightSwVendorPatch;
@property (nonatomic, assign) NSInteger osSwMajor;
@property (nonatomic, assign) NSInteger osSwMinor;
@property (nonatomic, assign) NSInteger osSwPatch;
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