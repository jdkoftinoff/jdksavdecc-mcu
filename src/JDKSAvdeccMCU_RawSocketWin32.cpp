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
#include "JDKSAvdeccMCU_RawSocketWin32.hpp"

#if defined( _WIN32 ) && JDKSAVDECCMCU_ENABLE_RAWSOCKETWIN32

#include <pcap.h>
#include <iphlpapi.h>
#include <winsock2.h>
#pragma comment( lib, "IPHLPAPI.lib" )
#pragma comment( lib, "wpcap.lib" )
#pragma comment( lib, "Ws2_32.lib" )

namespace JDKSAvdeccMCU
{
static pcap_if_t *rawsocket_win32_raw_alldevs = 0;

static void rawsocket_win32_raw_cleanup();

static void rawsocket_win32_raw_cleanup()
{
    if ( rawsocket_win32_raw_alldevs )
    {
        pcap_freealldevs( rawsocket_win32_raw_alldevs );
    }
}

static void rawsocket_win32_initialize()
{
    static bool initted = false;
    if ( !initted )
    {
        WSADATA wsaData;
        WORD version;
        int error;
        version = MAKEWORD( 2, 2 );
        error = WSAStartup( version, &wsaData );
        if ( error == 0 && version == wsaData.wVersion )
        {
            initted = true;
        }
    }
}

RawSocketWin32::RawSocketWin32( const char *device,
                                uint16_t ethertype,
                                const jdksavdecc_eui48 *multicast_to_join )
    : m_fd( bad_filedescriptor ), m_device( device ), m_ethertype( ethertype )
{
    filedescriptor_type r = -1;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *p;

    m_ethertype = ethertype;
    if ( multicast_to_join )
    {
        m_default_dest_mac_address = *multicast_to_join;
    }
    else
    {
        jdksavdecc_eui48_zero( &m_default_dest_mac_address );
    }

    p = pcap_open_live( device, 65536, 1, 1, errbuf );
    m_pcap = (void *)p;

    if ( !p )
    {
        // fprintf( stderr, "pcap open error on interface '%s': %s\n",
        // interface_name, errbuf );
        r = bad_filedescriptor;
        return;
    }
    else
    {
        int dl = pcap_datalink( p );

        if ( dl != DLT_EN10MB && dl != DLT_IEEE802_11 )
        {
            // fprintf( stderr, "Interface %s is not an Ethernet or wireless
            // interface\n", interface_name );
            r = bad_filedescriptor;
        }
        else
        {
            pcap_if_t *d = 0;
            m_interface_id = -1;
            if ( rawsocket_win32_raw_alldevs == 0 )
            {
                if ( pcap_findalldevs( &rawsocket_win32_raw_alldevs, errbuf )
                     != 0 || rawsocket_win32_raw_alldevs == 0 )
                {
                    // fprintf( stderr, "pcap_findalldevs failed\n" );
                    pcap_close( p );
                    return;
                }
            }
            {
                for ( d = rawsocket_win32_raw_alldevs; d != NULL; d = d->next )
                {
                    m_interface_id++;

                    /* find the interface by name */
                    if ( strcmp( device, d->name ) == 0 )
                    {
                        PIP_ADAPTER_INFO info = NULL, ninfo;
                        ULONG ulOutBufLen = 0;
                        DWORD dwRetVal = 0;
                        if ( GetAdaptersInfo( info, &ulOutBufLen )
                             == ERROR_BUFFER_OVERFLOW )
                        {
                            info = (PIP_ADAPTER_INFO)malloc( ulOutBufLen );
                            if ( info != NULL )
                            {
                                if ( ( dwRetVal = GetAdaptersInfo(
                                           info, &ulOutBufLen ) ) == NO_ERROR )
                                {
                                    ninfo = info;
                                    while ( ninfo != NULL )
                                    {
                                        if ( strstr( d->name,
                                                     ninfo->AdapterName ) > 0 )
                                        {
                                            if ( ninfo->AddressLength == 6 )
                                                memcpy( &m_mac_address.value[0],
                                                        ninfo->Address,
                                                        6 );
                                            break;
                                        }
                                        ninfo = ninfo->Next;
                                    }
                                }
                                else
                                {
                                    // fprintf( stderr, "Error in
                                    // GetAdaptersInfo\n" );
                                    r = bad_filedescriptor;
                                }
                                free( info );
                            }
                            else
                            {
                                // fprintf( stderr, "Error in malloc for
                                // GetAdaptersInfo\n" );
                                r = bad_filedescriptor;
                            }
                        }
                        break;
                    }
                }

                if ( m_interface_id == -1 )
                {
                    // fprintf( stderr, "unable to get MAC address for interface
                    // '%s'\n", interface_name );
                    r = r = bad_filedescriptor;
                }
                else
                {
                    /* enable ether protocol filter */
                    if (multicast_to_join)
                    {
                        joinMulticast(*multicast_to_join);
                    }
                    r = pcap_fileno( p );
                    if ( m_fd == bad_filedescriptor )
                    {
                        // fprintf( stderr, "Unable to get pcap fd\n" );
                        r = bad_filedescriptor;
                    }
                    else
                    {
                        r = m_fd;
                    }
                }
            }
        }
    }

    if ( r == bad_filedescriptor )
    {
        if ( p )
        {
            pcap_close( p );
            m_pcap = 0;
        }
    }
    else
    {
        setNonblocking();
    }
    m_fd = r;
}

RawSocketWin32::~RawSocketWin32()
{
    if ( m_fd != bad_filedescriptor )
    {
        _close( m_fd );
        m_fd = bad_filedescriptor;
    }

    if ( m_pcap )
    {
        pcap_close( (pcap_t *)m_pcap );
        m_pcap = 0;
    }
}

bool RawSocketWin32::recvFrame( Frame *frame )
{
    bool r = false;
    pcap_t *pcap = (pcap_t *)m_pcap;

    if ( pcap )
    {
        const uint8_t *data;
        struct pcap_pkthdr *header;
        int e = pcap_next_ex( pcap, &header, &data );

        if ( e == 1 && ( (ssize_t)header->caplen <= frame->getMaxLength() ) )
        {
            memcpy( frame->getBuf(), &data, r );
            frame->setLength( header->caplen );
            frame->setTimeInMilliseconds( ( header->ts.tv_sec * 1000 )
                                          + ( header->ts.tv_usec / 1000 ) );
            r = true;
        }
    }
    return r;
}

bool RawSocketWin32::sendFrame( const Frame &frame,
                                const uint8_t *data1,
                                uint16_t len1,
                                const uint8_t *data2,
                                uint16_t len2 )
{
    pcap_t *pcap = (pcap_t *)m_pcap;
    bool r = false;

    if ( m_fd != bad_filedescriptor )
    {
        /// fully formed ethernet frame buffer
        uint8_t buffer[1514];

        /// Fill it in with our data concatenated
        uint16_t buffer_length = 0;
        memcpy( buffer, frame.getBuf(), frame.getLength() );
        buffer_length += frame.getLength();
        if ( data1 != 0 && len1 > 0 )
        {
            if ( buffer_length + len1 > sizeof( buffer ) )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data1, len1 );
            buffer_length += len1;
        }
        if ( data2 != 0 && len2 > 0 )
        {
            if ( buffer_length + len2 > sizeof( buffer ) )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data2, len2 );
            buffer_length += len2;
        }

