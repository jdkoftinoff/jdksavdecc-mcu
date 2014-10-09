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

#ifdef JDKSAVDECCMCU_ENABLE_RAW

#if defined( __linux__ )
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <errno.h>
#include <strings.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/sockios.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#elif defined( __APPLE__ )

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <errno.h>
#include <strings.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <net/if_dl.h>
#include <pcap.h>

#define AF_PACKET AF_LINK

#elif defined( _WIN32 )
#include <Windows.h>
#if defined( ENABLE_PCAP ) && ENABLE_PCAP == 1
#include <pcap.h>
#include <iphlpapi.h>
#include <winsock2.h>
#pragma comment( lib, "IPHLPAPI.lib" )
#pragma comment( lib, "wpcap.lib" )
#pragma comment( lib, "Ws2_32.lib" )
#endif
#endif


namespace JDKSAvdeccMCU
{
#if !( defined( _WIN32 ) && ( !defined( ENABLE_PCAP ) || ENABLE_PCAP == 0 ) )
#if defined( __APPLE__ ) || defined( _WIN32 )
static pcap_if_t *raw_alldevs = 0;

static void raw_cleanup();

static void raw_cleanup()
{
    if ( raw_alldevs )
    {
        pcap_freealldevs( raw_alldevs );
    }
}

#endif


RawNetIO::RawNetIO( uint16_t ethertype, const char *interface_name, const uint8_t join_multicast[] )
{
#if defined( __linux__ )
    init_sockets();
    int fd = socket( AF_PACKET, SOCK_RAW, htons( ethertype ) );

    if ( join_multicast )
    {
        memcpy( m_default_dest_mac, join_multicast, 6 );
    }

    if ( fd >= 0 && interface_name )
    {
        int i;
        struct ifreq ifr;
        strncpy( ifr.ifr_name, interface_name, sizeof( ifr.ifr_name ) - 1 );
        if ( ioctl( fd, SIOCGIFINDEX, &ifr ) < 0 )
        {
            close( fd );
            return -1;
        }
        m_interface_id = ifr.ifr_ifindex;
        if ( ioctl( fd, SIOCGIFHWADDR, &ifr ) < 0 )
        {
            close( fd );
            return -1;
        }
        for ( i = 0; i < 6; ++i )
        {
            m_my_mac[i] = (uint8_t)ifr.ifr_hwaddr.sa_data[i];
        }
        m_fd = fd;
        m_ethertype = ethertype;
        if ( join_multicast )
        {
            joinMulticast( self, join_multicast );
        }
        setNonblocking();
    }
    return fd;
#elif defined( __APPLE__ ) || defined( _WIN32 )
    int r = -1;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *p;

    m_ethertype = ethertype;
    if ( join_multicast )
    {
        memcpy( m_default_dest_mac, join_multicast, 6 );
    }

    p = pcap_open_live( interface_name, 65536, 1, 1, errbuf );
    m_pcap = (void *)p;

    if ( !p )
    {
        fprintf( stderr, "pcap open error on interface '%s': %s\n", interface_name, errbuf );
    }
    else
    {
        int dl = pcap_datalink( p );

        if ( dl != DLT_EN10MB && dl != DLT_IEEE802_11 )
        {
            fprintf( stderr, "Interface %s is not an Ethernet or wireless interface\n", interface_name );
        }
        else
        {
            pcap_if_t *d = 0;
            m_interface_id = -1;
            if ( raw_alldevs == 0 )
            {
                if ( pcap_findalldevs( &raw_alldevs, errbuf ) != 0 || raw_alldevs == 0 )
                {
                    fprintf( stderr, "pcap_findalldevs failed\n" );
                    pcap_close( p );
                    return;
                }
                atexit( raw_cleanup );
            }
            {
                for ( d = raw_alldevs; d != NULL; d = d->next )
                {
                    m_interface_id++;

                    /* find the interface by name */
                    if ( strcmp( interface_name, d->name ) == 0 )
                    {
/* now find the MAC address associated with it */
#if defined( _WIN32 )
                        PIP_ADAPTER_INFO info = NULL, ninfo;
                        ULONG ulOutBufLen = 0;
                        DWORD dwRetVal = 0;
                        if ( GetAdaptersInfo( info, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW )
                        {
                            info = (PIP_ADAPTER_INFO)malloc( ulOutBufLen );
                            if ( info != NULL )
                            {
                                if ( ( dwRetVal = GetAdaptersInfo( info, &ulOutBufLen ) ) == NO_ERROR )
                                {
                                    ninfo = info;
                                    while ( ninfo != NULL )
                                    {
                                        if ( strstr( d->name, ninfo->AdapterName ) > 0 )
                                        {
                                            if ( ninfo->AddressLength == 6 )
                                                memcpy( m_my_mac, ninfo->Address, 6 );
                                            break;
                                        }
                                        ninfo = ninfo->Next;
                                    }
                                }
                                else
                                {
                                    fprintf( stderr, "Error in GetAdaptersInfo\n" );
                                }
                                free( info );
                            }
                            else
                            {
                                fprintf( stderr, "Error in malloc for GetAdaptersInfo\n" );
                            }
                        }
#else
                        pcap_addr_t *alladdrs;
                        pcap_addr_t *a;
                        alladdrs = d->addresses;
                        for ( a = alladdrs; a != NULL; a = a->next )
                        {
                            if ( a->addr->sa_family == AF_LINK )
                            {
                                uint8_t const *mac;
                                struct sockaddr_dl *dl = (struct sockaddr_dl *)a->addr;
                                mac = (uint8_t const *)dl->sdl_data + dl->sdl_nlen;

                                memcpy( m_my_mac, mac, 6 );
                            }
                        }
#endif
                        break;
                    }
                }

                if ( m_interface_id == -1 )
                {
                    fprintf( stderr, "unable to get MAC address for interface '%s'\n", interface_name );
                }
                else
                {
                    /* enable ether protocol filter */
                    joinMulticast( join_multicast );
                    m_fd = pcap_fileno( p );
                    if ( m_fd == -1 )
                    {
                        fprintf( stderr, "Unable to get pcap fd\n" );
                    }
                    else
                    {
                        r = m_fd;
                    }
                }
            }
        }
    }

    if ( r == -1 )
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
#endif
}

RawNetIO::~RawNetIO()
{
#if defined( __linux__ )
    if ( m_fd >= 0 )
    {
        close( m_fd );
        m_fd = -1;
    }
#elif defined( __APPLE__ )
    if ( m_fd >= 0 )
    {
        close( m_fd );
        m_fd = -1;
    }

    if ( m_pcap )
    {
        pcap_close( (pcap_t *)m_pcap );
        m_pcap = 0;
    }

#elif defined( _WIN32 )
    if ( m_fd >= 0 )
    {
        _close( m_fd );
        m_fd = -1;
    }

    if ( m_pcap )
    {
        pcap_close( m_pcap );
        m_pcap = 0;
    }
#endif
}

void RawNetIO::initialize()
{
    static bool initted = false;
    if ( !initted )
    {
#if defined( _WIN32 )

        WSADATA wsaData;
        WORD version;
        int error;
        version = MAKEWORD( 2, 2 );
        error = WSAStartup( version, &wsaData );
        if ( error != 0 )
        {
            return false;
        }
        if ( version != wsaData.wVersion )
        {
            return false;
        }
        initted = true;
        return true;

#elif defined( __linux__ ) || defined( __APPLE__ )

        struct sigaction act;
        act.sa_handler = SIG_IGN;
        sigemptyset( &act.sa_mask );
        act.sa_flags = 0;
        sigaction( SIGPIPE, &act, NULL );
        initted = true;
        return true;

#endif
    }
}

const jdksavdecc_eui48 &RawNetIO::getMACAddress() const
{

}

bool RawNetIO::joinMulticast( const uint8_t multicast_mac[6] )
{
}

uint16_t RawNetIO::receiveRawNet( uint8_t *data, uint16_t max_len )
{
#if defined( __linux__ )
    ssize_t r = -1;
    ssize_t buf_len;
    uint8_t buf[2048];

    do
    {
        buf_len = recv( m_fd, buf, sizeof( buf ), 0 );
    } while ( buf_len < 0 && ( errno == EINTR ) );

    if ( buf_len >= 0 )
    {
        if ( src_mac )
        {
            memcpy( src_mac, &buf[6], 6 );
        }
        if ( dest_mac )
        {
            memcpy( dest_mac, &buf[0], 6 );
        }
        if ( payload_buf && ( payload_buf_max_size > buf_len - 14 ) )
        {
            memcpy( payload_buf, &buf[14], buf_len - 14 );
            r = buf_len - 14;
        }
    }
    return r;
#elif defined( __APPLE__ ) || defined( _WIN32 )
    ssize_t r = -1;
    pcap_t *pcap = (pcap_t *)m_pcap;

    if ( pcap )
    {
        const uint8_t *data;
        struct pcap_pkthdr *header;
        int e = pcap_next_ex( pcap, &header, &data );

        if ( e == 1 && ( (ssize_t)header->caplen - 14 ) <= payload_buf_max_size )
        {
            r = header->caplen - 14;
            memcpy( payload_buf, &data[14], r );
            if ( src_mac )
            {
                memcpy( src_mac, &data[6], 6 );
            }
            if ( dest_mac )
            {
                memcpy( dest_mac, &data[0], 6 );
            }
        }
    }
    return r;

#endif
}

bool RawNetIO::sendRawNet(
    const uint8_t *data, uint16_t len, const uint8_t *data1, uint16_t len1, const uint8_t *data2, uint16_t len2 )
{
#if defined( __linux__ )
    ssize_t r = -1;
    ssize_t sent_len;
    struct sockaddr_ll socket_address;
    uint8_t buffer[ETH_FRAME_LEN];
    unsigned char *etherhead = buffer;
    unsigned char *data = buffer + 14;
    struct ethhdr *eh = (struct ethhdr *)etherhead;
    socket_address.sll_family = PF_PACKET;
    socket_address.sll_protocol = htons( m_ethertype );
    socket_address.sll_ifindex = m_interface_id;
    socket_address.sll_hatype = 1; /*ARPHRD_ETHER; */
    socket_address.sll_pkttype = PACKET_OTHERHOST;
    socket_address.sll_halen = ETH_ALEN;
    memcpy( socket_address.sll_addr, m_my_mac, ETH_ALEN );
    socket_address.sll_addr[6] = 0x00;
    socket_address.sll_addr[7] = 0x00;

    if ( dest_mac )
    {
        memcpy( (void *)buffer, (void *)dest_mac, ETH_ALEN );
    }
    else
    {
        memcpy( (void *)buffer, (void *)m_default_dest_mac, 6 );
    }

    memcpy( (void *)( buffer + ETH_ALEN ), (void *)m_my_mac, ETH_ALEN );
    eh->h_proto = htons( m_ethertype );
    memcpy( data, payload, payload_len );
    do
    {
        sent_len = sendto( m_fd, buffer, payload_len + 14, 0, (struct sockaddr *)&socket_address, sizeof( socket_address ) );
    } while ( sent_len < 0 && ( errno == EINTR ) );
    if ( sent_len >= 0 )
    {
        r = sent_len - 14;
    }

    return r;
#elif defined( __APPLE__ ) || defined( _WIN32 )
    int r = 0;
    pcap_t *pcap = (pcap_t *)m_pcap;

    if ( pcap )
    {
        uint8_t buffer[2048];
        uint8_t *data = buffer + 14;
        if ( dest_mac )
        {
            memcpy( (void *)buffer, (void *)dest_mac, 6 );
        }
        else
        {
            memcpy( (void *)buffer, (void *)m_default_dest_mac, 6 );
        }
        memcpy( (void *)( buffer + 6 ), (void *)m_my_mac, 6 );
        buffer[12] = ( m_ethertype >> 8 ) & 0xff;
        buffer[13] = ( m_ethertype & 0xff );
        memcpy( data, payload, payload_len );
        r = pcap_sendpacket( pcap, buffer, (int)payload_len + 14 ) == 0;
    }
    else
    {
        r = false;
    }
    return r ? payload_len : -1;

#endif
}

bool RawNetIO::sendReplyRawNet(
    const uint8_t *data, uint16_t len, const uint8_t *data1, uint16_t len1, const uint8_t *data2, uint16_t len2 )
{
}
}

#else

const char *jdksavdeccmcu_rawnetio_file = __FILE__;

#endif
