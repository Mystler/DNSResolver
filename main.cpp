#include "DNS.h"
#include <cstdio>
#include <ctime>

#ifdef BUILD_WIN32
#   include <winsock2.h>
#   define close(x) closesocket(x)
#else
#   include <sys/socket.h>
#endif

#define NAMESERVER "8.8.8.8"
#define PORT 53
#define MAX_PACKET_LENGTH 65536

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: DNSResolver <hostname>");
        return 1;
    }

#ifdef BUILD_WIN32
    // HAAX
    WSADATA WsaDat;
    WSAStartup(MAKEWORD(2, 2), &WsaDat);
#endif

    // Start stopwatch
    clock_t time_start = clock();

    // Create our socket and connect to the NS
    auto sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in ns_addr;
    ns_addr.sin_family = AF_INET;
    ns_addr.sin_port = htons(PORT);
    ns_addr.sin_addr.s_addr = inet_addr(NAMESERVER);
    if (connect(sock, (const sockaddr*)&ns_addr, sizeof(ns_addr)) != 0) {
        printf("Connection error!");
        return 2;
    }

    // Create our buffer
    size_t length = 0;
    char buf[MAX_PACKET_LENGTH];

    DNSPacket dns(argv[1]);
    dns.WriteToNetBuffer(buf, length);

    // Send that stuff to the NS
    send(sock, buf, length, 0);
    // Wait for the answer and overwrite our buffer
    recv(sock, buf, MAX_PACKET_LENGTH, 0);

    // Read what we got
    dns.ReadFromNetBuffer(buf);

    // Close our socket
    close(sock);

    // Stop stopwatch
    uint32_t ms = static_cast<uint32_t>((clock() - time_start) * 1000.f / CLOCKS_PER_SEC);
    printf("Time: %ums", ms);

#ifdef BUILD_WIN32
    // Even moar haxxes
    WSACleanup();
#endif

    return 0;
}
