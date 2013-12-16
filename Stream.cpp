#include "Stream.h"
#include <cstring>

#ifdef BUILD_WIN32
#   include <winsock2.h>
#else
#   include <arpa/inet.h>
#endif

Stream::Stream(uint8_t* buf) : fBuffer(buf), fPos(0)
{
}

uint8_t Stream::ReadByte()
{
    uint8_t* ptr = (uint8_t*)&fBuffer[fPos++];
    return *ptr;
}

uint16_t Stream::ReadU16()
{
    uint16_t* ptr = (uint16_t*)&fBuffer[fPos];
    fPos += sizeof(uint16_t);
    return ntohs(*ptr);
}

uint32_t Stream::ReadU32()
{
    uint32_t* ptr = (uint32_t*)&fBuffer[fPos];
    fPos += sizeof(uint32_t);
    return ntohl(*ptr);
}

void Stream::WriteDNSHostname(const char* host)
{
    size_t processed = 0;
    for (size_t i = 0; i < strlen(host) + 1; i++) {
        if (host[i] == '.' || host[i] == 0) {
            fBuffer[fPos++] = i - processed; // Set char count
            for (; processed < i; processed++)
                fBuffer[fPos++] = host[processed]; // Copy characters
            processed++; // Skip the dot
        }
    }
    fBuffer[fPos++] = 0; // Add Null-Termination
}

void Stream::WriteByte(const uint8_t& value)
{
    uint8_t* ptr = (uint8_t*)&fBuffer[fPos++];
    *ptr = value;
}

void Stream::WriteU16(const uint16_t& value)
{
    uint16_t* ptr = (uint16_t*)&fBuffer[fPos];
    fPos += sizeof(uint16_t);
    *ptr = htons(value);
}

void Stream::WriteU32(const uint32_t& value)
{
    uint32_t* ptr = (uint32_t*)&fBuffer[fPos];
    fPos += sizeof(uint32_t);
    *ptr = htonl(value);
}

void Stream::ReadDNSHostname(char* host)
{
    const uint8_t* dns = &fBuffer[fPos];
    uint16_t* ptr = (uint16_t*)&fBuffer[fPos];
    uint16_t offsetshort = ntohs(*ptr);
    bool offset = (offsetshort & 0xC000) == 0xC000;
    if (offset) {
        dns = &fBuffer[offsetshort & ~0xC000];
        fPos += sizeof(uint16_t);
    }

    size_t i = 0;
    while (dns[i] != 0) {
        size_t read = dns[i] + i; // How many characters in part
        for (i += 1; i <= read; i++)
            *host++ = dns[i];
        *host++ = '.'; // Add delimiter
    }
    *(host - 1) = 0; // Null-Termination replaces last dot

    if (!offset)
        fPos += i + 1;
}
