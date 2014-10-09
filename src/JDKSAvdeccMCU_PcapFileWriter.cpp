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

#ifdef JDKSAVDECCMCU_ENABLE_PCAPFILE

#include "JDKSAvdeccMCU_PcapFileWriter.hpp"

namespace JDKSAvdeccMCU
{

PcapFileWriter::PcapFileWriter( std::string const &filename ) : m_file( filename, "rb" ), m_filename( filename )
{
    /* Did it already exist ?*/
    if ( m_file.get() )
    {
        /* yes, so close and re-open in append mode */
        m_file = PcapFile( filename, "a+b" );
        if ( !m_file.get() )
        {
            throw std::runtime_error( std::string( "Error appending to pcap file: " ) + filename );
        }
    }
    else
    {
        /* The file does not already exist, so create the file and add a wireshark pcap header */
        /* create data logging file in current directory */
        m_file = PcapFile( filename, "wb" );

        if ( m_file.get() )
        {
            pcap_hdr_t header;
            header.magic_number = 0xa1b2c3d4;
            header.version_major = 2;
            header.version_minor = 2;
            header.thiszone = 0;
            header.sigfigs = 0;
            header.snaplen = 0xffff;
            header.network = 1;
            if ( std::fwrite( &header, sizeof( header ), 1, m_file.get() ) )
            {
                throw std::runtime_error( std::string( "Error writing pcap file: " ) + filename );
            }
        }
        else
        {
            throw std::runtime_error( std::string( "Error creating pcap file: " ) + filename );
        }
    }
}

PcapFileWriter::~PcapFileWriter() {}

void PcapFileWriter::WritePacket( PcapFilePacket const &packet )
{
    if ( packet.size() > 14 )
    {
        WritePacket( get_current_time_in_microseconds(), packet );
    }
}

void PcapFileWriter::WritePacket( uint64_t time_in_micros, PcapFilePacket const &packet )
{
    if ( packet.size() > 14 )
    {
        pcaprec_hdr_t pktheader;
        pktheader.ts_sec = uint32_t( time_in_micros / 1000000 );
        pktheader.ts_usec = uint32_t( time_in_micros % 1000000 );
        pktheader.incl_len = uint32_t( packet.size() );
        pktheader.orig_len = pktheader.incl_len;
        if ( std::fwrite( &pktheader, sizeof( pktheader ), 1, m_file.get() ) != 1 )
        {
            throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
        }
        if ( std::fwrite( &packet[0], packet.size(), 1, m_file.get() ) != 1 )
        {
            throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
        }
    }
}

void PcapFileWriter::WritePacket( uint8_t const da[6],
                                  uint8_t const sa[6],
                                  uint16_t ethertype,
                                  PcapFilePacket const &packet_payload )
{
    WritePacket( get_current_time_in_microseconds(), da, sa, ethertype, packet_payload );
}

void PcapFileWriter::WritePacket( uint64_t packet_time_in_micros,
                                  uint8_t const da[6],
                                  uint8_t const sa[6],
                                  uint16_t ethertype,
                                  PcapFilePacket const &packet_payload )
{
    pcaprec_hdr_t pktheader;
    pktheader.ts_sec = uint32_t( packet_time_in_micros / 1000000 );
    pktheader.ts_usec = uint32_t( packet_time_in_micros % 1000000 );
    pktheader.incl_len = int( packet_payload.size() + 14 );
    pktheader.orig_len = pktheader.incl_len;
    if ( std::fwrite( &pktheader, sizeof( pktheader ), 1, m_file.get() ) != 1 )
    {
        throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
    }
    if ( std::fwrite( da, 6, 1, m_file.get() ) != 1 )
    {
        throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
    }

    if ( std::fwrite( sa, 6, 1, m_file.get() ) != 1 )
    {
        throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
    }
    if ( std::fputc( ( ethertype >> 8 ) & 0xff, m_file.get() ) == EOF )
    {
        throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
    }
    if ( std::fputc( ( ethertype >> 0 ) & 0xff, m_file.get() ) == EOF )
    {
        throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
    }
    if ( std::fwrite( &packet_payload[0], packet_payload.size(), 1, m_file.get() ) != 1 )
    {
        throw std::runtime_error( std::string( "Error writing to pcap file: " ) + m_filename );
    }
}
}

#else
const char *jdksavdeccmcu_pcapfilewriter_file = __FILE__;

#endif
