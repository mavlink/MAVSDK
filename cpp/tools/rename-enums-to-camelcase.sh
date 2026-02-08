#!/usr/bin/env bash

files=`git ls-files | grep -E "\.h$|\.c$|\.cpp$"`


sed -i 's/Action::Result::UNKNOWN/Action::Result::Unknown/g' $files
sed -i 's/Action::Result::SUCCESS/Action::Result::Success/g' $files
sed -i 's/Action::Result::NO_SYSTEM/Action::Result::NoSystem/g' $files
sed -i 's/Action::Result::CONNECTION_ERROR/Action::Result::ConnectionError/g' $files
sed -i 's/Action::Result::BUSY/Action::Result::Busy/g' $files
sed -i 's/Action::Result::COMMAND_DENIED/Action::Result::CommandDenied/g' $files
sed -i 's/Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN/Action::Result::CommandDeniedLandedStateUnknown/g' $files
sed -i 's/Action::Result::COMMAND_DENIED_NOT_LANDED/Action::Result::CommandDeniedNotLanded/g' $files
sed -i 's/Action::Result::TIMEOUT/Action::Result::Timeout/g' $files
sed -i 's/Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN/Action::Result::VtolTransitionSupportUnknown/g' $files
sed -i 's/Action::Result::NO_VTOL_TRANSITION_SUPPORT/Action::Result::NoVtolTransitionSupport/g' $files
sed -i 's/Action::Result::PARAMETER_ERROR/Action::Result::ParameterError/g' $files

sed -i 's/Calibration::Result::UNKNOWN/Calibration::Result::Unknown/g' $files
sed -i 's/Calibration::Result::SUCCESS/Calibration::Result::Success/g' $files
sed -i 's/Calibration::Result::NEXT/Calibration::Result::Next/g' $files
sed -i 's/Calibration::Result::FAILED/Calibration::Result::Failed/g' $files
sed -i 's/Calibration::Result::NO_SYSTEM/Calibration::Result::NoSystem/g' $files
sed -i 's/Calibration::Result::CONNECTION_ERROR/Calibration::Result::ConnectionError/g' $files
sed -i 's/Calibration::Result::BUSY/Calibration::Result::Busy/g' $files
sed -i 's/Calibration::Result::COMMAND_DENIED/Calibration::Result::CommandDenied/g' $files
sed -i 's/Calibration::Result::TIMEOUT/Calibration::Result::Timeout/g' $files
sed -i 's/Calibration::Result::CANCELLED/Calibration::Result::Cancelled/g' $files

sed -i 's/Camera::Mode::UNKNOWN/Camera::Mode::Unknown/g' $files
sed -i 's/Camera::Mode::PHOTO/Camera::Mode::Photo/g' $files
sed -i 's/Camera::Mode::VIDEO/Camera::Mode::Video/g' $files

sed -i 's/Camera::StorageStatus::NOT_AVAILABLE/Camera::StorageStatus::NotAvailable/g' $files
sed -i 's/Camera::StorageStatus::UNFORMATTED/Camera::StorageStatus::Unformatted/g' $files
sed -i 's/Camera::StorageStatus::FORMATTED/Camera::StorageStatus::Formatted/g' $files

sed -i 's/FollowMe::FollowDirection::NONE/FollowMe::FollowDirection::None/g' $files
sed -i 's/FollowMe::FollowDirection::BEHIND/FollowMe::FollowDirection::Behind/g' $files
sed -i 's/FollowMe::FollowDirection::FRONT/FollowMe::FollowDirection::Front/g' $files
sed -i 's/FollowMe::FollowDirection::FRONT_RIGHT/FollowMe::FollowDirection::FrontRight/g' $files
sed -i 's/FollowMe::FollowDirection::FRONT_LEFT/FollowMe::FollowDirection::FrontLeft/g' $files

