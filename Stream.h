#ifndef _Stream_h_
#define _Stream_h_

#include <cstdlib>
#include <cstdint>

class Stream
{
public:
    /** Construct a simple network buffer stream */
    Stream(uint8_t* buf);

    /** Read an unsigned 8 bit integer */
    uint8_t ReadByte();

    /** Read an unsigned 16 bit integer */
    uint16_t ReadU16();

    /** Read an unsigned 32 bit integer */
    uint32_t ReadU32();

    /** Read the Hostname from DNS representation */
    void ReadDNSHostname(char* host);

    /** Write an unsigned 8 bit integer */
    void WriteByte(const uint8_t& value);

    /** Write an unsigned 16 bit integer */
    void WriteU16(const uint16_t& value);

    /** Write an unsigned 32 bit integer */
    void WriteU32(const uint32_t& value);

    /** Write the Hostname in DNS representation */
    void WriteDNSHostname(const char* host);

    /** Returns the current position in the buffer */
    size_t GetPos() const { return fPos; }

private:
    uint8_t* fBuffer;
    size_t fPos;
};

#endif
