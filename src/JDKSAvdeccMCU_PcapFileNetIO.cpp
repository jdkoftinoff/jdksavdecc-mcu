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
#include "JDKSAvdeccMCU_RawNetIO.hpp"

#ifdef JDKSAVDECCMCU_ENABLE_PCAPFILE

#include "JDKSAvdeccMCU_PcapFileNetIO.hpp"

namespace JDKSAvdeccMCU
{

PcapFileNetIO::PcapFileNetIO( jdksavdecc_eui48 mac_address, const char *incoming_file_name, const char *outgoing_file_name )
    : m_mac_address( mac_address ), m_incoming_file( incoming_file_name ), m_outgoing_file( outgoing_file_name )
{
}

PcapFileNetIO::~PcapFileNetIO() {}

void PcapFileNetIO::initialize() {}

uint16_t PcapFileNetIO::receiveRawNet( uint8_t *data, uint16_t max_len )
{
    uint16_t len = 0;
    uint64_t timestamp_micros;
    uint8_t da[6];
    uint8_t sa[6];
    uint16_t ethertype;
    PcapFilePacket packet_payload;
    packet_payload.reserve( 1500 );
    if ( m_incoming_file.ReadPacket( &timestamp_micros, da, sa, &ethertype, &packet_payload ) )
    {
        if ( packet_payload.size() + 14 <= max_len )
        {
            memcpy( &data[0], da, 6 );
            memcpy( &data[6], sa, 6 );
            data[12] = uint8_t( ( ethertype >> 8 ) & 0xff );
            data[13] = uint8_t( ( ethertype >> 0 ) & 0xff );
            memcpy( &data[14], packet_payload.data(), packet_payload.size() );
            len = packet_payload.size() + 14;
        }
    }
    return len;
}

bool PcapFileNetIO::sendRawNet(
    const uint8_t *data, uint16_t len, const uint8_t *data1, uint16_t len1, const uint8_t *data2, uint16_t len2 )
{
    PcapFilePacket packet;
    packet.reserve( 1500 );

    if ( data && len > 0 )
    {
        for ( uint16_t i = 0; i < len; ++i )
        {
            packet.push_back( data[i] );
        }
    }
    if ( data1 && len1 > 0 )
    {
        for ( uint16_t i = 0; i < len1; ++i )
        {
            packet.push_back( data1[i] );
        }
    }
    if ( data2 && len2 > 0 )
    {
        for ( uint16_t i = 0; i < len2; ++i )
        {
            packet.push_back( data2[i] );
        }
    }

    m_outgoing_file.WritePacket( packet );
    return true;
}

bool PcapFileNetIO::sendReplyRawNet(
    const uint8_t *data, uint16_t len, const uint8_t *data1, uint16_t len1, const uint8_t *data2, uint16_t len2 )
{
    PcapFilePacket packet;
    packet.reserve( 1500 );

    if ( data && len > 0 )
    {
        for ( uint16_t i = 0; i < len; ++i )
        {
            packet.push_back( data[i] );
        }
    }
    if ( data1 && len1 > 0 )
    {
        for ( uint16_t i = 0; i < len1; ++i )
        {
            packet.push_back( data1[i] );
        }
    }
    if ( data2 && len2 > 0 )
    {
        for ( uint16_t i = 0; i < len2; ++i )
        {
            packet.push_back( data2[i] );
        }
    }

    // reverse DA and SA
    for ( uint16_t i = 0; i < 6; ++i )
    {
        std::swap( packet[i], packet[i + 6] );
    }

    m_outgoing_file.WritePacket( packet );
    return true;
}
}

#else

const char *jdksavdeccmcu_pcapfilenetio_file = __FILE__;

#endif
