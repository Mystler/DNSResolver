#include "DNS.h"
#include <cstring>
#include <cstdlib>

#ifdef BUILD_WIN32
#   include <winsock2.h>
#else
#   include <arpa/inet.h>
#endif

void ConvertHostnameToDNS(const char* host, char* dns)
{
    size_t processed = 0;
    for (size_t i = 0; i < strlen(host) + 1; i++) {
        if (host[i] == '.' || host[i] == 0) {
            *dns++ = i - processed; // Set char count
            for (; processed < i; processed++)
                *dns++ = host[processed]; // Copy characters
            processed++; // Skip the dot
        }
    }
    *dns = 0; // Add Null-Termination
}

DNSPacket::DNSPacket(const char* hostname)
{
    // Create the header
    fHeader = new DNSHeader();
    fHeader->id = rand(); // We don't care about real ID management for now
    fHeader->flags = kFlagRD; // Let the NS do the magic
    fHeader->qdcount = 1;
    fHeader->ancount = 0;
    fHeader->nscount = 0;
    fHeader->arcount = 0;

    // Convert the hostname
    ConvertHostnameToDNS(hostname, fQName);

    // Set the question info
    fQuestion = new DNSQuestionInfo();
    fQuestion->qtype = TYPE_A;
    fQuestion->qclass = CLASS_INET;
}

DNSPacket::~DNSPacket()
{
    delete fHeader;
    delete fQuestion;
}

void DNSPacket::WriteQuestion(char* buf, size_t &len)
{
    len = 0;
    DNSHeader* bufHeader = (DNSHeader*)buf;
    bufHeader->id = htons(fHeader->id);
    bufHeader->flags = htons(fHeader->flags);
    bufHeader->qdcount = htons(fHeader->qdcount);
    bufHeader->ancount = htons(fHeader->ancount);
    bufHeader->nscount = htons(fHeader->nscount);
    bufHeader->arcount = htons(fHeader->arcount);
    len += sizeof(DNSHeader);

    strcpy(&buf[len], fQName);
    len += strlen(fQName) + 1;

    DNSQuestionInfo* bufQuestion = (DNSQuestionInfo*)&buf[len];
    bufQuestion->qtype = htons(fQuestion->qtype);
    bufQuestion->qclass = htons(fQuestion->qclass);
    len += sizeof(DNSQuestionInfo);
}

void DNSPacket::ReadAnswer(char *buf)
{
    //TODO
}
