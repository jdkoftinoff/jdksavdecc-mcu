#pragma once

#include "JDKSAvdeccWorld.h"

namespace JDKSAvdecc {


class FrameBase {
protected:
    uint8_t *m_buf;
    uint16_t m_pos;
public:
    FrameBase(
          uint8_t *buf,
          jdksavdecc_eui48 const &dest_mac,
          jdksavdecc_eui48 const &src_mac,
          uint16_t ethertype )
        : m_buf(buf)
        , m_pos(0) {
        PutEUI48( dest_mac );
        PutEUI48( src_mac );
        PutDoublet( ethertype );
    }

    void PutEUI48( jdksavdecc_eui48 const &val ) {
        for( uint16_t i=0; i<6; ++i ) {
            m_buf[ m_pos++ ] = val.value[i];
        }
    }

    void PutEUI64( jdksavdecc_eui64 const &val ) {
        for( uint16_t i=0; i<8; ++i ) {
            m_buf[ m_pos++ ] = val.value[i];
        }
    }

    void PutOctet( uint8_t val ) {
        m_buf[ m_pos++ ] = val;
    }

    void PutDoublet( uint16_t val ) {
        m_buf[ m_pos++ ] = (val>>8)&0xff;
        m_buf[ m_pos++ ] = (val>>0)&0xff;
    }

    void PutQuadlet( uint32_t val ) {
        m_buf[ m_pos++ ] = (val>>24)&0xff;
        m_buf[ m_pos++ ] = (val>>16)&0xff;
        m_buf[ m_pos++ ] = (val>>8)&0xff;
        m_buf[ m_pos++ ] = (val>>0)&0xff;
    }

    void PutOctlet( uint64_t val ) {
        m_buf[ m_pos++ ] = (val>>56)&0xff;
        m_buf[ m_pos++ ] = (val>>48)&0xff;
        m_buf[ m_pos++ ] = (val>>40)&0xff;
        m_buf[ m_pos++ ] = (val>>32)&0xff;
        m_buf[ m_pos++ ] = (val>>24)&0xff;
        m_buf[ m_pos++ ] = (val>>16)&0xff;
        m_buf[ m_pos++ ] = (val>>8)&0xff;
        m_buf[ m_pos++ ] = (val>>0)&0xff;
    }

    void PutZeros( uint16_t count ) {
        for( uint16_t i=0; i<count; ++i ) {
            m_buf[ m_pos++ ] = 0;
        }
    }

    void PutBuf( uint8_t *buf, uint16_t len ) {
        for( uint16_t i=0; i<len; ++i ) {
            m_buf[ m_pos++ ] = buf[i];
        }
    }

    uint8_t const *GetBuf() const { return m_buf; }
    uint16_t GetLength() const { return m_pos; }
};

template <size_t MaxSize>
class Frame : public FrameBase {
protected:
    uint8_t m_buf_storage[MaxSize];
public:
    Frame( jdksavdecc_eui48 const &dest_mac,
           jdksavdecc_eui48 const &src_mac,
           uint16_t ethertype )
        : FrameBase(m_buf_storage,dest_mac,src_mac,ethertype) {}

};

}
