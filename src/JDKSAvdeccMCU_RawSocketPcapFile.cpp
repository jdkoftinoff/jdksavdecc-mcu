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

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_RawSocketPcapFile.hpp"

#if JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE && JDKSAVDECCMCU_ENABLE_PCAPFILE

namespace JDKSAvdeccMCU
{

RawSocketPcapFile::RawSocketPcapFile(
    uint16_t ethertype,
    jdksavdecc_eui48 my_mac,
    jdksavdecc_eui48 default_dest_mac,
    jdksavdecc_eui48 *join_multicast,
    const std::string &input_file,
    const std::string &output_file,
    jdksavdecc_timestamp_in_milliseconds time_granularity_in_ms )
    : m_ethertype( ethertype )
    , m_my_mac( my_mac )
    , m_default_dest_mac( default_dest_mac )
    , m_pcap_file_reader( input_file )
    , m_pcap_file_writer( output_file )
    , m_current_time( 0 )
    , m_time_granularity_in_ms( time_granularity_in_ms )
{
    if ( join_multicast )
    {
        m_join_multicast = *join_multicast;
    }
    else
    {
        jdksavdecc_eui48_init( &m_join_multicast );
    }
    if ( readNextIncomingFrame() )
    {
        m_current_time = m_next_incoming_frame.getTimeInMilliseconds();
    }
}

RawSocketPcapFile::~RawSocketPcapFile() {}

jdksavdecc_timestamp_in_milliseconds RawSocketPcapFile::getTimeInMilliseconds()
{
    jdksavdecc_timestamp_in_milliseconds t = m_current_time;
    m_current_time += m_time_granularity_in_ms;
    if ( m_next_incoming_frame.getLength() > 0 )
    {
        if ( m_current_time >= m_next_incoming_frame.getTimeInMilliseconds() )
        {
            m_current_time = m_next_incoming_frame.getTimeInMilliseconds();
        }
    }
    return t;
}

bool RawSocketPcapFile::recvFrame( Frame *frame )
{
    bool r = false;
    if ( m_next_incoming_frame.getLength() > 0 )
    {
        if ( m_current_time >= m_next_incoming_frame.getTimeInMilliseconds() )
        {
            *frame = m_next_incoming_frame;
            m_next_incoming_frame.setLength( 0 );
            readNextIncomingFrame();
            r = true;
        }
    }
    return r;
}

bool RawSocketPcapFile::sendFrame( const Frame &frame,
                                   const uint8_t *data1,
                                   uint16_t len1,
                                   const uint8_t *data2,
                                   uint16_t len2 )
{
    jdksavdecc_eui48 da = frame.getDA();
    jdksavdecc_eui48 sa = m_my_mac;
    if ( jdksavdecc_eui48_is_unset( da ) )
    {
        da = m_default_dest_mac;
    }
    PcapFilePacket packet;
    packet.reserve( frame.getLength() + len1 + len2
                    - JDKSAVDECC_FRAME_HEADER_LEN );
    for ( size_t i = JDKSAVDECC_FRAME_HEADER_LEN; i < frame.getLength(); ++i )
    {
        packet.push_back( frame.getOctet( i ) );
    }
    if ( data1 )
    {
        for ( size_t i = 0; i < len1; ++i )
        {
            packet.push_back( data1[i] );
        }
    }
    if ( data2 )
    {
        for ( size_t i = 0; i < len1; ++i )
        {
            packet.push_back( data2[i] );
        }
    }

    m_pcap_file_writer.WritePacket(
        m_current_time * 1000, da.value, sa.value, m_ethertype, packet );
    return true;
}

bool RawSocketPcapFile::sendReplyFrame( Frame &frame,
                                        const uint8_t *data1,
                                        uint16_t len1,
                                        const uint8_t *data2,
                                        uint16_t len2 )
{
    jdksavdecc_eui48 da = frame.getSA();
    jdksavdecc_eui48 sa = m_my_mac;
    if ( da.value[0] & 0x1 )
    {
        // squash multicast
        da.value[0] &= 0xfe;
    }
    PcapFilePacket packet;
    packet.resize( frame.getLength() + len1 + len2
                   - JDKSAVDECC_FRAME_HEADER_LEN );
    for ( size_t i = JDKSAVDECC_FRAME_HEADER_LEN; i < frame.getLength(); ++i )
    {
        packet.push_back( frame.getOctet( i ) );
    }
    if ( data1 )
    {
        for ( size_t i = 0; i < len1; ++i )
        {
            packet.push_back( data1[i] );
        }
    }
    if ( data2 )
    {
        for ( size_t i = 0; i < len1; ++i )
        {
            packet.push_back( data2[i] );
        }
    }

    m_pcap_file_writer.WritePacket(
        m_current_time * 1000, da.value, sa.value, m_ethertype, packet );
    return true;
}

bool RawSocketPcapFile::joinMulticast( const jdksavdecc_eui48 &multicast_mac )
{
    m_join_multicast = multicast_mac;

    return true;
}

void RawSocketPcapFile::setNonblocking() {}

RawSocket::filedescriptor_type RawSocketPcapFile::getFd() const { return 0; }

const jdksavdecc_eui48 &RawSocketPcapFile::getMACAddress() const
{
    return m_my_mac;
}

bool RawSocketPcapFile::readNextIncomingFrame()
{
    bool r = false;
    uint64_t timestamp_in_microseconds = 0;
    PcapFilePacket frame_data;
    // try read the pcap file
    if ( m_pcap_file_reader.ReadPacket( &timestamp_in_microseconds,
                                        frame_data ) )
    {
        // we got a frame fromt he file, does it fit?
        if ( frame_data.size() <= m_next_incoming_frame.getMaxLength() )
        {
            // yes, so copy the frame into the next incoming frame buffer and
            // track the time
            m_next_incoming_frame.setTimeInMilliseconds(
                timestamp_in_microseconds / 1000 );
            memcpy( m_next_incoming_frame.getBuf(),
                    &frame_data[0],
                    m_next_incoming_frame.getMaxLength() );

            // make sure it is an ethertype that we care about
            if ( m_next_incoming_frame.getEtherType() == m_ethertype )
            {
                m_next_incoming_frame.setLength( frame_data.size() );

                r = true;
            }
            else
            {
                // it isn't a packet we care about, so drop it
                m_next_incoming_frame.setLength( 0 );
            }
        }
    }
    return r;
}
}

#else
const char *jdksavdeccmcu_rawsocketpcapfile_file = __FILE__;
#endif