sed -i 's/FollowMe::Result::UNKNOWN/FollowMe::Result::Unknown/g' $files
sed -i 's/FollowMe::Result::SUCCESS/FollowMe::Result::Success/g' $files
sed -i 's/FollowMe::Result::NO_SYSTEM/FollowMe::Result::NoSystem/g' $files
sed -i 's/FollowMe::Result::CONNECTION_ERROR/FollowMe::Result::ConnectionError/g' $files
sed -i 's/FollowMe::Result::BUSY/FollowMe::Result::Busy/g' $files
sed -i 's/FollowMe::Result::COMMAND_DENIED/FollowMe::Result::CommandDenied/g' $files
sed -i 's/FollowMe::Result::TIMEOUT/FollowMe::Result::Timeout/g' $files
sed -i 's/FollowMe::Result::NOT_ACTIVE/FollowMe::Result::NotActive/g' $files
sed -i 's/FollowMe::Result::SET_CONFIG_FAILED/FollowMe::Result::SetConfigFailed/g' $files

sed -i 's/FollowMe::FollowDirection::NONE/FollowMe::FollowDirection::None/g' $files
sed -i 's/FollowMe::FollowDirection::BEHIND/FollowMe::FollowDirection::Behind/g' $files
sed -i 's/FollowMe::FollowDirection::FRONT/FollowMe::FollowDirection::Front/g' $files
sed -i 's/FollowMe::FollowDirection::FRONT_RIGHT/FollowMe::FollowDirection::FrontRight/g' $files
sed -i 's/FollowMe::FollowDirection::FRONT_LEFT/FollowMe::FollowDirection::FrontLeft/g' $files

sed -i 's/FollowMe::Result::UNKNOWN/FollowMe::Result::Unknown/g' $files
sed -i 's/FollowMe::Result::SUCCESS/FollowMe::Result::Success/g' $files
sed -i 's/FollowMe::Result::NO_SYSTEM/FollowMe::Result::NoSystem/g' $files
sed -i 's/FollowMe::Result::CONNECTION_ERROR/FollowMe::Result::ConnectionError/g' $files
sed -i 's/FollowMe::Result::BUSY/FollowMe::Result::Busy/g' $files
sed -i 's/FollowMe::Result::COMMAND_DENIED/FollowMe::Result::CommandDenied/g' $files
sed -i 's/FollowMe::Result::TIMEOUT/FollowMe::Result::Timeout/g' $files
sed -i 's/FollowMe::Result::NOT_ACTIVE/FollowMe::Result::NotActive/g' $files
sed -i 's/FollowMe::Result::SET_CONFIG_FAILED/FollowMe::Result::SetConfigFailed/g' $files

sed -i 's/Ftp::Result::UNKNOWN/Ftp::Result::Unknown/g' $files
sed -i 's/Ftp::Result::SUCCESS/Ftp::Result::Success/g' $files
sed -i 's/Ftp::Result::NEXT/Ftp::Result::Next/g' $files
sed -i 's/Ftp::Result::TIMEOUT/Ftp::Result::Timeout/g' $files
sed -i 's/Ftp::Result::BUSY/Ftp::Result::Busy/g' $files
sed -i 's/Ftp::Result::FILE_IO_ERROR/Ftp::Result::FileIoError/g' $files
sed -i 's/Ftp::Result::FILE_EXISTS/Ftp::Result::FileExists/g' $files
sed -i 's/Ftp::Result::FILE_DOES_NOT_EXIST/Ftp::Result::FileDoesNotExist/g' $files
sed -i 's/Ftp::Result::FILE_PROTECTED/Ftp::Result::FileProtected/g' $files
sed -i 's/Ftp::Result::INVALID_PARAMETER/Ftp::Result::InvalidParameter/g' $files
sed -i 's/Ftp::Result::UNSUPPORTED/Ftp::Result::Unsupported/g' $files
sed -i 's/Ftp::Result::PROTOCOL_ERROR/Ftp::Result::ProtocolError/g' $files

sed -i 's/Geofence::Type::INCLUSION/Geofence::Type::Inclusion/g' $files
sed -i 's/Geofence::Type::EXCLUSION/Geofence::Type::Exclusion/g' $files

