#include "serial_connection.hpp"
#include "mavsdk_impl.hpp"
#include "log.hpp"

#if defined(APPLE) || defined(LINUX)
#include <termios.h>
#include <unistd.h>
#endif

#include <asio/buffer.hpp>
#include <asio/dispatch.hpp>
#include <asio/error.hpp>
#include <asio/post.hpp>
#include <asio/write.hpp>

#include <future>
#include <sstream>
#include <utility>

namespace mavsdk {

#if defined(WINDOWS)
// Taken from:
// https://coolcowstudio.wordpress.com/2012/10/19/getlasterror-as-stdstring/
static std::string GetLastErrorStdStr()
{
    DWORD error = GetLastError();
    if (error == 0) {
        return "";
    }

    LPVOID lpMsgBuf = nullptr;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL);

    if (bufLen) {
        LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
        std::string result(lpMsgStr, lpMsgStr + bufLen);
        LocalFree(lpMsgBuf);

        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
        if (!result.empty() && result.back() == '\r') {
            result.pop_back();
        }

        return result;
    }

    return std::to_string(error);
}
#define GET_ERROR() GetLastErrorStdStr()
#else
#define GET_ERROR() strerror(errno)
#endif

SerialConnection::SerialConnection(
    Connection::ReceiverCallback receiver_callback,
    Connection::LibmavReceiverCallback libmav_receiver_callback,
    MavsdkImpl& mavsdk_impl,
    std::string path,
    int baudrate,
    bool flow_control,
    ForwardingOption forwarding_option) :
    Connection(
        std::move(receiver_callback),
        std::move(libmav_receiver_callback),
        mavsdk_impl,
        forwarding_option),
    _serial_node(std::move(path)),
    _baudrate(baudrate),
    _flow_control(flow_control),
    _serial_port(mavsdk_impl.io_context())
{}

SerialConnection::~SerialConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}

ConnectionResult SerialConnection::start()
{
    if (!start_mavlink_receiver()) {
        return ConnectionResult::ConnectionsExhausted;
    }

    if (!start_libmav_receiver()) {
        return ConnectionResult::ConnectionsExhausted;
    }

    ConnectionResult ret = setup_port();
    if (ret != ConnectionResult::Success) {
        return ret;
    }

    // Kick off the first async read — subsequent ones are re-posted from the handler.
    do_receive();

    return ConnectionResult::Success;
}

ConnectionResult SerialConnection::setup_port()
{
    // Open the port through Asio — this sets up the underlying fd/HANDLE for async I/O.
    asio::error_code ec;
    _serial_port.open(_serial_node, ec);
    if (ec) {
        LogErr("Call open failed: {}", ec.message());
        return ConnectionResult::ConnectionError;
    }

#if defined(LINUX) || defined(APPLE)
    // Apply raw-mode termios settings via the native fd.  Asio opens serial ports with
    // O_NONBLOCK (required for async I/O), so we do NOT clear that flag here.
    struct termios tc;
    bzero(&tc, sizeof(tc));

    const int fd = _serial_port.native_handle();

    if (tcgetattr(fd, &tc) != 0) {
        LogErr("Call tcgetattr failed: {}", GET_ERROR());
        _serial_port.close(ec);
        return ConnectionResult::ConnectionError;
    }

    tc.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    tc.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);
    tc.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG | TOSTOP);
    tc.c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
    tc.c_cflag |= CS8;
    tc.c_cflag |= CLOCAL; // Without this a write() blocks indefinitely.

    // VMIN=0 / VTIME=0: non-blocking raw mode — Asio drives readiness via epoll/kqueue.
    tc.c_cc[VMIN] = 0;
    tc.c_cc[VTIME] = 0;

    if (_flow_control) {
        tc.c_cflag |= CRTSCTS;
    }

#if defined(LINUX)
    const int baudrate_or_define = define_from_baudrate(_baudrate);
#elif defined(APPLE)
    const int baudrate_or_define = _baudrate;
