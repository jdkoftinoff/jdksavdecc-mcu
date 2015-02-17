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

#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/RawSocketLinux.hpp"

#if defined( __linux__ ) && JDKSAVDECCMCU_ENABLE_RAWSOCKETLINUX
#include <fcntl.h>
#include <signal.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/sockios.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <net/if.h>

namespace JDKSAvdeccMCU
{

void RawSocketTracker::closeAllEthernetPorts()
{
    for ( int i = 0; i < num_rawsockets; ++i )
    {
        delete net[i];
    }
    num_rawsockets = 0;
}

int RawSocketTracker::openAllEthernetPorts( uint16_t ethertype,
                                            const Eui48 &multicast_to_join )
{
    struct ifaddrs *ifaddr, *ifa;
    int family;

    if ( getifaddrs( &ifaddr ) == -1 )
    {
        return 0;
    }

    closeAllEthernetPorts();

    for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next )
    {
        if ( ifa->ifa_addr == NULL )
        {
            continue;
        }

        if ( num_rawsockets >= JDKSAVDECCMCU_MAX_RAWSOCKETS )
        {
            break;
        }

        family = ifa->ifa_addr->sa_family;

        if ( family == AF_PACKET )
        {
            new RawSocketLinux( ifa->ifa_name, ethertype, multicast_to_join );
        }
    }

    atexit( closeAllEthernetPorts );

    freeifaddrs( ifaddr );
    return num_rawsockets;
}

static void rawsocket_linux_initialize()
{
    static bool initted = false;
    if ( !initted )
    {
        struct sigaction act;
        act.sa_handler = SIG_IGN;
        sigemptyset( &act.sa_mask );
        act.sa_flags = 0;
        sigaction( SIGPIPE, &act, NULL );
        initted = true;
    }
}

void RawSocketLinux::initialize() { rawsocket_linux_initialize(); }

RawSocketLinux::RawSocketLinux( const char *device,
                                uint16_t ethertype,
                                const Eui48 &multicast_to_join )
    : m_fd( bad_filedescriptor ), m_device( device ), m_ethertype( ethertype )
{
    initialize();

    m_fd = ::socket( AF_PACKET, SOCK_RAW, htons( ethertype ) );

    m_default_dest_mac_address = multicast_to_join;

    if ( m_fd != bad_filedescriptor && device != 0 )
    {
        int i;
        struct ifreq ifr;
        ::strncpy( ifr.ifr_name, device, sizeof( ifr.ifr_name ) - 1 );
        if ( ::ioctl( m_fd, SIOCGIFINDEX, &ifr ) < 0 )
        {
            ::close( m_fd );
            m_fd = bad_filedescriptor;
            return;
        }
        m_interface_id = ifr.ifr_ifindex;
        if ( ioctl( m_fd, SIOCGIFHWADDR, &ifr ) < 0 )
        {
            ::close( m_fd );
            m_fd = bad_filedescriptor;
            return;
        }
        for ( i = 0; i < ETH_ALEN; ++i )
        {
            m_mac_address.value[i] = (uint8_t)ifr.ifr_hwaddr.sa_data[i];
        }
        if ( multicast_to_join.isSet() )
        {
            joinMulticast( multicast_to_join );
        }
        setNonblocking();
    }
}

RawSocketLinux::~RawSocketLinux()
{
    if ( m_fd != bad_filedescriptor )
    {
        ::close( m_fd );
        m_fd = bad_filedescriptor;
    }
}

bool RawSocketLinux::recvFrame( Frame *frame )
{
    bool r = false;

    if ( m_fd != bad_filedescriptor )
    {
        ssize_t len;

        do
        {
            len = ::recv( m_fd, frame->getBuf(), frame->getMaxLength(), 0 );
        } while ( len < 0 && ( errno == EINTR ) );

        if ( len >= 0 )
        {
            frame->setLength( len );
            r = true;
        }
        else
        {
            frame->setLength( 0 );
        }
    }
    return r;
}

