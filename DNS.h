#ifndef _DNS_h_
#define _DNS_h_

#include <cstdlib>
#include <cstdint>
#include <vector>
#include <memory>

#define TYPE_A 0x0001
#define CLASS_INET 0x0001

class Stream;

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

    void Read(Stream* S);
    void Write(Stream* S) const;
};

struct DNSQuestion
{
    char host[256];
    uint16_t qtype;
    uint16_t qclass;

    void Read(Stream* S);
    void Write(Stream* S) const;
};

struct DNSAnswer
{
    char host[256];
    uint16_t atype;
    uint16_t aclass;
    uint32_t ttl;
    uint16_t datalen;
    uint8_t addr[4];

    void Read(Stream* S);
    void Write(Stream* S) const;
};

class DNSPacket
{
public:
    /** Construct a simple recursive DNS query */
    DNSPacket(const char* hostname);

    /** Destroy it all!!! */
    ~DNSPacket();

    /** Write the packet to a network buffer. */
    void WriteQuestion(uint8_t* buf, size_t &len) const;

    /** Read a DNS (Response) packet from a raw network buffer. */
    void ReadAnswer(uint8_t* buf);

    /** Returns the vector reference of our DNS Answer Records */
    std::vector<std::unique_ptr<DNSAnswer>>& GetAnswers() { return fAnswers; }

private:
    DNSHeader* fHeader;
    std::vector<std::unique_ptr<DNSQuestion>> fQuestions;
    std::vector<std::unique_ptr<DNSAnswer>> fAnswers;
};

#endif
