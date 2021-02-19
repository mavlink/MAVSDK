#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>





@interface MVSIntParam : NSObject



@property (nonatomic, strong) NSString *name;


@property (nonatomic, assign) NSInteger value;



@end




@interface MVSFloatParam : NSObject



@property (nonatomic, strong) NSString *name;


@property (nonatomic, assign) float value;



@end




@interface MVSAllParams : NSObject



@property (nonatomic, strong) NSMutableArray *intParams;


@property (nonatomic, strong) NSMutableArray *floatParams;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultTimeout,
    MVSResultConnectionError,
    MVSResultWrongType,
    MVSResultParamNameTooLong,
};



@interface MVSParam : NSObject


- (MVSResult)setParamInt
:(
NSString*)
name :(
NSInteger)
value;

- (MVSResult)setParamFloat
:(
NSString*)
name :(
float)
value;



@end