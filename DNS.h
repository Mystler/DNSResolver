#ifndef _DNS_h_
#define _DNS_h_

#include <cstdlib>
#include <cstdint>

#define TYPE_A 0x0001
#define CLASS_INET 0x0001

enum Flags
{
    kFlagErr = 0x000F, // ErrorCode
    kFlagRA = 0x0080, // Recursion Available
    kFlagRD = 0x0100, // Recursion Desired
    kFlagTC = 0x0200, // Truncated
    kFlagAA = 0x0400, // Authoritative Answer
    kFlagOp = 0x7800, // Opcode
    kFlagQR = 0x8000 // Query 0 or Response 1
};

struct DNSHeader
{
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount; // Number of Questions
    uint16_t ancount; // Number of Answers
    uint16_t nscount; // Number of Authority Records
    uint16_t arcount; // Number of Additional Records
};

struct DNSQuestionInfo
{
    uint16_t qtype;
    uint16_t qclass;
};

class DNSPacket
{
public:
    /** Construct a simple recursive DNS query */
    DNSPacket(const char* hostname);
    ~DNSPacket();

    /** Write the packet to a network buffer.
     *  This will handle BE conversion.
     */
    void WriteQuestion(char* buf, size_t &len);

    /** Read a DNS (Response) packet from a raw network buffer.
     *  This will handle BE conversion.
     */
    void ReadAnswer(char* buf);

private:
    DNSHeader* fHeader;
    char fQName[256];
    DNSQuestionInfo* fQuestion;
};

#endif
