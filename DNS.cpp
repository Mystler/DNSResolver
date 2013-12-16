#include "DNS.h"
#include "Stream.h"
#include <cstring>

void DNSHeader::Read(Stream* S)
{
    id = S->ReadU16();
    flags = S->ReadU16();
    qdcount = S->ReadU16();
    ancount = S->ReadU16();
    nscount = S->ReadU16();
    arcount = S->ReadU16();
}

void DNSHeader::Write(Stream* S) const
{
    S->WriteU16(id);
    S->WriteU16(flags);
    S->WriteU16(qdcount);
    S->WriteU16(ancount);
    S->WriteU16(nscount);
    S->WriteU16(arcount);
}

void DNSQuestion::Read(Stream* S)
{
    S->ReadDNSHostname(host);
    qtype = S->ReadU16();
    qclass = S->ReadU16();
}

void DNSQuestion::Write(Stream* S) const
{
    S->WriteDNSHostname(host);
    S->WriteU16(qtype);
    S->WriteU16(qclass);
}

void DNSAnswer::Read(Stream* S)
{
    S->ReadDNSHostname(host);
    atype = S->ReadU16();
    aclass = S->ReadU16();
    ttl = S->ReadU32();
    datalen = S->ReadU16(); // should be 4
    for (size_t i = 0; i < datalen; i++)
        addr[i] = S->ReadByte();
}

void DNSAnswer::Write(Stream* S) const
{
    // We don't care about shortening stuff
    S->WriteDNSHostname(host);
    S->WriteU16(atype);
    S->WriteU16(aclass);
    S->WriteU32(ttl);
    S->WriteU16(datalen);
    for (size_t i = 0; i < datalen; i++)
        S->WriteByte(addr[i]);
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

    // Set the question info
    std::unique_ptr<DNSQuestion> question(new DNSQuestion());
    strcpy(question->host, hostname);
    question->qtype = TYPE_A;
    question->qclass = CLASS_INET;
    fQuestions.push_back(std::move(question));
}

DNSPacket::~DNSPacket()
{
    delete fHeader;
    fQuestions.clear();
    fAnswers.clear();
}

void DNSPacket::WriteQuestion(uint8_t* buf, size_t &len) const
{
    Stream S(buf);
    fHeader->Write(&S);
    for (size_t i = 0; i < fQuestions.size(); i++)
        fQuestions[i]->Write(&S);
    len = S.GetPos();
}

void DNSPacket::ReadAnswer(uint8_t* buf)
{
    Stream S(buf);

    // Read the DNS header, whew that was easy
    fHeader->Read(&S);

    // Read our questions
    fQuestions.clear();
    for (size_t i = 0; i < fHeader->qdcount; i++) {
        std::unique_ptr<DNSQuestion> question(new DNSQuestion());
        question->Read(&S);
        fQuestions.push_back(std::move(question));
    }

    // Read the answers
    fAnswers.clear();
    for (size_t i = 0; i < fHeader->ancount; i++) {
        std::unique_ptr<DNSAnswer> answer(new DNSAnswer());
        answer->Read(&S);
        fAnswers.push_back(std::move(answer));
    }
}