#endif

    if (baudrate_or_define == -1) {
        _serial_port.close(ec);
        return ConnectionResult::BaudrateUnknown;
    }

    if (cfsetispeed(&tc, baudrate_or_define) != 0) {
        LogErr("Call cfsetispeed failed: {}", GET_ERROR());
        _serial_port.close(ec);
        return ConnectionResult::ConnectionError;
    }

    if (cfsetospeed(&tc, baudrate_or_define) != 0) {
        LogErr("Call cfsetospeed failed: {}", GET_ERROR());
        _serial_port.close(ec);
        return ConnectionResult::ConnectionError;
    }

    if (tcsetattr(fd, TCSANOW, &tc) != 0) {
        LogErr("Call tcsetattr failed: {}", GET_ERROR());
        _serial_port.close(ec);
        return ConnectionResult::ConnectionError;
    }

#elif defined(WINDOWS)
    // Apply DCB settings via the native HANDLE.
    HANDLE handle = _serial_port.native_handle();

    DCB dcb;
    SecureZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(handle, &dcb)) {
        LogErr("GetCommState failed with error: {}", GET_ERROR());
        return ConnectionResult::ConnectionError;
    }

    dcb.BaudRate = _baudrate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    if (_flow_control) {
        dcb.fOutxCtsFlow = TRUE;
        dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
    } else {
        dcb.fDtrControl = DTR_CONTROL_DISABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
    }
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fBinary = TRUE;
    dcb.fNull = FALSE;
    dcb.fDsrSensitivity = FALSE;

    if (!SetCommState(handle, &dcb)) {
        LogErr("SetCommState failed with error: {}", GET_ERROR());
        return ConnectionResult::ConnectionError;
    }

    // Asio manages timeouts internally for overlapped I/O; clear COMMTIMEOUTS so
    // the OS never times out a read prematurely.
    COMMTIMEOUTS timeout{};
    if (!SetCommTimeouts(handle, &timeout)) {
        LogErr("SetCommTimeouts failed with error: {}", GET_ERROR());
        return ConnectionResult::ConnectionError;
    }
#endif

    _port_open = true;

    return ConnectionResult::Success;
}