        // If dest address is not set in the frame we want to send, then fill in
        // the default destination address
        uint8_t *h_dest = &buffer[0];
        if ( h_dest[0] == 0 && h_dest[1] == 0 && h_dest[2] == 0
             && h_dest[3] == 0 && h_dest[4] == 0 && h_dest[5] == 0 )
        {
            memcpy( &h_dest[0], &m_default_dest_mac_address.value[0], 6 );
        }

        // Set the src address of the frame to match our real ethernet MAC
        // address
        uint8_t *h_source = &buffer[6];
        memcpy( &h_source[0], &m_mac_address.value[0], 6 );

        // Set the ethertype of the frame to match our real ethernet MAC address
        uint8_t *h_proto = &buffer[12];
        h_proto[0] = ( m_ethertype >> 8 ) & 0xff;
        h_proto[1] = ( m_ethertype >> 0 ) & 0xff;

        // pad the buffer with zeros to fill in the minimum payload size
        if ( buffer_length < JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH )
        {
            memset( &buffer[buffer_length],
                    0,
                    JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH - buffer_length );
            buffer_length = JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH;
        }

        r = pcap_sendpacket( pcap, buffer, buffer_length ) == 0 ? true : false;
    }

    return r;
}

bool RawSocketWin32::sendReplyFrame( Frame &frame,
                                     const uint8_t *data1,
                                     uint16_t len1,
                                     const uint8_t *data2,
                                     uint16_t len2 )
{
    pcap_t *pcap = (pcap_t *)m_pcap;
    bool r = false;

    if ( m_fd != bad_filedescriptor )
    {
        /// fully formed ethernet frame buffer
        uint8_t buffer[1514];

        /// Fill it in with our data concatenated
        uint16_t buffer_length = 0;
        memcpy( buffer, frame.getBuf(), frame.getLength() );
        buffer_length += frame.getLength();
        if ( data1 != 0 && len1 > 0 )
        {
            if ( buffer_length + len1 > sizeof( buffer ) )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data1, len1 );
            buffer_length += len1;
        }
        if ( data2 != 0 && len2 > 0 )
        {
            if ( buffer_length + len2 > sizeof( buffer ) )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data2, len2 );
            buffer_length += len2;
        }

        // set the destination address to what the source was
        uint8_t *h_dest = &buffer[0];
        uint8_t *h_source = &buffer[6];
        uint8_t *h_proto = &buffer[12];

        memcpy( &h_dest[0], &h_source[0], 6 );

        // make sure it was not a multicast
        h_dest[0] &= 0xfe;

        // Set the src address of the frame to match our real ethernet MAC
        // address
        memcpy( &h_source[0], &m_mac_address.value[0], 6 );

        // Set the ethertype of the frame to match our real ethernet MAC address
        h_proto[0] = ( m_ethertype >> 8 ) & 0xff;
        h_proto[1] = ( m_ethertype >> 0 ) & 0xff;

        // pad the buffer with zeros to fill in the minimum payload size
        if ( buffer_length < JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH )
        {
            memset( &buffer[buffer_length],
                    0,
                    JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH - buffer_length );
            buffer_length = JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH;
        }

        r = pcap_sendpacket( pcap, buffer, buffer_length ) == 0 ? true : false;
    }

    return r;
}

