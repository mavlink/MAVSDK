#import "plugin-tune.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/tune/tune.h>


MAVSDKTuneSongElement translateFromCppSongElement(mavsdk::Tune::SongElement song_element)
{
    switch (song_element) {
        default:
            NSLog(@"Unknown song_element enum value: %d", static_cast<int>(song_element));
        // FALLTHROUGH
        case mavsdk::Tune::SongElement::StyleLegato:
            return MAVSDKTuneSongElementStyleLegato;
        case mavsdk::Tune::SongElement::StyleNormal:
            return MAVSDKTuneSongElementStyleNormal;
        case mavsdk::Tune::SongElement::StyleStaccato:
            return MAVSDKTuneSongElementStyleStaccato;
        case mavsdk::Tune::SongElement::Duration1:
            return MAVSDKTuneSongElementDuration1;
        case mavsdk::Tune::SongElement::Duration2:
            return MAVSDKTuneSongElementDuration2;
        case mavsdk::Tune::SongElement::Duration4:
            return MAVSDKTuneSongElementDuration4;
        case mavsdk::Tune::SongElement::Duration8:
            return MAVSDKTuneSongElementDuration8;
        case mavsdk::Tune::SongElement::Duration16:
            return MAVSDKTuneSongElementDuration16;
        case mavsdk::Tune::SongElement::Duration32:
            return MAVSDKTuneSongElementDuration32;
        case mavsdk::Tune::SongElement::NoteA:
            return MAVSDKTuneSongElementNoteA;
        case mavsdk::Tune::SongElement::NoteB:
            return MAVSDKTuneSongElementNoteB;
        case mavsdk::Tune::SongElement::NoteC:
            return MAVSDKTuneSongElementNoteC;
        case mavsdk::Tune::SongElement::NoteD:
            return MAVSDKTuneSongElementNoteD;
        case mavsdk::Tune::SongElement::NoteE:
            return MAVSDKTuneSongElementNoteE;
        case mavsdk::Tune::SongElement::NoteF:
            return MAVSDKTuneSongElementNoteF;
        case mavsdk::Tune::SongElement::NoteG:
            return MAVSDKTuneSongElementNoteG;
        case mavsdk::Tune::SongElement::NotePause:
            return MAVSDKTuneSongElementNotePause;
        case mavsdk::Tune::SongElement::Sharp:
            return MAVSDKTuneSongElementSharp;
        case mavsdk::Tune::SongElement::Flat:
            return MAVSDKTuneSongElementFlat;
        case mavsdk::Tune::SongElement::OctaveUp:
            return MAVSDKTuneSongElementOctaveUp;
        case mavsdk::Tune::SongElement::OctaveDown:
            return MAVSDKTuneSongElementOctaveDown;
    }
}

mavsdk::Tune::SongElement translateToCppSongElement(MAVSDKTuneSongElement songElement)
{
    switch (songElement) {
        default:
            NSLog(@"Unknown SongElement enum value: %d", static_cast<int>(songElement));
        // FALLTHROUGH
        case MAVSDKTuneSongElementStyleLegato:
            return mavsdk::Tune::SongElement::StyleLegato;
        case MAVSDKTuneSongElementStyleNormal:
            return mavsdk::Tune::SongElement::StyleNormal;
        case MAVSDKTuneSongElementStyleStaccato:
            return mavsdk::Tune::SongElement::StyleStaccato;
        case MAVSDKTuneSongElementDuration1:
            return mavsdk::Tune::SongElement::Duration1;
        case MAVSDKTuneSongElementDuration2:
            return mavsdk::Tune::SongElement::Duration2;
        case MAVSDKTuneSongElementDuration4:
            return mavsdk::Tune::SongElement::Duration4;
        case MAVSDKTuneSongElementDuration8:
            return mavsdk::Tune::SongElement::Duration8;
        case MAVSDKTuneSongElementDuration16:
            return mavsdk::Tune::SongElement::Duration16;
        case MAVSDKTuneSongElementDuration32:
            return mavsdk::Tune::SongElement::Duration32;
        case MAVSDKTuneSongElementNoteA:
            return mavsdk::Tune::SongElement::NoteA;
        case MAVSDKTuneSongElementNoteB:
            return mavsdk::Tune::SongElement::NoteB;
        case MAVSDKTuneSongElementNoteC:
            return mavsdk::Tune::SongElement::NoteC;
        case MAVSDKTuneSongElementNoteD:
            return mavsdk::Tune::SongElement::NoteD;
        case MAVSDKTuneSongElementNoteE:
            return mavsdk::Tune::SongElement::NoteE;
        case MAVSDKTuneSongElementNoteF:
            return mavsdk::Tune::SongElement::NoteF;
        case MAVSDKTuneSongElementNoteG:
            return mavsdk::Tune::SongElement::NoteG;
        case MAVSDKTuneSongElementNotePause:
            return mavsdk::Tune::SongElement::NotePause;
        case MAVSDKTuneSongElementSharp:
            return mavsdk::Tune::SongElement::Sharp;
        case MAVSDKTuneSongElementFlat:
            return mavsdk::Tune::SongElement::Flat;
        case MAVSDKTuneSongElementOctaveUp:
            return mavsdk::Tune::SongElement::OctaveUp;
        case MAVSDKTuneSongElementOctaveDown:
            return mavsdk::Tune::SongElement::OctaveDown;
    }
}



MAVSDKTuneTuneDescription* translateFromCppTuneDescription(mavsdk::Tune::TuneDescription tune_description)
{
    MAVSDKTuneTuneDescription *obj = [[MAVSDKTuneTuneDescription alloc] init];


        
            
    for (const auto& elem : tune_description.song_elements) {
        [obj.songElements addObject:[NSNumber numberWithInteger:translateFromCppSongElement(elem)]];
    }
            
        
    
        
    obj.tempo = tune_description.tempo;
        
    
    return obj;
}

mavsdk::Tune::TuneDescription translateToCppTuneDescription(MAVSDKTuneTuneDescription* tuneDescription)
{
    mavsdk::Tune::TuneDescription obj;


        
            
    for (NSNumber *elem in tuneDescription.songElements) {
        obj.song_elements.push_back(translateToCppSongElement((MAVSDKTuneSongElement)[elem intValue]));
    }
            
        
    
        
    obj.tempo = tuneDescription.tempo;
        
    
    return obj;
}



@implementation MAVSDKTuneTuneDescription
@end








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


@end

