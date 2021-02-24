#ifndef plugin_tune_h
#define plugin_tune_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;



typedef NS_ENUM(NSInteger, MAVSDKTuneSongElement)  {
    MAVSDKTuneSongElementStyleLegato,
    MAVSDKTuneSongElementStyleNormal,
    MAVSDKTuneSongElementStyleStaccato,
    MAVSDKTuneSongElementDuration1,
    MAVSDKTuneSongElementDuration2,
    MAVSDKTuneSongElementDuration4,
    MAVSDKTuneSongElementDuration8,
    MAVSDKTuneSongElementDuration16,
    MAVSDKTuneSongElementDuration32,
    MAVSDKTuneSongElementNoteA,
    MAVSDKTuneSongElementNoteB,
    MAVSDKTuneSongElementNoteC,
    MAVSDKTuneSongElementNoteD,
    MAVSDKTuneSongElementNoteE,
    MAVSDKTuneSongElementNoteF,
    MAVSDKTuneSongElementNoteG,
    MAVSDKTuneSongElementNotePause,
    MAVSDKTuneSongElementSharp,
    MAVSDKTuneSongElementFlat,
    MAVSDKTuneSongElementOctaveUp,
    MAVSDKTuneSongElementOctaveDown,
};




@interface MAVSDKTuneTuneDescription : NSObject


@property (nonatomic, strong) NSMutableArray *songElements;
@property (nonatomic, assign) SInt32 tempo; 


@end






typedef NS_ENUM(NSInteger, MAVSDKTuneResult)  {
    MAVSDKTuneResultUnknown,
    MAVSDKTuneResultSuccess,
    MAVSDKTuneResultInvalidTempo,
    MAVSDKTuneResultTuneTooLong,
    MAVSDKTuneResultError,
};



@interface MAVSDKTune : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKTuneResult)playTune :(MAVSDKTuneTuneDescription*) tuneDescription;


@end

#endif // plugin_tune_h