#include "mavlink_receiver.h"
#include "global_include.h"

#if DROP_DEBUG ==1
#include <iomanip>
#endif

namespace dronecore {

MAVLinkReceiver::MAVLinkReceiver(uint8_t channel) :
    _channel(channel)
#if DROP_DEBUG == 1
    , _last_time()
#endif
{
}

void MAVLinkReceiver::set_new_datagram(char *datagram, unsigned datagram_len)
{
    _datagram = datagram;
    _datagram_len = datagram_len;

#if DROP_DEBUG == 1
    _bytes_received += _datagram_len;
#endif
}

bool MAVLinkReceiver::parse_message()
{
    // Note that one datagram can contain multiple mavlink messages.
    for (unsigned i = 0; i < _datagram_len; ++i) {
        if (mavlink_parse_char(_channel, _datagram[i], &_last_message, &_status) == 1) {

            // Move the pointer to the datagram forward by the amount parsed.
            _datagram += (i + 1);
            // And decrease the length, so we don't overshoot in the next round.
            _datagram_len -= (i + 1);

#if DROP_DEBUG == 1
            debug_drop_rate();
#endif

            // We have parsed one message, let's return so it can be handled.
            return true;
        }
    }

    // No (more) messages, let's give up.
    _datagram = nullptr;
    _datagram_len = 0;
    return false;
}

#if DROP_DEBUG == 1
void MAVLinkReceiver::debug_drop_rate()
{
    if (_last_message.msgid == MAVLINK_MSG_ID_SYS_STATUS) {

        const unsigned msg_len = (_last_message.len + MAVLINK_NUM_NON_PAYLOAD_BYTES);

        _bytes_received -= msg_len;

        mavlink_sys_status_t sys_status;
        mavlink_msg_sys_status_decode(&_last_message, &sys_status);


        if (!_first) {

            LogDebug() << "-------------------------------------------------------------------"
                       << "-----------";

            if (_bytes_received <= sys_status.errors_comm &&
                sys_status.errors_count2 <= sys_status.errors_comm) {

                _bytes_sent_overall += sys_status.errors_comm;
                //_bytes_at_gimbal_overall += sys_status.errors_count1;
                _bytes_at_camera_overall += sys_status.errors_count2;
                _bytes_at_sdk_overall += _bytes_received;

                _time_elapsed += elapsed_since_s(_last_time);

                print_line("FMU   ", sys_status.errors_comm, sys_status.errors_comm,
                           _bytes_sent_overall, _bytes_sent_overall);
                //print_line("Gimbal", sys_status.errors_count1, sys_status.errors_comm,
                //           _bytes_at_gimbal_overall, _bytes_sent_overall);
                print_line("Camera", sys_status.errors_count2, sys_status.errors_comm,
                           _bytes_at_camera_overall, _bytes_sent_overall);
                print_line("SDK   ", _bytes_received, sys_status.errors_comm,
                           _bytes_at_sdk_overall, _bytes_sent_overall);

            } else {
                LogDebug() << "Missed SYS_STATUS";
            }
        }
        _first = false;

        _last_time = steady_time();

        // Reset afterwards:
        _bytes_received = msg_len;
    }
}

void MAVLinkReceiver::print_line(const char *index, unsigned count, unsigned count_total,
                                 unsigned overall_bytes, unsigned overall_bytes_total)
{
    LogDebug() << "count "
               << index
               << ": "
               << std::setw(6)
               << count
               << ", loss: "
               << std::setw(6)
               << count_total -  count
               << ",  "
               << std::setw(6)
               << std::setprecision(2)
               << std::fixed
               << 100.0 * float(count) / float(count_total)
               << " %, overall: "
               << std::setw(6)
               << std::setprecision(2)
               << std::fixed
               << (100.0 * double(overall_bytes) / double(overall_bytes_total))
               << " %, "
               << std::setw(6)
               << std::setprecision(2)
               << std::fixed
               << (double(overall_bytes) / _time_elapsed / 1024.0)
               << " KiB/s";
}
#endif

} // namespace dronecore
