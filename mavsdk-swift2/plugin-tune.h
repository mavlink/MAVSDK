#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/tune/tune.h>



typedef NS_ENUM(NSInteger, MVSSongElement)  {
    MVSSongElementStyleLegato,
    MVSSongElementStyleNormal,
    MVSSongElementStyleStaccato,
    MVSSongElementDuration1,
    MVSSongElementDuration2,
    MVSSongElementDuration4,
    MVSSongElementDuration8,
    MVSSongElementDuration16,
    MVSSongElementDuration32,
    MVSSongElementNoteA,
    MVSSongElementNoteB,
    MVSSongElementNoteC,
    MVSSongElementNoteD,
    MVSSongElementNoteE,
    MVSSongElementNoteF,
    MVSSongElementNoteG,
    MVSSongElementNotePause,
    MVSSongElementSharp,
    MVSSongElementFlat,
    MVSSongElementOctaveUp,
    MVSSongElementOctaveDown,
};




@interface MVSTuneDescription : NSObject



@property (nonatomic, assign) MVSSongElement songElements;


@property (nonatomic, assign) NSInteger tempo;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultInvalidTempo,
    MVSResultTuneTooLong,
    MVSResultError,
};



@interface MVSTune : NSObject

- (MVSResult)playTune
:(
MVSTuneDescription*)
tuneDescription;


@end