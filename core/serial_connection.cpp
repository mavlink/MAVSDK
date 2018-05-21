#include "serial_connection.h"
#include "global_include.h"
#include "log.h"


#if defined(LINUX)
#include <unistd.h>
#include <fcntl.h>
#include <asm/termbits.h>
#include <sys/ioctl.h>

#elif defined(APPLE)
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif

#ifndef WINDOWS
#define GET_ERROR() strerror(errno)
#else
#define GET_ERROR() GetLastErrorStdStr()
// Taken from:
// https://coolcowstudio.wordpress.com/2012/10/19/getlasterror-as-stdstring/
std::string GetLastErrorStdStr()
{
    DWORD error = GetLastError();
    if (error) {
        LPVOID lpMsgBuf;
        DWORD bufLen = FormatMessage(
                           FORMAT_MESSAGE_ALLOCATE_BUFFER |
                           FORMAT_MESSAGE_FROM_SYSTEM |
                           FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL,
                           error,
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPTSTR) &lpMsgBuf,
                           0, NULL);
        if (bufLen) {
            LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
            std::string result(lpMsgStr, lpMsgStr + bufLen);

            LocalFree(lpMsgBuf);

            return result;
        }
    }
    return std::string();
}
#endif

namespace dronecore {

SerialConnection::SerialConnection(DroneCoreImpl &parent, const std::string &path, int baudrate):
    Connection(parent),
    _serial_node(path),
    _baudrate(baudrate) {}

SerialConnection::~SerialConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
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
#if defined(LINUX)
    _fd = open(_serial_node.c_str(), O_RDWR | O_NOCTTY);
    if (_fd == -1) {
        LogErr() << "open failed: " << GET_ERROR();
        return ConnectionResult::CONNECTION_ERROR;
    }
#elif defined(APPLE)
    // open() hangs on macOS unless you give it O_NONBLOCK
    _fd = open(_serial_node.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (_fd == -1) {
        LogErr() << "open failed: " << GET_ERROR();
        return ConnectionResult::CONNECTION_ERROR;
    }
    // We need to clear the O_NONBLOCK again because we can block while reading
    // as we do it in a separate thread.
    if (fcntl(_fd, F_SETFL, 0) == -1) {
        LogErr() << "fcntl failed: " << GET_ERROR();
        return ConnectionResult::CONNECTION_ERROR;
    }
#elif defined(WINDOWS)
    _handle = CreateFile(_serial_node.c_str(),
                         GENERIC_READ | GENERIC_WRITE,
                         0,      // exclusive-access
                         NULL,   //  default security attributes
                         OPEN_EXISTING,
                         0,      //  not overlapped I/O
                         NULL);  //  hTemplate must be NULL for comm devices

    if (_handle == INVALID_HANDLE_VALUE) {
        LogErr() << "CreateFile failed with: " << GET_ERROR();
        return ConnectionResult::CONNECTION_ERROR;
    }
#endif

#if defined(LINUX)
    struct termios2 tc;
    bzero(&tc, sizeof(tc));

    if (ioctl(_fd, TCGETS2, &tc) == -1) {
        LogErr() << "Could not get termios2 " << GET_ERROR();
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }
#elif defined(APPLE)
    struct termios tc;
    bzero(&tc, sizeof(tc));

    if (tcgetattr(_fd, &tc) != 0) {
        LogErr() << "tcgetattr failed: " << GET_ERROR();
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }
#endif

#if defined(LINUX) || defined(APPLE)
    tc.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    tc.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);
    tc.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG | TOSTOP);
    tc.c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
    tc.c_cflag |= CS8;

    tc.c_cc[VMIN] = 1; // We want at least 1 byte to be available.
    tc.c_cc[VTIME] = 0; // We don't timeout but wait indefinitely.
#endif