sed -i 's/Geofence::Result::UNKNOWN/Geofence::Result::Unknown/g' $files
sed -i 's/Geofence::Result::SUCCESS/Geofence::Result::Success/g' $files
sed -i 's/Geofence::Result::ERROR/Geofence::Result::Error/g' $files
sed -i 's/Geofence::Result::TOO_MANY_GEOFENCE_ITEMS/Geofence::Result::TooManyGeofenceItems/g' $files
sed -i 's/Geofence::Result::BUSY/Geofence::Result::Busy/g' $files
sed -i 's/Geofence::Result::TIMEOUT/Geofence::Result::Timeout/g' $files
sed -i 's/Geofence::Result::INVALID_ARGUMENT/Geofence::Result::InvalidArgument/g' $files

sed -i 's/Gimbal::GimbalMode::YAW_FOLLOW/Gimbal::GimbalMode::YawFollow/g' $files
sed -i 's/Gimbal::GimbalMode::YAW_LOCK/Gimbal::GimbalMode::YawLock/g' $files

sed -i 's/Gimbal::Result::UNKNOWN/Gimbal::Result::Unknown/g' $files
sed -i 's/Gimbal::Result::SUCCESS/Gimbal::Result::Success/g' $files
sed -i 's/Gimbal::Result::ERROR/Gimbal::Result::Error/g' $files
sed -i 's/Gimbal::Result::TIMEOUT/Gimbal::Result::Timeout/g' $files

sed -i 's/Info::Result::UNKNOWN/Info::Result::Unknown/g' $files
sed -i 's/Info::Result::SUCCESS/Info::Result::Success/g' $files
sed -i 's/Info::Result::INFORMATION_NOT_RECEIVED_YET/Info::Result::InformationNotReceivedYet/g' $files

sed -i 's/LogFiles::Result::UNKNOWN/LogFiles::Result::Unknown/g' $files
sed -i 's/LogFiles::Result::SUCCESS/LogFiles::Result::Success/g' $files
sed -i 's/LogFiles::Result::PROGRESS/LogFiles::Result::Progress/g' $files
sed -i 's/LogFiles::Result::NO_LOGFILES/LogFiles::Result::NoLogfiles/g' $files
sed -i 's/LogFiles::Result::TIMEOUT/LogFiles::Result::Timeout/g' $files
sed -i 's/LogFiles::Result::INVALID_ARGUMENT/LogFiles::Result::InvalidArgument/g' $files
sed -i 's/LogFiles::Result::FILE_OPEN_FAILED/LogFiles::Result::FileOpenFailed/g' $files

sed -i 's/Mission::CameraAction::NONE/Mission::CameraAction::None/g' $files
sed -i 's/Mission::CameraAction::TAKE_PHOTO/Mission::CameraAction::TakePhoto/g' $files
sed -i 's/Mission::CameraAction::START_PHOTO_INTERVAL/Mission::CameraAction::StartPhotoInterval/g' $files
sed -i 's/Mission::CameraAction::STOP_PHOTO_INTERVAL/Mission::CameraAction::StopPhotoInterval/g' $files
sed -i 's/Mission::CameraAction::START_VIDEO/Mission::CameraAction::StartVideo/g' $files
sed -i 's/Mission::CameraAction::STOP_VIDEO/Mission::CameraAction::StopVideo/g' $files

sed -i 's/Mission::Result::UNKNOWN/Mission::Result::Unknown/g' $files
sed -i 's/Mission::Result::SUCCESS/Mission::Result::Success/g' $files
sed -i 's/Mission::Result::ERROR/Mission::Result::Error/g' $files
sed -i 's/Mission::Result::TOO_MANY_MISSION_ITEMS/Mission::Result::TooManyMissionItems/g' $files
sed -i 's/Mission::Result::BUSY/Mission::Result::Busy/g' $files
sed -i 's/Mission::Result::TIMEOUT/Mission::Result::Timeout/g' $files
sed -i 's/Mission::Result::INVALID_ARGUMENT/Mission::Result::InvalidArgument/g' $files
sed -i 's/Mission::Result::UNSUPPORTED/Mission::Result::Unsupported/g' $files
sed -i 's/Mission::Result::NO_MISSION_AVAILABLE/Mission::Result::NoMissionAvailable/g' $files
sed -i 's/Mission::Result::FAILED_TO_OPEN_QGC_PLAN/Mission::Result::FailedToOpenQgcPlan/g' $files
sed -i 's/Mission::Result::FAILED_TO_PARSE_QGC_PLAN/Mission::Result::FailedToParseQgcPlan/g' $files
sed -i 's/Mission::Result::UNSUPPORTED_MISSION_CMD/Mission::Result::UnsupportedMissionCmd/g' $files
sed -i 's/Mission::Result::TRANSFER_CANCELLED/Mission::Result::TransferCancelled/g' $files

