#pragma once

#include <string>
#include <cmath>

namespace mavsdk {

class CalibrationStatustextParser {
public:
    CalibrationStatustextParser();
    ~CalibrationStatustextParser();

    // Delete copy and move constructors and assign operators.
    CalibrationStatustextParser(CalibrationStatustextParser const&) = delete;
    CalibrationStatustextParser(CalibrationStatustextParser&&) = delete;
    CalibrationStatustextParser& operator=(CalibrationStatustextParser const&) = delete;
    CalibrationStatustextParser& operator=(CalibrationStatustextParser&&) = delete;

    enum class Status { None, Started, Done, Failed, Cancelled, Progress, Instruction };

    void reset();
    bool parse(const std::string& statustext);
    Status get_status() const { return _status; }
    float get_progress() const { return _progress; }
    const std::string& get_failed_message() const { return _failed_message; }
    const std::string& get_instruction() const { return _instruction_message; }

private:
    bool is_relevant(const std::string& statustext);
    bool check_started(const std::string& statustext);
    bool check_done(const std::string& statustext);
    bool check_failed(const std::string& statustext);
    bool check_cancelled(const std::string& statustext);
    bool check_progress(const std::string& statustext);
    bool check_instruction(const std::string& statustext);

    Status _status{Status::None};
    float _progress{NAN};
    std::string _failed_message{};
    std::string _instruction_message{};

    char _tmp_str[64];
};

} // namespace mavsdk
