#include "tune_impl.h"
#include "log.h"
#include "mavlink_address.h"

namespace mavsdk {

TuneImpl::TuneImpl(System& system) : PluginImplBase(system), _mavlink_tune_item_messages()
{
    _system_impl->register_plugin(this);
}

TuneImpl::TuneImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system)),
    _mavlink_tune_item_messages()
{
    _system_impl->register_plugin(this);
}

TuneImpl::~TuneImpl()
{
    _system_impl->unregister_plugin(this);
}

void TuneImpl::init() {}

void TuneImpl::deinit() {}

void TuneImpl::enable() {}

void TuneImpl::disable() {}

Tune::Result TuneImpl::play_tune(const Tune::TuneDescription& tune)
{
    std::promise<Tune::Result> prom;
    auto fut = prom.get_future();

    play_tune_async(tune, [&prom](const Tune::Result result) { prom.set_value(result); });
    return fut.get();
}

void TuneImpl::play_tune_async(
    const Tune::TuneDescription& tune, const Tune::ResultCallback& callback)
{
    const auto song_elements = tune.song_elements;
    const int tempo = tune.tempo;

    if (tempo < 32 || tempo > 255) {
        report_tune_result(callback, Tune::Result::InvalidTempo);
        return;
    }

    std::string tune_str("MFT" + std::to_string(tempo) + "O2");

    // We need to reserve enough because inside the mavlink pack
    // function it does a memcpy of the full length.
    tune_str.reserve(MAVLINK_MSG_PLAY_TUNE_V2_FIELD_TUNE_LEN);

    int last_duration = 1;

    for (auto song_elem : song_elements) {
        switch (song_elem) {
            case Tune::SongElement::StyleLegato:
                tune_str.append("ML");
                break;
            case Tune::SongElement::StyleNormal:
                tune_str.append("MN");
                break;
            case Tune::SongElement::StyleStaccato:
                tune_str.append("MS");
                break;
            case Tune::SongElement::Duration1:
                tune_str.append("L1");
                last_duration = 1;
                break;
            case Tune::SongElement::Duration2:
                tune_str.append("L2");
                last_duration = 2;
                break;
            case Tune::SongElement::Duration4:
                tune_str.append("L4");
                last_duration = 4;
                break;
            case Tune::SongElement::Duration8:
                tune_str.append("L8");
                last_duration = 8;
                break;
            case Tune::SongElement::Duration16:
                tune_str.append("L16");
                last_duration = 16;
                break;
            case Tune::SongElement::Duration32:
                tune_str.append("L32");
                last_duration = 32;
                break;
            case Tune::SongElement::NoteA:
                tune_str.append("A");
                break;
            case Tune::SongElement::NoteB:
                tune_str.append("B");
                break;
            case Tune::SongElement::NoteC:
                tune_str.append("C");
                break;
            case Tune::SongElement::NoteD:
                tune_str.append("D");
                break;
            case Tune::SongElement::NoteE:
                tune_str.append("E");
                break;
            case Tune::SongElement::NoteF:
                tune_str.append("F");
                break;
            case Tune::SongElement::NoteG:
                tune_str.append("G");
                break;
            case Tune::SongElement::NotePause:
                tune_str.append("P" + std::to_string(last_duration));
                break;
            case Tune::SongElement::Sharp:
                tune_str.append("+");
                break;
            case Tune::SongElement::Flat:
                tune_str.append("-");
                break;
            case Tune::SongElement::OctaveUp:
                tune_str.append(">");
                break;
            case Tune::SongElement::OctaveDown:
                tune_str.append("<");
                break;
            default:
                break;
        }
    }

    if (tune_str.size() > MAVLINK_MSG_PLAY_TUNE_V2_FIELD_TUNE_LEN - 1) {
        report_tune_result(callback, Tune::Result::TuneTooLong);
        return;
    }

    if (!_system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_play_tune_v2_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _system_impl->get_system_id(),
                _system_impl->get_autopilot_id(),
                TUNE_FORMAT_QBASIC1_1,
                tune_str.c_str());
            return message;
        })) {
        report_tune_result(callback, Tune::Result::Error);
        return;
    }

    report_tune_result(callback, Tune::Result::Success);
}

void TuneImpl::report_tune_result(const Tune::ResultCallback& callback, Tune::Result result)
{
    if (callback == nullptr) {
        LogWarn() << "Callback is not set";
        return;
    }

    _system_impl->call_user_callback([callback, result]() { callback(result); });
}

} // namespace mavsdk