sed -i 's/Mission::Result::UNKNOWN/Mission::Result::Unknown/g' $files
sed -i 's/Mission::Result::SUCCESS/Mission::Result::Success/g' $files
sed -i 's/Mission::Result::ERROR/Mission::Result::Error/g' $files
sed -i 's/Mission::Result::TOO_MANY_MISSION_ITEMS/Mission::Result::TooManyMissionItems/g' $files
sed -i 's/Mission::Result::BUSY/Mission::Result::Busy/g' $files
sed -i 's/Mission::Result::TIMEOUT/Mission::Result::Timeout/g' $files
sed -i 's/Mission::Result::INVALID_ARGUMENT/Mission::Result::InvalidArgument/g' $files
sed -i 's/Mission::Result::UNSUPPORTED/Mission::Result::Unsupported/g' $files
sed -i 's/Mission::Result::NO_MISSION_AVAILABLE/Mission::Result::NoMissionAvailable/g' $files
sed -i 's/Mission::Result::FAILED_TO_OPEN_QGC_PLAN/Mission::Result::FailedToOpenQgcPlan/g' $files
sed -i 's/Mission::Result::FAILED_TO_PARSE_QGC_PLAN/Mission::Result::FailedToParseQgcPlan/g' $files
sed -i 's/Mission::Result::UNSUPPORTED_MISSION_CMD/Mission::Result::UnsupportedMissionCmd/g' $files
sed -i 's/Mission::Result::TRANSFER_CANCELLED/Mission::Result::TransferCancelled/g' $files

sed -i 's/Mocap::MavFrame::MOCAP_NED/Mocap::MavFrame::MocapNed/g' $files
sed -i 's/Mocap::MavFrame::LOCAL_FRD/Mocap::MavFrame::LocalFrd/g' $files

sed -i 's/Mocap::Result::UNKNOWN/Mocap::Result::Unknown/g' $files
sed -i 's/Mocap::Result::SUCCESS/Mocap::Result::Success/g' $files
sed -i 's/Mocap::Result::NO_SYSTEM/Mocap::Result::NoSystem/g' $files
sed -i 's/Mocap::Result::CONNECTION_ERROR/Mocap::Result::ConnectionError/g' $files
sed -i 's/Mocap::Result::INVALID_REQUEST_DATA/Mocap::Result::InvalidRequestData/g' $files

sed -i 's/Offboard::Result::UNKNOWN/Offboard::Result::Unknown/g' $files
sed -i 's/Offboard::Result::SUCCESS/Offboard::Result::Success/g' $files
sed -i 's/Offboard::Result::NO_SYSTEM/Offboard::Result::NoSystem/g' $files
sed -i 's/Offboard::Result::CONNECTION_ERROR/Offboard::Result::ConnectionError/g' $files
sed -i 's/Offboard::Result::BUSY/Offboard::Result::Busy/g' $files
sed -i 's/Offboard::Result::COMMAND_DENIED/Offboard::Result::CommandDenied/g' $files
sed -i 's/Offboard::Result::TIMEOUT/Offboard::Result::Timeout/g' $files
sed -i 's/Offboard::Result::NO_SETPOINT_SET/Offboard::Result::NoSetpointSet/g' $files

sed -i 's/Param::Result::UNKNOWN/Param::Result::Unknown/g' $files
sed -i 's/Param::Result::SUCCESS/Param::Result::Success/g' $files
sed -i 's/Param::Result::TIMEOUT/Param::Result::Timeout/g' $files
sed -i 's/Param::Result::CONNECTION_ERROR/Param::Result::ConnectionError/g' $files
sed -i 's/Param::Result::WRONG_TYPE/Param::Result::WrongType/g' $files
sed -i 's/Param::Result::PARAM_NAME_TOO_LONG/Param::Result::ParamNameTooLong/g' $files

