#import "plugin-tracking_server.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/tracking_server/tracking_server.h>


MAVSDKTrackingServerCommandAnswer translateFromCppCommandAnswer(mavsdk::TrackingServer::CommandAnswer command_answer)
{
    switch (command_answer) {
        default:
            NSLog(@"Unknown command_answer enum value: %d", static_cast<int>(command_answer));
        // FALLTHROUGH
        case mavsdk::TrackingServer::CommandAnswer::Accepted:
            return MAVSDKTrackingServerCommandAnswerAccepted;
        case mavsdk::TrackingServer::CommandAnswer::TemporarilyRejected:
            return MAVSDKTrackingServerCommandAnswerTemporarilyRejected;
        case mavsdk::TrackingServer::CommandAnswer::Denied:
            return MAVSDKTrackingServerCommandAnswerDenied;
        case mavsdk::TrackingServer::CommandAnswer::Unsupported:
            return MAVSDKTrackingServerCommandAnswerUnsupported;
        case mavsdk::TrackingServer::CommandAnswer::Failed:
            return MAVSDKTrackingServerCommandAnswerFailed;
    }
}

mavsdk::TrackingServer::CommandAnswer translateToCppCommandAnswer(MAVSDKTrackingServerCommandAnswer commandAnswer)
{
    switch (commandAnswer) {
        default:
            NSLog(@"Unknown CommandAnswer enum value: %d", static_cast<int>(commandAnswer));
        // FALLTHROUGH
        case MAVSDKTrackingServerCommandAnswerAccepted:
            return mavsdk::TrackingServer::CommandAnswer::Accepted;
        case MAVSDKTrackingServerCommandAnswerTemporarilyRejected:
            return mavsdk::TrackingServer::CommandAnswer::TemporarilyRejected;
        case MAVSDKTrackingServerCommandAnswerDenied:
            return mavsdk::TrackingServer::CommandAnswer::Denied;
        case MAVSDKTrackingServerCommandAnswerUnsupported:
            return mavsdk::TrackingServer::CommandAnswer::Unsupported;
        case MAVSDKTrackingServerCommandAnswerFailed:
            return mavsdk::TrackingServer::CommandAnswer::Failed;
    }
}



MAVSDKTrackingServerTrackPoint* translateFromCppTrackPoint(mavsdk::TrackingServer::TrackPoint track_point)
{
    MAVSDKTrackingServerTrackPoint *obj = [[MAVSDKTrackingServerTrackPoint alloc] init];


        
    obj.pointX = track_point.point_x;
        
    
        
    obj.pointY = track_point.point_y;
        
    
        
    obj.radius = track_point.radius;
        
    
    return obj;
}

mavsdk::TrackingServer::TrackPoint translateToCppTrackPoint(MAVSDKTrackingServerTrackPoint* trackPoint)
{
    mavsdk::TrackingServer::TrackPoint obj;


        
    obj.point_x = trackPoint.pointX;
        
    
        
    obj.point_y = trackPoint.pointY;
        
    
        
    obj.radius = trackPoint.radius;
        
    
    return obj;
}



@implementation MAVSDKTrackingServerTrackPoint
@end



MAVSDKTrackingServerTrackRectangle* translateFromCppTrackRectangle(mavsdk::TrackingServer::TrackRectangle track_rectangle)
{
    MAVSDKTrackingServerTrackRectangle *obj = [[MAVSDKTrackingServerTrackRectangle alloc] init];


        
    obj.topLeftCornerX = track_rectangle.top_left_corner_x;
        
    
        
    obj.topLeftCornerY = track_rectangle.top_left_corner_y;
        
    
        
    obj.bottomRightCornerX = track_rectangle.bottom_right_corner_x;
        
    
        
    obj.bottomRightCornerY = track_rectangle.bottom_right_corner_y;
        
    
    return obj;
}

mavsdk::TrackingServer::TrackRectangle translateToCppTrackRectangle(MAVSDKTrackingServerTrackRectangle* trackRectangle)
{
    mavsdk::TrackingServer::TrackRectangle obj;


        
    obj.top_left_corner_x = trackRectangle.topLeftCornerX;
        
    
        
    obj.top_left_corner_y = trackRectangle.topLeftCornerY;
        
    
        
    obj.bottom_right_corner_x = trackRectangle.bottomRightCornerX;
        
    
        
    obj.bottom_right_corner_y = trackRectangle.bottomRightCornerY;
        
    
    return obj;
}



@implementation MAVSDKTrackingServerTrackRectangle
@end








@implementation MAVSDKTrackingServer

mavsdk::TrackingServer *tracking_server;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    tracking_server = new mavsdk::TrackingServer(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (void)setTrackingPointStatus:(MAVSDKTrackingServerTrackPoint*)trackedPoint {
    tracking_server->set_tracking_point_status(translateToCppTrackedPoint(trackedPoint));
}
- (void)setTrackingRectangleStatus:(MAVSDKTrackingServerTrackRectangle*)trackedRectangle {
    tracking_server->set_tracking_rectangle_status(translateToCppTrackedRectangle(trackedRectangle));
}
- (void)setTrackingOffStatus {
    tracking_server->set_tracking_off_status();
}



- (MAVSDKTrackingServerResult)respondTrackingPointCommand:(MAVSDKTrackingServerCommandAnswer)commandAnswer {
    return (MAVSDKTrackingServerResult)tracking_server->respond_tracking_point_command((mavsdk::TrackingServer::CommandAnswer)commandAnswer);
}
- (MAVSDKTrackingServerResult)respondTrackingRectangleCommand:(MAVSDKTrackingServerCommandAnswer)commandAnswer {
    return (MAVSDKTrackingServerResult)tracking_server->respond_tracking_rectangle_command((mavsdk::TrackingServer::CommandAnswer)commandAnswer);
}
- (MAVSDKTrackingServerResult)respondTrackingOffCommand:(MAVSDKTrackingServerCommandAnswer)commandAnswer {
    return (MAVSDKTrackingServerResult)tracking_server->respond_tracking_off_command((mavsdk::TrackingServer::CommandAnswer)commandAnswer);
}


@end