#if defined(LINUX)
    // CBAUD and BOTHER don't seem to be available for macOS with termios.
    tc.c_cflag &= ~(CBAUD);
    tc.c_cflag |= BOTHER;

    if (ioctl(_fd, TCSETS2, &tc) == -1) {
        LogErr() << "Could not set terminal attributes " << GET_ERROR();
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }


    if (ioctl(_fd, TCFLSH, TCIOFLUSH) == -1) {
        LogErr() << "Could not flush terminal " << GET_ERROR();
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }
#elif defined(APPLE)
    tc.c_cflag |= CLOCAL; // Without this a write() blocks indefinitely.

    cfsetispeed(&tc, _baudrate);
    cfsetospeed(&tc, _baudrate);

    if (tcsetattr(_fd, TCSANOW, &tc) != 0) {
        LogErr() << "tcsetattr failed: " << GET_ERROR();
        close(_fd);
        return ConnectionResult::CONNECTION_ERROR;
    }
#endif

#if defined(WINDOWS)
    DCB dcb;
    SecureZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(_handle, &dcb)) {
        LogErr() << "GetCommState failed with error: " << GET_ERROR();
        return ConnectionResult::CONNECTION_ERROR;
    }

    dcb.BaudRate = _baudrate;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fBinary = TRUE;
    dcb.fNull = FALSE;
    dcb.fDsrSensitivity = FALSE;


    if (!SetCommState(_handle, &dcb)) {
        LogErr() << "SetCommState failed with error: " <<  GET_ERROR();
        return ConnectionResult::CONNECTION_ERROR;
    }

    COMMTIMEOUTS timeout = { 0 };
    timeout.ReadIntervalTimeout = 1;
    timeout.ReadTotalTimeoutConstant = 1;
    timeout.ReadTotalTimeoutMultiplier = 1;
    timeout.WriteTotalTimeoutConstant = 1;
    timeout.WriteTotalTimeoutMultiplier = 1;
    SetCommTimeouts(_handle, &timeout);

    if (!SetCommTimeouts(_handle, &timeout)) {
        LogErr() << "SetCommTimeouts failed with error: " <<  GET_ERROR();
        return ConnectionResult::CONNECTION_ERROR;
    }

#endif

    return ConnectionResult::SUCCESS;
}

void SerialConnection::start_recv_thread()
{
    _recv_thread = new std::thread(&SerialConnection::receive, this);
}

ConnectionResult SerialConnection::stop()
{
    _should_exit = true;
#if defined(LINUX) || defined(APPLE)
    close(_fd);
#elif defined(WINDOWS)
    CloseHandle(_handle);
#endif

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

bool SerialConnection::send_message(const mavlink_message_t &message)
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

    int send_len;
#if defined(LINUX) || defined(APPLE)
    send_len =  write(_fd, buffer, buffer_len);
#else
    if (!WriteFile(_handle, buffer, buffer_len, LPDWORD(&send_len), NULL)) {
        LogErr() << "WriteFile failure: " << GET_ERROR();
        return false;
    }
#endif

    if (send_len != buffer_len) {
        LogErr() << "write failure: " << GET_ERROR();
        return false;
    }

    return true;
}

void SerialConnection::receive()
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!_should_exit) {
        int recv_len;
#if defined(LINUX) || defined(APPLE)
        recv_len = read(_fd, buffer, sizeof(buffer));
        if (recv_len < -1) {
            LogErr() << "read failure: " << GET_ERROR();
        }
#else
        if (!ReadFile(_handle, buffer, sizeof(buffer), LPDWORD(&recv_len), NULL)) {
            LogErr() << "ReadFile failure: " << GET_ERROR();
            continue;
        }
#endif
        if (recv_len > static_cast<int>(sizeof(buffer)) || recv_len == 0) {
            continue;
        }
        _mavlink_receiver->set_new_datagram(buffer, recv_len);
        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (_mavlink_receiver->parse_message()) {
            receive_message(_mavlink_receiver->get_last_message());
        }
    }
}
} // namespace dronecore