sed -i 's/Shell::Result::UNKNOWN/Shell::Result::Unknown/g' $files
sed -i 's/Shell::Result::SUCCESS/Shell::Result::Success/g' $files
sed -i 's/Shell::Result::NO_SYSTEM/Shell::Result::NoSystem/g' $files
sed -i 's/Shell::Result::CONNECTION_ERROR/Shell::Result::ConnectionError/g' $files
sed -i 's/Shell::Result::NO_RESPONSE/Shell::Result::NoResponse/g' $files
sed -i 's/Shell::Result::BUSY/Shell::Result::Busy/g' $files

sed -i 's/Telemetry::FixType::NO_GPS/Telemetry::FixType::NoGps/g' $files
sed -i 's/Telemetry::FixType::NO_FIX/Telemetry::FixType::NoFix/g' $files
sed -i 's/Telemetry::FixType::FIX_2D/Telemetry::FixType::Fix2D/g' $files
sed -i 's/Telemetry::FixType::FIX_3D/Telemetry::FixType::Fix3D/g' $files
sed -i 's/Telemetry::FixType::FIX_DGPS/Telemetry::FixType::FixDgps/g' $files
sed -i 's/Telemetry::FixType::RTK_FLOAT/Telemetry::FixType::RtkFloat/g' $files
sed -i 's/Telemetry::FixType::RTK_FIXED/Telemetry::FixType::RtkFixed/g' $files

sed -i 's/Telemetry::MavFrame::UNDEF/Telemetry::MavFrame::Undef/g' $files
sed -i 's/Telemetry::MavFrame::BODY_NED/Telemetry::MavFrame::BodyNed/g' $files
sed -i 's/Telemetry::MavFrame::VISION_NED/Telemetry::MavFrame::VisionNed/g' $files
sed -i 's/Telemetry::MavFrame::ESTIM_NED/Telemetry::MavFrame::EstimNed/g' $files

sed -i 's/Telemetry::Result::UNKNOWN/Telemetry::Result::Unknown/g' $files
sed -i 's/Telemetry::Result::SUCCESS/Telemetry::Result::Success/g' $files
sed -i 's/Telemetry::Result::NO_SYSTEM/Telemetry::Result::NoSystem/g' $files
sed -i 's/Telemetry::Result::CONNECTION_ERROR/Telemetry::Result::ConnectionError/g' $files
sed -i 's/Telemetry::Result::BUSY/Telemetry::Result::Busy/g' $files
sed -i 's/Telemetry::Result::COMMAND_DENIED/Telemetry::Result::CommandDenied/g' $files
sed -i 's/Telemetry::Result::TIMEOUT/Telemetry::Result::Timeout/g' $files

sed -i 's/Tune::SongElement::STYLE_LEGATO/Tune::SongElement::StyleLegato/g' $files
sed -i 's/Tune::SongElement::STYLE_NORMAL/Tune::SongElement::StyleNormal/g' $files
sed -i 's/Tune::SongElement::STYLE_STACCATO/Tune::SongElement::StyleStaccato/g' $files
sed -i 's/Tune::SongElement::DURATION_1/Tune::SongElement::Duration1/g' $files
sed -i 's/Tune::SongElement::DURATION_2/Tune::SongElement::Duration2/g' $files
sed -i 's/Tune::SongElement::DURATION_4/Tune::SongElement::Duration4/g' $files
sed -i 's/Tune::SongElement::DURATION_8/Tune::SongElement::Duration8/g' $files
sed -i 's/Tune::SongElement::DURATION_16/Tune::SongElement::Duration16/g' $files
sed -i 's/Tune::SongElement::DURATION_32/Tune::SongElement::Duration32/g' $files
sed -i 's/Tune::SongElement::NOTE_A/Tune::SongElement::NoteA/g' $files
sed -i 's/Tune::SongElement::NOTE_B/Tune::SongElement::NoteB/g' $files
sed -i 's/Tune::SongElement::NOTE_C/Tune::SongElement::NoteC/g' $files
sed -i 's/Tune::SongElement::NOTE_D/Tune::SongElement::NoteD/g' $files
sed -i 's/Tune::SongElement::NOTE_E/Tune::SongElement::NoteE/g' $files
sed -i 's/Tune::SongElement::NOTE_F/Tune::SongElement::NoteF/g' $files
sed -i 's/Tune::SongElement::NOTE_G/Tune::SongElement::NoteG/g' $files
sed -i 's/Tune::SongElement::NOTE_PAUSE/Tune::SongElement::NotePause/g' $files
sed -i 's/Tune::SongElement::SHARP/Tune::SongElement::Sharp/g' $files
sed -i 's/Tune::SongElement::FLAT/Tune::SongElement::Flat/g' $files
sed -i 's/Tune::SongElement::OCTAVE_UP/Tune::SongElement::OctaveUp/g' $files
sed -i 's/Tune::SongElement::OCTAVE_DOWN/Tune::SongElement::OctaveDown/g' $files

