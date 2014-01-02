/*
 Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "JDKSAvdeccWorld.hpp"

namespace JDKSAvdecc {


class FrameBase {
protected:
    jdksavdecc_timestamp_in_milliseconds m_time_in_ms;
    uint8_t *m_buf;
    uint16_t m_pos;
    uint16_t m_len;

public:
    FrameBase(
          jdksavdecc_timestamp_in_milliseconds time_in_ms,
          uint8_t *buf,
          uint16_t len,
          jdksavdecc_eui48 const &dest_mac,
          jdksavdecc_eui48 const &src_mac,
          uint16_t ethertype )
        : m_time_in_ms(time_in_ms)
        , m_buf(buf)
        , m_pos(0)
        , m_len(len) {
        PutEUI48( dest_mac );
        PutEUI48( src_mac );
        PutDoublet( ethertype );
    }

    FrameBase(
          jdksavdecc_timestamp_in_milliseconds time_in_ms,
          uint8_t *buf,
          uint16_t len)
        : m_time_in_ms(time_in_ms)
        , m_buf(buf)
        , m_pos(JDKSAVDECC_FRAME_HEADER_LEN)
        , m_len(len) {}

    uint16_t GetLen() const { return m_len; }

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
    uint8_t *GetBuf() { return m_buf; }
    uint16_t GetPos() const { return m_pos; }
    void SetPos( uint16_t n = JDKSAVDECC_FRAME_HEADER_LEN ) {
        m_pos = n;
    }

    jdksavdecc_eui48 GetDA() const {
        return jdksavdecc_eui48_get(m_buf, JDKSAVDECC_FRAME_HEADER_DA_OFFSET );
    }

    jdksavdecc_eui48 GetSA() const {
        return jdksavdecc_eui48_get(m_buf, JDKSAVDECC_FRAME_HEADER_SA_OFFSET );
    }

    uint16_t GetEtherType() const {
        return jdksavdecc_uint16_get(m_buf, JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET );
    }

    uint8_t *GetPayload() {
        return &m_buf[JDKSAVDECC_FRAME_HEADER_LEN];
    }

    uint8_t const *GetPayload() const {
        return &m_buf[JDKSAVDECC_FRAME_HEADER_LEN];
    }

    jdksavdecc_timestamp_in_milliseconds GetTimeInMs() const { return m_time_in_ms; }
};

template <size_t MaxSize>
class Frame : public FrameBase {
protected:
    uint8_t m_buf_storage[MaxSize];
public:
    Frame(
            jdksavdecc_timestamp_in_milliseconds time_in_ms,
            jdksavdecc_eui48 const &dest_mac,
            jdksavdecc_eui48 const &src_mac,
            uint16_t ethertype )
        : FrameBase(time_in_ms,m_buf_storage,MaxSize,dest_mac,src_mac,ethertype) {}

};

}
