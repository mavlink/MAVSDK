#import "plugin-tune.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/tune/tune.h>

@implementation MAVSDKTune

mavsdk::Tune *tune;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    tune = new mavsdk::Tune(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKTuneResult)playTune:(MAVSDKTuneTuneDescription*)tuneDescription {
    return (MAVSDKTuneResult)tune->play_tune(translateToCppTuneDescription(tuneDescription));
}







MAVSDKTuneTuneDescription* translateFromCppTuneDescription(mavsdk::Tune::TuneDescription tune_description)
{
    MAVSDKTuneTuneDescription *obj = [[MAVSDKTuneTuneDescription alloc] init];


        
            //for (const auto& elem : tune_description.song_elements) {
            //    [obj.songElements addObject:translateFromCppSongElement(elem)];
            //}
        
    
        
    obj.tempo = tune_description.tempo;
        
    
    return obj;
}

mavsdk::Tune::TuneDescription translateToCppTuneDescription(MAVSDKTuneTuneDescription* tuneDescription)
{
    mavsdk::Tune::TuneDescription obj;


        
            //for (MAVSDKTuneSongElement *elem in tuneDescription.songElements) {
            //    obj.song_elements.push_back(translateToCppSongElement(elem));
            //}
        
    
        
    obj.tempo = tuneDescription.tempo;
        
    
    return obj;
}






@end