bool RawSocketLinux::sendFrame( const Frame &frame,
                                const uint8_t *data1,
                                uint16_t len1,
                                const uint8_t *data2,
                                uint16_t len2 )
{
    bool r = false;

    if ( m_fd != bad_filedescriptor )
    {
        ssize_t sent_len = -1;
        /// fully formed ethernet frame buffer
        uint8_t buffer[ETH_FRAME_LEN];

        /// Fill it in with our data concatenated
        uint16_t buffer_length = 0;
        memcpy( buffer, frame.getBuf(), frame.getLength() );
        buffer_length += frame.getLength();
        if ( data1 != 0 && len1 > 0 )
        {
            if ( buffer_length + len1 > ETH_FRAME_LEN )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data1, len1 );
            buffer_length += len1;
        }
        if ( data2 != 0 && len2 > 0 )
        {
            if ( buffer_length + len2 > ETH_FRAME_LEN )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data2, len2 );
            buffer_length += len2;
        }

        /// ptr to the ethernet header in buffer
        unsigned char *etherhead = buffer;

        /// representation of ethernet header as ethhdr structure
        struct ethhdr *eh = (struct ethhdr *)etherhead;

        /// the linux link local socket address
        struct sockaddr_ll socket_address;

        // Fill in socket_address
        socket_address.sll_family = PF_PACKET;
        socket_address.sll_protocol = htons( m_ethertype );
        socket_address.sll_ifindex = m_interface_id;
        socket_address.sll_hatype = 1; /*ARPHRD_ETHER; */
        socket_address.sll_pkttype = PACKET_OTHERHOST;
        socket_address.sll_halen = ETH_ALEN;
        memcpy( socket_address.sll_addr, &m_mac_address.value[0], ETH_ALEN );
        socket_address.sll_addr[6] = 0x00;
        socket_address.sll_addr[7] = 0x00;

        // If dest address is not set in the frame we want to send, then fill in
        // the default destination address
        if ( eh->h_dest[0] == 0 && eh->h_dest[1] == 0 && eh->h_dest[2] == 0
             && eh->h_dest[3] == 0 && eh->h_dest[4] == 0 && eh->h_dest[5] == 0 )
        {
            memcpy( &eh->h_dest[0],
                    &m_default_dest_mac_address.value[0],
                    ETH_ALEN );
        }

        // Set the src address of the frame to match our real ethernet MAC
        // address
        memcpy( &eh->h_source[0], &m_mac_address.value[0], ETH_ALEN );

        // Set the ethertype of the frame to match our real ethernet MAC address
        eh->h_proto = htons( m_ethertype );

        // pad the buffer with zeros to fill in the minimum payload size
        if ( buffer_length < JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH )
        {
            memset( &buffer[buffer_length],
                    0,
                    JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH - buffer_length );
            buffer_length = JDKSAVDECCMCU_RAWSOCKET_MIN_FRAME_LENGTH;
        }

        do
        {
            sent_len = sendto( m_fd,
                               buffer,
                               buffer_length,
                               0,
                               (struct sockaddr *)&socket_address,
                               sizeof( socket_address ) );
        } while ( sent_len < 0 && ( errno == EINTR ) );

        if ( sent_len == buffer_length )
        {
            r = true;
        }
    }

    return r;
}

