#include "tune_impl.h"
#include "global_include.h"
#include "log.h"

namespace mavsdk {

TuneImpl::TuneImpl(System& system) : PluginImplBase(system), _mavlink_tune_item_messages()
{
    _parent->register_plugin(this);
}

TuneImpl::~TuneImpl()
{
    _parent->unregister_plugin(this);
}

void TuneImpl::init() {}

void TuneImpl::deinit() {}

void TuneImpl::enable() {}

void TuneImpl::disable() {}

void TuneImpl::play_tune_async(
    const std::vector<Tune::SongElement>& tune,
    const int tempo,
    const Tune::result_callback_t& callback)
{
    if (tempo < 32 || tempo > 255) {
        report_tune_result(callback, Tune::Result::INVALID_TEMPO);
        return;
    }

    std::string tune_str("MFT" + std::to_string(tempo) + "O2");
    int last_duration = 1;

    for (auto song_elem : tune) {
        switch (song_elem) {
            case Tune::SongElement::STYLE_LEGATO:
                tune_str.append("ML");
                break;
            case Tune::SongElement::STYLE_NORMAL:
                tune_str.append("MN");
                break;
            case Tune::SongElement::STYLE_STACCATO:
                tune_str.append("MS");
                break;
            case Tune::SongElement::DURATION_1:
                tune_str.append("L1");
                last_duration = 1;
                break;
            case Tune::SongElement::DURATION_2:
                tune_str.append("L2");
                last_duration = 2;
                break;
            case Tune::SongElement::DURATION_4:
                tune_str.append("L4");
                last_duration = 4;
                break;
            case Tune::SongElement::DURATION_8:
                tune_str.append("L8");
                last_duration = 8;
                break;
            case Tune::SongElement::DURATION_16:
                tune_str.append("L16");
                last_duration = 16;
                break;
            case Tune::SongElement::DURATION_32:
                tune_str.append("L32");
                last_duration = 32;
                break;
            case Tune::SongElement::NOTE_A:
                tune_str.append("A");
                break;
            case Tune::SongElement::NOTE_B:
                tune_str.append("B");
                break;
            case Tune::SongElement::NOTE_C:
                tune_str.append("C");
                break;
            case Tune::SongElement::NOTE_D:
                tune_str.append("D");
                break;
            case Tune::SongElement::NOTE_E:
                tune_str.append("E");
                break;
            case Tune::SongElement::NOTE_F:
                tune_str.append("F");
                break;
            case Tune::SongElement::NOTE_G:
                tune_str.append("G");
                break;
            case Tune::SongElement::NOTE_PAUSE:
                tune_str.append("P" + std::to_string(last_duration));
                break;
            case Tune::SongElement::SHARP:
                tune_str.append("+");
                break;
            case Tune::SongElement::FLAT:
                tune_str.append("-");
                break;
            case Tune::SongElement::OCTAVE_UP:
                tune_str.append(">");
                break;
            case Tune::SongElement::OCTAVE_DOWN:
                tune_str.append("<");
                break;
            default:
                break;
        }
    }

    LogDebug() << "About to send tune: " << tune_str;

    if (tune_str.size() > 230) {
        report_tune_result(callback, Tune::Result::TUNE_TOO_LONG);
        return;
    }

    std::string tune1_str;
    std::string tune2_str;

    if (tune_str.size() < 30) {
        tune1_str = tune_str;
        tune2_str = "";
    } else {
        tune1_str = tune_str.substr(0, 30);
        tune2_str = tune_str.substr(30);
    }

    mavlink_message_t message;
    mavlink_msg_play_tune_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        tune1_str.c_str(),
        tune2_str.c_str());

    if (!_parent->send_message(message)) {
        report_tune_result(callback, Tune::Result::ERROR);
        return;
    }

    report_tune_result(callback, Tune::Result::SUCCESS);
}

void TuneImpl::report_tune_result(const Tune::result_callback_t& callback, Tune::Result result)
{
    if (callback == nullptr) {
        LogWarn() << "Callback is not set";
        return;
    }

    _parent->call_user_callback([callback, result]() { callback(result); });
}

} // namespace mavsdk
