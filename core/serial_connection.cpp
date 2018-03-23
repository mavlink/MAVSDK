#if !defined(WINDOWS) && !defined(APPLE)
#include "serial_connection.h"
#include "global_include.h"
#include "log.h"

#ifndef WINDOWS
#include <asm/termbits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include <cassert>

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace dronecore {

SerialConnection::SerialConnection(DroneCoreImpl &parent, const std::string &path, int baudrate):
    Connection(parent),
    _serial_node(path),
    _baudrate(baudrate)
{
    if (baudrate == 0) {
        _baudrate = DEFAULT_SERIAL_BAUDRATE;
    }
    if (path == "") {
        _serial_node = DEFAULT_SERIAL_DEV_PATH;
    }
}

SerialConnection::~SerialConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}


bool SerialConnection::is_ok() const
{
    return true;
}

ConnectionResult SerialConnection::start()
{
    if (!start_mavlink_receiver()) {
        return ConnectionResult::CONNECTIONS_EXHAUSTED;
    }

    ConnectionResult ret = setup_port();
    if (ret != ConnectionResult::SUCCESS) {
        return ret;
    }

    start_recv_thread();

    return ConnectionResult::SUCCESS;
}

ConnectionResult SerialConnection::setup_port()
{
    struct termios2 tc;

    bzero(&tc, sizeof(tc));

    _fd = open(_serial_node.c_str(), O_RDWR | O_NOCTTY);
    if (_fd == -1) {
        return ConnectionResult::CONNECTION_ERROR;
    }
    if (ioctl(_fd, TCGETS2, &tc) == -1) {
        LogErr() << "Could not get termios2 " << GET_ERROR(errno);
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }

    tc.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    tc.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);
    tc.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG | TOSTOP);
    tc.c_cflag &= ~(CSIZE | PARENB | CBAUD | CRTSCTS);
    tc.c_cflag |= CS8 | BOTHER;

    tc.c_cc[VMIN] = 1; // We want at least 1 byte to be available.
    tc.c_cc[VTIME] = 0; // We don't timeout but wait indefinitely.
    tc.c_ispeed = _baudrate;
    tc.c_ospeed = _baudrate;

    if (ioctl(_fd, TCSETS2, &tc) == -1) {
        LogErr() << "Could not set terminal attributes " << GET_ERROR(errno);
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }

    if (ioctl(_fd, TCFLSH, TCIOFLUSH) == -1) {
        LogErr() << "Could not flush terminal " << GET_ERROR(errno);
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }
    return ConnectionResult::SUCCESS;
}

void SerialConnection::start_recv_thread()
{
    _recv_thread = new std::thread(receive, this);
}

ConnectionResult SerialConnection::stop()
{
    _should_exit = true;
    //TODO for windows
    close(_fd);

    if (_recv_thread) {
        _recv_thread->join();
        delete _recv_thread;
        _recv_thread = nullptr;
    }

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::SUCCESS;
}

bool SerialConnection::send_message(const mavlink_message_t &message, uint8_t, uint8_t)
{
    if (_serial_node.empty()) {
        LogErr() << "Dev Path unknown";
        return false;
    }

    if (_baudrate == 0) {
        LogErr() << "Baudrate unknown";
        return false;
    }

    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

    int send_len =  write(_fd, buffer, buffer_len);

    if (send_len != buffer_len) {
        LogErr() << "write failure: " << GET_ERROR(errno);
        return false;
    }

    return true;
}

void SerialConnection::receive(SerialConnection *parent)
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!parent->_should_exit) {
        int recv_len = read(parent->_fd, buffer, sizeof(buffer));
        if (recv_len < -1) {
            LogErr() << "read failure: " << GET_ERROR(errno);
        }
        if (recv_len > static_cast<int>(sizeof(buffer)) || recv_len == 0) {
            continue;
        }
        parent->_mavlink_receiver->set_new_datagram(buffer, recv_len);
        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (parent->_mavlink_receiver->parse_message()) {
            parent->receive_message(parent->_mavlink_receiver->get_last_message());
        }
    }
}
} // namespace dronecore
#endif
