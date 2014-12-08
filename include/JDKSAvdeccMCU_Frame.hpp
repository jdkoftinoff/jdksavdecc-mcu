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

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_FixedBuffer.hpp"
#include "JDKSAvdeccMCU_Frame.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief The Ethernet Frame class
///
/// A type of FixedBuffer that adds methods to manage
/// Ethernet Frame headers and payloads and a timestamp
///
class Frame : public FixedBuffer
{
  protected:
    ///
    /// \brief m_time_in_ms The timestamp of the frame in milliseconds
    ///
    jdksavdecc_timestamp_in_milliseconds m_time_in_ms;

  public:
    ///
    /// \brief Frame Constructor of a Frame
    /// \param time_in_ms The time in milliseconds
    /// \param buf The buffer storage area to use
    /// \param len The buffer storage area length
    /// \param dest_mac The destination MAC address
    /// \param src_mac The source MAC Address
    /// \param ethertype The Ethertype
    ///
    Frame( jdksavdecc_timestamp_in_milliseconds time_in_ms,
           uint8_t *buf,
           uint16_t len,
           Eui48 const &dest_mac,
           Eui48 const &src_mac,
           uint16_t ethertype )
        : FixedBuffer( buf, len ), m_time_in_ms( time_in_ms )
    {
        putEUI48( dest_mac );
        putEUI48( src_mac );
        putDoublet( ethertype );
    }

    ///
    /// \brief Frame Constructor of a Frame
    /// \param time_in_ms The time in milliseconds
    /// \param buf The buffer storage area to use
    /// \param len The buffer storage area length
    ///
    Frame( jdksavdecc_timestamp_in_milliseconds time_in_ms,
           uint8_t *buf,
           uint16_t len )
        : FixedBuffer( buf, len ), m_time_in_ms( time_in_ms )
    {
    }

    ///
    /// \brief getDA Get the Frame's Destination Address
    /// \return The Eui48
    ///
    Eui48 getDA() const
    {
        return getEUI48( JDKSAVDECC_FRAME_HEADER_DA_OFFSET );
    }

    ///
    /// \brief setDA Set the Frame's Destination Address
    /// \param da The Eui48
    ///
    void setDA( Eui48 const &da )
    {
        setEUI48( da, JDKSAVDECC_FRAME_HEADER_DA_OFFSET );
    }

    ///
    /// \brief getDA Get the Frame's Source Address
    /// \return The Eui48
    ///
    Eui48 getSA() const
    {
        return getEUI48( JDKSAVDECC_FRAME_HEADER_SA_OFFSET );
    }

    ///
    /// \brief setDA Set the Frame's Source Address
    /// \param sa The Eui48
    ///
    void setSA( Eui48 const &sa )
    {
        setEUI48( sa, JDKSAVDECC_FRAME_HEADER_SA_OFFSET );
    }

    ///
    /// \brief getEtherType Get the Frame's EtherType
    /// Note: does not manage tagged frames
    /// \return The EtherType value
    ///
    uint16_t getEtherType() const
    {
        return getDoublet( JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET );
    }

    ///
    /// \brief setEtherType Set the Frame's EtherType
    /// Note: does not manage tagged frames
    /// \param et The EtherType value
    ///
    void setEtherType( uint16_t et )
    {
        setDoublet( et, JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET );
    }

    ///
    /// \brief getPayload Get the Frame's Payload
    /// Note: does not manage tagged frames
    /// \return The raw uint8_t pointer to the payload
    ///
    uint8_t *getPayload() { return &m_buf[JDKSAVDECC_FRAME_HEADER_LEN]; }

    ///
    /// \brief getPayload Get The Frame's const Payload
    /// \return The raw uint8_t const pointer to the payload
    ///
    uint8_t const *getPayload() const
    {
        return &m_buf[JDKSAVDECC_FRAME_HEADER_LEN];
    }

    ///
    /// \brief getTimeInMilliseconds Get the timestamp in milliseconds
    /// \return The timestamp
    ///
    jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds() const
    {
        return m_time_in_ms;
    }

    ///
    /// \brief setTimeInMilliseconds Set the timestamp in milliseconds
    /// \param v The timestamp
    ///
    void setTimeInMilliseconds( jdksavdecc_timestamp_in_milliseconds v )
    {
        m_time_in_ms = v;
    }
};

///
/// \brief The Ethernet FrameWithSize class
///
/// A subclass of Frame which is templatized on a MaxSize
/// argument and allocates a static buffer of that size
///
template <size_t MaxSize=(1500+JDKSAVDECC_FRAME_HEADER_LEN)>
class FrameWithSize : public Frame
{
  protected:
    ///
    /// \brief m_buf_storage The storage for the frame
    ///
    uint8_t m_buf_storage[MaxSize];

  public:
    ///
    /// \brief FrameWithSize Constructor of an empty allocated frame
    ///
    FrameWithSize() : Frame( 0, m_buf_storage, MaxSize ) {}

    ///
    /// \brief FrameWithSize Constructor of an empty frame with timestamp
    /// \param time_in_ms The timestamp in milliseconds
    ///
    FrameWithSize( jdksavdecc_timestamp_in_milliseconds time_in_ms )
        : Frame( time_in_ms, m_buf_storage, MaxSize )
    {
    }

    ///
    /// \brief FrameWithSize Constructor of a frame with header and timestamp
    /// \param time_in_ms The timestamp in milliseconds
    /// \param dest_mac The destination MAC adddress
    /// \param src_mac The source MAC address
    /// \param ethertype The EtherType
    ///
    FrameWithSize( jdksavdecc_timestamp_in_milliseconds time_in_ms,
                   Eui48 const &dest_mac,
                   Eui48 const &src_mac,
                   uint16_t ethertype )
        : Frame(
              time_in_ms, m_buf_storage, MaxSize, dest_mac, src_mac, ethertype )
    {
    }
};
}
