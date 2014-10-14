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

#if JDKSAVDECCMCU_ENABLE_PCAPFILE==1

#include "JDKSAvdeccMCU_PcapFileReader.hpp"

namespace JDKSAvdeccMCU
{

PcapFileReader::PcapFileReader( std::string const &filename )
    : m_file( filename, "rb" )
    , m_filename( filename )
    , m_swap( false )
    , m_seen_first_timestamp( false )
    , m_first_timestamp_in_microseconds( 0 )
{
    if ( !m_file.get() )
    {
        throw std::runtime_error( std::string( "Error opening pcap file: " )
                                  + filename );
    }

    pcap_hdr_t header;
    if ( std::fread( &header, sizeof( header ), 1, m_file.get() ) != 1 )
    {
        throw std::runtime_error(
            std::string( "Error reading pcap file header: " ) + filename );
    }

    if ( header.magic_number == 0xa1b2c3d4 )
    {
        m_swap = false;
    }
    else if ( header.magic_number == 0xd4c3b2a1 )
    {
        m_swap = true;
    }
    else
    {
        throw std::runtime_error(
            std::string( "Error pcap file header is incompatible: " )
            + filename );
    }
}

PcapFileReader::~PcapFileReader() {}

bool PcapFileReader::ReadPacket( uint64_t *timestamp_in_microseconds,
                                 PcapFilePacket &results )
{
    pcaprec_hdr_t packet_header;
    if ( std::feof( m_file.get() ) )
    {
        return false;
    }
    if ( std::fread( &packet_header, sizeof( packet_header ), 1, m_file.get() )
         != 1 )
    {
        if ( std::feof( m_file.get() ) )
        {
            return false;
        }
        else
        {
            throw std::runtime_error(
                std::string( "Error reading pcap file packet from: " )
                + m_filename );
        }
    }

    /* swap bytes if necessary */
    if ( m_swap )
    {
        packet_header.incl_len = PcapFileSwap( packet_header.incl_len );
        packet_header.orig_len = PcapFileSwap( packet_header.orig_len );
        packet_header.ts_sec = PcapFileSwap( packet_header.ts_sec );
        packet_header.ts_usec = PcapFileSwap( packet_header.ts_usec );
    }

    /* read the payload */
    if ( packet_header.incl_len > 32768 )
    {
        throw std::runtime_error( std::string( "Error reading packet from: " )
                                  + m_filename );
    }

    *timestamp_in_microseconds = ( packet_header.ts_sec * 1000000 )
                                 + ( packet_header.ts_usec );

    results.resize( (size_t)packet_header.incl_len );
    if ( std::fread( &results[0], results.size(), 1, m_file.get() ) != 1 )
    {
        throw std::runtime_error(
            std::string( "Error reading pcap file packet data from: " )
            + m_filename );
    }
    if ( !m_seen_first_timestamp )
    {
        m_seen_first_timestamp = true;
        m_first_timestamp_in_microseconds = *timestamp_in_microseconds;
    }
    *timestamp_in_microseconds = *timestamp_in_microseconds
                                 - m_first_timestamp_in_microseconds;
    return true;
}

bool PcapFileReader::ReadPacket( uint64_t *timestamp_in_microseconds,
                                 uint8_t da[6],
                                 uint8_t sa[6],
                                 uint16_t *ethertype,
                                 PcapFilePacket *packet_payload )
{
    bool r;
    std::vector<uint8_t> tmp;
    r = ReadPacket( timestamp_in_microseconds, tmp );
    if ( r )
    {
        if ( tmp.size() >= 14 )
        {
            memcpy( da, &tmp[0], 6 );
            memcpy( sa, &tmp[6], 6 );
            *ethertype = ( ( (uint16_t)tmp[12] ) << 8 ) + tmp[13];

            size_t len = tmp.size() - 14;
            if ( len <= 1524 )
            {
                size_t packet_payload_length = ( uint16_t )( tmp.size() - 14 );
                packet_payload->resize( packet_payload_length );
                memcpy(
                    &( *packet_payload )[0], &tmp[14], packet_payload_length );
            }
            else
            {
                packet_payload->clear();
            }
        }
    }
    return r;
}
}

#else
const char *jdksavdeccmcu_pcapfilereader_file = __FILE__;

#endif
