#include "hostname_to_ip.h"
#include "log.h"

#if defined(WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

namespace mavsdk {

std::optional<std::string> resolve_hostname_to_ip(const std::string& hostname)
{
#if defined(WINDOWS)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return {};
    }
#endif

    addrinfo hints{};
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo* result = nullptr;
    int res = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
    if (res != 0) {
#if defined(WINDOWS)
        LogErr() << "getaddrinfo failed: " << WSAGetLastError();
        WSACleanup();
#else
        LogErr() << "getaddrinfo failed: " << gai_strerror(res);
#endif
        return {};
    }

    std::string ipAddress;
    for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        sockaddr_in* sockaddrIpv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr);
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sockaddrIpv4->sin_addr), ipStr, INET_ADDRSTRLEN);
        ipAddress = ipStr;
        break; // Take the first result
    }

    freeaddrinfo(result);
#if defined(WINDOWS)
    WSACleanup();
#endif
    return {ipAddress};
}

} // namespace mavsdk