sed -i 's/Tune::Result::SUCCESS/Tune::Result::Success/g' $files
sed -i 's/Tune::Result::INVALID_TEMPO/Tune::Result::InvalidTempo/g' $files
sed -i 's/Tune::Result::TUNE_TOO_LONG/Tune::Result::TuneTooLong/g' $files
sed -i 's/Tune::Result::ERROR/Tune::Result::Error/g' $files

sed -i 's/MavlinkPassthrough:Result::UNKNOWN/MavlinkPassthrough::Result::Unknown/g' $files
sed -i 's/MavlinkPassthrough:Result::SUCCESS/MavlinkPassthrough::Result::Success/g' $files
sed -i 's/MavlinkPassthrough:Result::CONNECTION_ERROR/MavlinkPassthrough::Result::ConnectionError/g' $files

sed -i 's/MAVLinkParameters::Result::SUCCESS/MAVLinkParameters::Result::Success/g' $files
sed -i 's/MAVLinkParameters::Result::TIMEOUT/MAVLinkParameters::Result::Timeout/g' $files
sed -i 's/MAVLinkParameters::Result::CONNECTION_ERROR/MAVLinkParameters::Result::ConnectionError/g' $files
sed -i 's/MAVLinkParameters::Result::WRONG_TYPE/MAVLinkParameters::Result::WrongType/g' $files
sed -i 's/MAVLinkParameters::Result::PARAM_NAME_TOO_LONG/MAVLinkParameters::Result::ParamNameTooLong/g' $files

sed -i 's/MAVLinkCommands::Result::SUCCESS/MAVLinkCommands::Result::Success/g' $files
sed -i 's/MAVLinkCommands::Result::NO_SYSTEM/MAVLinkCommands::Result::NoSystem/g' $files
sed -i 's/MAVLinkCommands::Result::CONNECTION_ERROR/MAVLinkCommands::Result::ConnectionError/g' $files
sed -i 's/MAVLinkCommands::Result::BUSY/MAVLinkCommands::Result::Busy/g' $files
sed -i 's/MAVLinkCommands::Result::COMMAND_DENIED/MAVLinkCommands::Result::CommandDenied/g' $files
sed -i 's/MAVLinkCommands::Result::TIMEOUT/MAVLinkCommands::Result::Timeout/g' $files
sed -i 's/MAVLinkCommands::Result::IN_PROGRESS/MAVLinkCommands::Result::InProgress/g' $files
sed -i 's/MAVLinkCommands::Result::UNKNOWN_ERROR/MAVLinkCommands::Result::UnknownError/g' $files

sed -i 's/Color::RED/Color::Red/g' $files
sed -i 's/Color::GREEN/Color::Green/g' $files
sed -i 's/Color::YELLOW/Color::Yellow/g' $files
sed -i 's/Color::BLUE/Color::Blue/g' $files
sed -i 's/Color::GRAY/Color::Gray/g' $files
sed -i 's/Color::RESET/Color::Reset/g' $files

