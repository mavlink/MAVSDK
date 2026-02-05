#include "calibration_statustext_parser.h"
#include "calibration_messages.h"
#include "log.h"

namespace mavsdk {

CalibrationStatustextParser::CalibrationStatustextParser() {}

CalibrationStatustextParser::~CalibrationStatustextParser() {}

bool CalibrationStatustextParser::parse(const std::string& statustext)
{
    // We do a quick check before doing more in-depth parsing.
    if (!is_relevant(statustext)) {
        return false;
    }

    // We start with the most likely messages in order to reduce parsing efforts.
    // As soon as one check is successful, the condition is true and we can stop.
    if (check_progress(statustext) || check_started(statustext) || check_done(statustext) ||
        check_failed(statustext) || check_cancelled(statustext)) {
        return true;
    }

    // Whatever doesn't fit into the first checks will end up as a generic
    // instruction and caught at the end.
    check_instruction(statustext);
    return true;
}

void CalibrationStatustextParser::reset()
{
    _status = Status::None;
    _progress = NAN;
    _failed_message.clear();
    _instruction_message.clear();
}

bool CalibrationStatustextParser::is_relevant(const std::string& statustext)
{
    // This should be a quick check, so pre-processing without looking at the whole string.
    static constexpr char CALIBRATION_PREFIX[] = "[cal] ";
    return (statustext.compare(0, 6, CALIBRATION_PREFIX) == 0);
}

bool CalibrationStatustextParser::check_started(const std::string& statustext)
{
    static constexpr char CUSTOM_CAL_QGC_STARTED_MSG[] = "[cal] calibration started: %i %s";

    int version_stamp;
    const int ret =
        sscanf(statustext.c_str(), CUSTOM_CAL_QGC_STARTED_MSG, &version_stamp, _tmp_str);

    if (ret == 2) {
        if (version_stamp == 2) {
            _status = Status::Started;
        } else {
            _status = Status::Failed;

            std::stringstream error_stream{};
            error_stream << "Unknown calibration version stamp: " << version_stamp;
            _failed_message = error_stream.str();
            LogErr() << _failed_message;
        }
        return true;
    }

    return false;
}

bool CalibrationStatustextParser::check_done(const std::string& statustext)
{
    const int ret = sscanf(statustext.c_str(), CAL_QGC_DONE_MSG, _tmp_str);

    if (ret == 1) {
        _status = Status::Done;
        return true;
    }
    return false;
}

bool CalibrationStatustextParser::check_failed(const std::string& statustext)
{
    static constexpr char CUSTOM_CAL_QGC_FAILED_MSG[] = "[cal] calibration failed: %63[^\n]";

    const int ret = sscanf(statustext.c_str(), CUSTOM_CAL_QGC_FAILED_MSG, _tmp_str);

    if (ret == 1) {
        _status = Status::Failed;
        _failed_message = _tmp_str;
        return true;
    }
    return false;
}

bool CalibrationStatustextParser::check_cancelled(const std::string& statustext)
{
    if (statustext.compare(CAL_QGC_CANCELLED_MSG) == 0) {
        _status = Status::Cancelled;
        return true;
    }
    return false;
}

bool CalibrationStatustextParser::check_progress(const std::string& statustext)
{
    int progress_int;
    int ret = sscanf(statustext.c_str(), CAL_QGC_PROGRESS_MSG, &progress_int);

    if (ret == 1 && progress_int >= 0 && progress_int <= 100) {
        _progress = float(progress_int) / 100;
        _status = Status::Progress;
        return true;
    }

    static constexpr char SPECIAL_MAG_CAL_QGC_PROGRESS_MSG[] =
        "[cal] %s side calibration: progress <%u>";

    ret = sscanf(statustext.c_str(), SPECIAL_MAG_CAL_QGC_PROGRESS_MSG, _tmp_str, &progress_int);

    if (ret == 2 && progress_int >= 0 && progress_int <= 100) {
        _progress = float(progress_int) / 100;
        _status = Status::Progress;
        return true;
    }

    return false;
}

bool CalibrationStatustextParser::check_instruction(const std::string& statustext)
{
    static constexpr char CATCHALL[] = "[cal] %63[^\n]";

    const int ret = sscanf(statustext.c_str(), CATCHALL, _tmp_str);

    if (ret == 1) {
        _status = Status::Instruction;
        _instruction_message = _tmp_str;
        return true;
    }
    return false;
}

} // namespace mavsdk
