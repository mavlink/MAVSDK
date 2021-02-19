#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/info/info.h>





@interface MVSFlightInfo : NSObject



@property (nonatomic, assign) UInt32 timeBootMs;


@property (nonatomic, assign) UInt64 flightUid;



@end




@interface MVSIdentification : NSObject



@property (nonatomic, strong) NSString *hardwareUid;



@end




@interface MVSProduct : NSObject



@property (nonatomic, assign) NSInteger vendorId;


@property (nonatomic, strong) NSString *vendorName;


@property (nonatomic, assign) NSInteger productId;


@property (nonatomic, strong) NSString *productName;



@end




@interface MVSVersion : NSObject



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






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultInformationNotReceivedYet,
};



@interface MVSInfo : NSObject








@end