sed -i 's/CliArg::Protocol::NONE/CliArg::Protocol::None/g' $files
sed -i 's/CliArg::Protocol::UDP/CliArg::Protocol::Udp/g' $files
sed -i 's/CliArg::Protocol::TCP/CliArg::Protocol::Tcp/g' $files
sed -i 's/CliArg::Protocol::SERIAL/CliArg::Protocol::Serial/g' $files

sed -i 's/FlightMode::UNKNOWN/FlightMode::Unknown/g' $files
sed -i 's/FlightMode::READY/FlightMode::Ready/g' $files
sed -i 's/FlightMode::TAKEOFF/FlightMode::Takeoff/g' $files
sed -i 's/FlightMode::HOLD/FlightMode::Hold/g' $files
sed -i 's/FlightMode::MISSION/FlightMode::Mission/g' $files
sed -i 's/FlightMode::RETURN_TO_LAUNCH/FlightMode::ReturnToLaunch/g' $files
sed -i 's/FlightMode::LAND/FlightMode::Land/g' $files
sed -i 's/FlightMode::OFFBOARD/FlightMode::Offboard/g' $files
sed -i 's/FlightMode::FOLLOW_ME/FlightMode::FollowMe/g' $files
sed -i 's/FlightMode::MANUAL/FlightMode::Manual/g' $files
sed -i 's/FlightMode::ALTCTL/FlightMode::Altctl/g' $files
sed -i 's/FlightMode::POSCTL/FlightMode::Posctl/g' $files
sed -i 's/FlightMode::ACRO/FlightMode::Acro/g' $files
sed -i 's/FlightMode::RATTITUDE/FlightMode::Rattitude/g' $files
sed -i 's/FlightMode::STABILIZED/FlightMode::Stabilized/g' $files


sed -i 's/CalibrationImpl::State::NONE/CalibrationImpl::None/g' $files
sed -i 's/CalibrationImpl::State::GYRO_CALIBRATION/CalibrationImpl::State::GyroCalibration/g' $files
sed -i 's/CalibrationImpl::State::ACCELEROMETER_CALIBRATION/CalibrationImpl::State::AccelerometerCalibration/g' $files
sed -i 's/CalibrationImpl::State::MAGNETOMETER_CALIBRATION/CalibrationImpl::State::MagnetometerCalibration/g' $files
sed -i 's/CalibrationImpl::State::GIMBAL_ACCELEROMETER_CALIBRATION/CalibrationImpl::State::GimbalAccelerometerCalibration/g' $files

sed -i 's/ConnectionResult::SUCCESS/ConnectionResult::Success/g' $files
sed -i 's/ConnectionResult::TIMEOUT/ConnectionResult::Timeout/g' $files
sed -i 's/ConnectionResult::SOCKET_ERROR/ConnectionResult::SocketError/g' $files
sed -i 's/ConnectionResult::BIND_ERROR/ConnectionResult::BindError/g' $files
sed -i 's/ConnectionResult::SOCKET_CONNECTION_ERROR/ConnectionResult::SocketConnectionError/g' $files
sed -i 's/ConnectionResult::CONNECTION_ERROR/ConnectionResult::ConnectionError/g' $files
sed -i 's/ConnectionResult::NOT_IMPLEMENTED/ConnectionResult::NotImplemented/g' $files
sed -i 's/ConnectionResult::SYSTEM_NOT_CONNECTED/ConnectionResult::SystemNotConnected/g' $files
sed -i 's/ConnectionResult::SYSTEM_BUSY/ConnectionResult::SystemBusy/g' $files
sed -i 's/ConnectionResult::COMMAND_DENIED/ConnectionResult::CommandDenied/g' $files
sed -i 's/ConnectionResult::DESTINATION_IP_UNKNOWN/ConnectionResult::DestinationIpUnknown/g' $files
sed -i 's/ConnectionResult::CONNECTIONS_EXHAUSTED/ConnectionResult::ConnectionsExhausted/g' $files
sed -i 's/ConnectionResult::CONNECTION_URL_INVALID/ConnectionResult::ConnectionUrlInvalid/g' $files
sed -i 's/ConnectionResult::BAUDRATE_UNKNOWN/ConnectionResult::BaudrateUnknown/g' $files
