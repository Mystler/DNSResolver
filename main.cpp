#include "DNS.h"
#include <cstdio>
#include <ctime>

#ifdef BUILD_WIN32
#   include <winsock2.h>
#   define close(x) closesocket(x)
#else
#   include <unistd.h>
#   include <arpa/inet.h>
#   include <sys/socket.h>
#endif

#define NAMESERVER "8.8.8.8"
#define PORT 53
#define MAX_PACKET_LENGTH 65536

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: DNSResolver <hostname>\n");
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
        printf("Connection error!\n");
        return 2;
    }

    // Create our buffer
    size_t length = 0;
    uint8_t buf[MAX_PACKET_LENGTH];

    DNSPacket dns(argv[1]);
    dns.WriteQuestion(buf, length);

    // Send that stuff to the NS
    send(sock, (const char*)buf, length, 0);
    // Wait for the answer and overwrite our buffer
    recv(sock, (char*)buf, MAX_PACKET_LENGTH, 0);

    // Read what we got
    dns.ReadAnswer(buf);

    // Hooray, now let's print out our answers
    const auto& answers = dns.GetAnswers();
    for (uint32_t i = 0; i < answers.size(); i++) {
        printf("%u - %s: %hhu.%hhu.%hhu.%hhu, TTL: %us\n", i + 1, answers[i]->host,
               answers[i]->addr[0], answers[i]->addr[1], answers[i]->addr[2], answers[i]->addr[3],
               answers[i]->ttl);
    }
    if (!answers.size())
        printf("Could not resolve host!\n");

    // Close our socket
    close(sock);

    // Stop stopwatch
    uint32_t ms = static_cast<uint32_t>((clock() - time_start) * 1000.f / CLOCKS_PER_SEC);
    printf("Time: %ums\n", ms);

#ifdef BUILD_WIN32
    // Even moar haxxes
    WSACleanup();
#endif

    return 0;
}