ConnectionResult SerialConnection::stop()
{
    _stopping = true;
    _port_open = false;

    auto& io_ctx = static_cast<asio::io_context&>(_serial_port.get_executor().context());
    if (!io_ctx.stopped()) {
        // Close the serial port from the io_context thread to avoid a data race
        // with a concurrent async_read_some() / async_write() reading the port's state.
        // Because io_ctx is driven by a single thread, posting here serialises
        // the close with all in-flight port access.
        std::promise<void> close_done;
        asio::post(io_ctx, [this, &close_done]() {
            _tx_queue.clear();
            if (_serial_port.is_open()) {
                asio::error_code ec;
                _serial_port.cancel(ec); // cancel outstanding async_read_some/async_write
                _serial_port.close(ec);
            }
            close_done.set_value();
        });
        close_done.get_future().wait();

        // Drain any operation_aborted handlers queued by the close. This also means the
        // in-flight write (if any) has completed, so its buffer is no longer referenced.
        std::promise<void> fence;
        asio::post(io_ctx, [&fence]() { fence.set_value(); });
        fence.get_future().wait();
    } else {
        // io_context already stopped — no concurrent async operations are running.
        _tx_queue.clear();
        if (_serial_port.is_open()) {
            asio::error_code ec;
            _serial_port.cancel(ec);
            _serial_port.close(ec);
        }
    }

    // Stop after stopping the port so we don't interfere with message parsing.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

std::pair<bool, std::string> SerialConnection::send_message(const mavlink_message_t& message)
{
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);
    return send_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

std::pair<bool, std::string> SerialConnection::send_raw_bytes(const char* bytes, size_t length)
{
    if (_serial_node.empty()) {
        return {false, "Dev Path unknown"};
    }

    if (_baudrate == 0) {
        return {false, "Baudrate unknown"};
    }

    if (!_port_open) {
        return {false, "Port not open"};
    }

    // Queue the bytes and let the async write chain drain them on the io thread. A
    // synchronous write here would hold the io thread for as long as the link needs to
    // drain the bytes -- at 57600 baud a single MAVLink frame is already tens of
    // milliseconds -- stalling every other connection, timer and plugin meanwhile.
    //
    // The flip side is that a write failure is only known later; it reaches the user
    // through report_send_error() instead of this return value.
    std::vector<char> buf(bytes, bytes + length);

    // dispatch() rather than post(): callers are normally already on the io thread (the
    // send path runs there), and running inline keeps the enqueue ordered with respect
    // to the writes already queued.
    asio::dispatch(_serial_port.get_executor(), [this, buf = std::move(buf)]() mutable {
        if (_stopping || !_serial_port.is_open()) {
            return;
        }

        const auto dropped = _tx_queue.push(std::move(buf));
        if (dropped > 0) {
            LogErr("Serial send queue full, dropped {} message(s)", dropped);
            report_send_error("Send queue full, dropped oldest message(s)");
        }

        start_write();
    });

    return {true, {}};
}

void SerialConnection::start_write()
{
    // Runs on the io thread. One write is in flight at a time; its completion handler
    // starts the next.
    if (_tx_queue.busy() || _tx_queue.empty()) {
        return;
    }

    if (_stopping || !_serial_port.is_open()) {
        _tx_queue.clear();
        return;
    }

    const auto& item = _tx_queue.start();
    asio::async_write(
        _serial_port, asio::buffer(item), [this](const asio::error_code& ec, std::size_t) {
            _tx_queue.finish();

            if (ec) {
                if (ec != asio::error::operation_aborted && !_stopping) {
                    const std::string msg = "write failure: " + ec.message();
                    LogErr("{}", msg);
                    report_send_error(msg);
                }
                _tx_queue.clear();
                return;
            }

            start_write();
        });
}

void SerialConnection::do_receive()
{
    // Post an async read. The handler runs on the dedicated io_context thread (_io_thread).
    _serial_port.async_read_some(
        asio::buffer(_recv_buffer), [this](const asio::error_code& ec, std::size_t recv_len) {
            if (ec == asio::error::operation_aborted) {
                // stop() was called — do not re-arm.
                return;
            }

            if (ec == asio::error::eof) {
                // Asio maps a 0-byte read() to EOF. On serial ports with
                // VMIN=0/VTIME=0 this can happen transiently (epoll signals
                // readable but the driver has no bytes, BREAK, USB hiccup,
                // etc.) — re-arm and keep going. A truly detached device will
                // surface as a different error on the next read.
                do_receive();
                return;
            }

            if (ec) {
                LogErr("Read failure: {}", ec.message());
                // Do not re-arm on hard errors (port removed, etc.). Nothing more will
                // go out either, so stop accepting sends and drop what is queued.
                _port_open = false;
                _tx_queue.clear();
                return;
            }

            if (recv_len == 0) {
                // No data yet — re-arm immediately.
                do_receive();
                return;
            }

            _mavlink_receiver->set_new_datagram(_recv_buffer.data(), static_cast<int>(recv_len));

            auto parse_result = _mavlink_receiver->parse_message();
            while (parse_result != MavlinkReceiver::ParseResult::NoneAvailable) {
                receive_message(parse_result, _mavlink_receiver->get_last_message(), this);
                parse_result = _mavlink_receiver->parse_message();
            }

            if (_libmav_receiver) {
                _libmav_receiver->set_new_datagram(_recv_buffer.data(), static_cast<int>(recv_len));
                while (_libmav_receiver->parse_message()) {
                    receive_libmav_message(_libmav_receiver->get_last_message(), this);
                }
            }

            // Re-arm for the next chunk.
            do_receive();
        });
}

#if defined(LINUX)
int SerialConnection::define_from_baudrate(int baudrate)
{
    switch (baudrate) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            LogErr("Unknown baudrate");
            return -1;
    }
}
#endif

} // namespace mavsdk