bool RawSocketWin32::joinMulticast( const jdksavdecc_eui48 &multicast_mac )
{
    bool r = false;
    struct bpf_program fcode;
    pcap_t *p = (pcap_t *)m_pcap;
    char filter[1024];
    /* TODO: add multicast address to pcap filter here if multicast_mac is set
     */
    (void)multicast_mac;
    sprintf_s( filter, "ether proto 0x%04x", m_ethertype );

    if ( pcap_compile( p, &fcode, filter, 1, 0xffffffff ) < 0 )
    {
        pcap_close( p );
        // fprintf( stderr, "Unable to pcap_compile: '%s'\n", filter );
        r = false;
    }
    else
    {
        if ( pcap_setfilter( p, &fcode ) < 0 )
        {
            pcap_close( p );
            // fprintf( stderr, "Unable to pcap_setfilter\n" );
            r = false;
        }
        else
        {
            r = true;
        }
        pcap_freecode( &fcode );
    }
    return r;
}

void RawSocketWin32::setNonblocking()
{
    u_long mode = 1;
    if ( ioctlsocket( m_fd, FIONBIO, &mode ) != NO_ERROR )
    {
        // fprintf(stderr, "fcntl F_SETFL O_NONBLOCK failed\n");
    }
}

void RawSocketWin32::initialize() { rawsocket_win32_initialize(); }
}

#else
const char *jdksavdeccmcu_rawsocketwin32_file = __FILE__;
#endif
