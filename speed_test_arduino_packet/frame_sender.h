#ifndef __FRAMED_SENDER_H_
#define __FRAMED_SENDER_H_

#include "stdint.h"
#include "stddef.h"

template<typename T>
struct FramedPacketData
{
    uint16_t type;
    uint16_t size;
    T data;
    uint32_t crc32;
};

template <uint16_t packet_type, typename T>
class FramedPacket
{
public:
    FramedPacket()
    {
        m_data.type = packet_type;
        m_data.size = sizeof(T);
        m_data.crc32 = 0xFFFFFFFF;
    }

    T* getData() const
    {
        return m_data.data;
    }

    virtual void updateCRC()
    {
        // TODO(jbott): Update CRC32 here
        m_data.crc32 = 0x00FF00FF;
    }

    const uint8_t* updateCRCAndGetBuffer()
    {
        this->updateCRC();
        return reinterpret_cast<unsigned char*>(&m_data);
    }

    virtual const size_t getSize() const
    {
        return sizeof(FramedPacketData<T>);
    }

    FramedPacketData<T> m_data;
};

template <uint16_t packet_type, size_t buffer_size = 256>
class LogPacket : public FramedPacket<packet_type, unsigned char[buffer_size]>
{
public:
    LogPacket() : FramedPacket<packet_type, unsigned char[buffer_size]>() {}

    void printf(const char* format, ...)
    {
      va_list args;
      va_start (args, format);
      vsnprintf (this->m_data.data, buffer_size, format, args);
      va_end(args);
    }

    virtual const size_t getSize() const
    {
        return FramedPacket<packet_type, unsigned char[buffer_size]>::getSize() - buffer_size + strlen(this->m_data.data);
    }

    virtual void updateCRC()
    {
        this->m_data.size = strlen(this->m_data.data);
        m_crc32_ptr = reinterpret_cast<uint8_t*>(&(this->m_data)) + getSize() - sizeof(uint32_t);
        m_crc32_ptr[0] = 0x00;
        m_crc32_ptr[1] = 0xFF;
        m_crc32_ptr[2] = 0x00;
        m_crc32_ptr[3] = 0xFF;
    }

private:
    uint8_t *m_crc32_ptr = nullptr;
};

#endif // __FRAMED_SENDER_H_