bool RawSocketLinux::sendReplyFrame( Frame &frame,
                                     const uint8_t *data1,
                                     uint16_t len1,
                                     const uint8_t *data2,
                                     uint16_t len2 )
{
    bool r = false;

    if ( m_fd != bad_filedescriptor )
    {
        ssize_t sent_len = -1;

        /// fully formed ethernet frame buffer
        uint8_t buffer[ETH_FRAME_LEN];

        /// Fill it in with our data concatenated
        uint16_t buffer_length = 0;
        memcpy( buffer, frame.getBuf(), frame.getLength() );
        buffer_length += frame.getLength();
        if ( data1 != 0 && len1 > 0 )
        {
            if ( buffer_length + len1 > ETH_FRAME_LEN )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data1, len1 );
            buffer_length += len1;
        }
        if ( data2 != 0 && len2 > 0 )
        {
            if ( buffer_length + len2 > ETH_FRAME_LEN )
            {
                return false;
            }
            memcpy( &buffer[buffer_length], data2, len2 );
            buffer_length += len2;
        }

        /// ptr to the ethernet header in buffer
        unsigned char *etherhead = buffer;

        /// representation of ethernet header as ethhdr structure
        struct ethhdr *eh = (struct ethhdr *)etherhead;

        /// the linux link local socket address
        struct sockaddr_ll socket_address;

        // Fill in socket_address
        socket_address.sll_family = PF_PACKET;
        socket_address.sll_protocol = htons( m_ethertype );
        socket_address.sll_ifindex = m_interface_id;
        socket_address.sll_hatype = 1; /*ARPHRD_ETHER; */
        socket_address.sll_pkttype = PACKET_OTHERHOST;
        socket_address.sll_halen = ETH_ALEN;
        memcpy( socket_address.sll_addr, &m_mac_address.value[0], ETH_ALEN );
        socket_address.sll_addr[6] = 0x00;
        socket_address.sll_addr[7] = 0x00;

        // set the destination address to what the source was
        memcpy( &eh->h_dest[0], &eh->h_source[0], ETH_ALEN );

        // make sure it was not a multicast
        eh->h_dest[0] &= 0xfe;

        // Set the src address of the frame to match our real ethernet MAC
        // address
        memcpy( &eh->h_source[0], &m_mac_address.value[0], ETH_ALEN );

        // Set the ethertype of the frame to match our real ethernet MAC address
        eh->h_proto = htons( m_ethertype );

        do
        {
            sent_len = sendto( m_fd,
                               buffer,
                               buffer_length,
                               0,
                               (struct sockaddr *)&socket_address,
                               sizeof( socket_address ) );
        } while ( sent_len < 0 && ( errno == EINTR ) );

        if ( sent_len == buffer_length )
        {
            r = true;
        }
    }

    return r;
}

bool RawSocketLinux::joinMulticast( const Eui48 &multicast_mac )
{
    bool r = false;
    struct packet_mreq mreq;
    struct sockaddr_ll saddr;

    ::memset( &saddr, 0, sizeof( saddr ) );
    saddr.sll_family = AF_PACKET;
    saddr.sll_ifindex = m_interface_id;
    saddr.sll_pkttype = PACKET_MULTICAST;
    saddr.sll_protocol = htons( m_ethertype );
    if ( ::bind( m_fd, (struct sockaddr *)&saddr, sizeof( saddr ) ) >= 0 )
    {
        ::memset( &mreq, 0, sizeof( mreq ) );
        mreq.mr_ifindex = m_interface_id;
        mreq.mr_type = PACKET_MR_MULTICAST;
        mreq.mr_alen = ETH_ALEN;
        memcpy( &mreq.mr_address[0], &multicast_mac.value[0], ETH_ALEN );

        if ( ::setsockopt( m_fd,
                           SOL_PACKET,
                           PACKET_ADD_MEMBERSHIP,
                           &mreq,
                           sizeof( mreq ) ) >= 0 )
        {
            r = true;
        }
        else
        {
            r = false;
        }
    }
    else
    {
        r = false;
    }

    return r;
}

void RawSocketLinux::setNonblocking()
{
    int val;
    int flags;
    val = ::fcntl( m_fd, F_GETFL, 0 );
    flags = O_NONBLOCK;
    val |= flags;
    ::fcntl( m_fd, F_SETFL, val );
}
}
#else
const char *jdksavdeccmcu_rawsocketlinux_file = __FILE__;
#endif
