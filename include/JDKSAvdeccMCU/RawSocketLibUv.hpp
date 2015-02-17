/*
  Copyright (c) 2015, J.D. Koftinoff Software, Ltd.
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

#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/RawSocket.hpp"

#if JDKSAVDECCMCU_ENABLE_RAWSOCKETLIBUV

#include "uv-rawpkt.h"

namespace JDKSAvdeccMCU
{
class RawSocketLibUv : public RawSocket
{
  public:
    RawSocketLibUv( uv_loop_t *loop,
                    uint16_t ethertype,
                    Eui48 const &multicast_to_join );

    virtual ~RawSocketLibUv();

    virtual jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
    {
        return JDKSAvdeccMCU::getTimeInMilliseconds();
    }

    virtual bool recvFrame( Frame *frame );

    virtual bool sendFrame( Frame const &frame,
                            uint8_t const *data1,
                            uint16_t len1,
                            uint8_t const *data2,
                            uint16_t len2 );

    virtual bool sendReplyFrame( Frame &frame,
                                 uint8_t const *data1,
                                 uint16_t len1,
                                 uint8_t const *data2,
                                 uint16_t len2 );

    virtual bool joinMulticast( const Eui48 &multicast_mac );

    virtual void setNonblocking();

    virtual Eui48 const &getMACAddress() const { return m_mac_address; }

    virtual void initialize();

  private:
    Eui48 m_mac_address;
    Eui48 m_default_dest_mac_address;
    uint16_t m_ethertype;
};

class RawSocketRunnerLibUv : public RawSocketRunner
{
public:
    RawSocketRunnerLibUv(uv_loop_t *loop)
        : m_loop( loop )
    {}
    virtual ~RawSocketRunnerLibUv() {}
    virtual void setTarget( RawSocketRunnerNotification *notification_target ) = 0;

private:

    uv_loop_t *m_loop;

    static uv_rawpkt_network_port_iterator_t *network_port_iterator;

    /**
     * \brief found_interface
     *
     * Called whenever an ethernet capable network port interface is
     * found
     *
     * \param iter the network port iterator that owns this network port
     * \param port_info the network port
     */
    static void found_interface( uv_rawpkt_network_port_iterator_t *iter,
                                 uv_rawpkt_network_port_t *port_info );

    /**
     * \brief removed_interface
     *
     * Called whenever an ethernet capable network port interface
     * was removed
     *
     * \param iter the network port iterator that owns this network port
     * \param port_info the network port
     */
    static void removed_interface( uv_rawpkt_network_port_iterator_t *iter,
                                   uv_rawpkt_network_port_t *port_info );

    /**
     * \brief The port_context_s struct
     *
     * Local structure to keep track of a linked list of uv_rawpkt_t objects
     * for one protocol type and any additional protocol state required for
     *them.
     *
     * These are kept in a doubly linked list so that the collection
     * of protocol handlers can forward messages between network ports if
     * necessary
     */
    struct port_context
    {
        uv_rawpkt_t rawpkt;
        RawSocketLibUv *port;
        struct port_context_s *next;
        struct port_context_s *prev;
    };

    /**
     * \brief portcontext_first The first entry of the port_context_t object
     * list
     */
    static port_context *portcontext_first;

    /**
     * \brief portcontext_last The last entry of the port_context_t object list
     */
    static port_context *portcontext_last;

    /**
     * \brief rawpkt_closed callback that is called whenever a uv_rawpkt is
     *closed
     *
     * \param handle The pointer to the uv_rawpkt_t as a uv_handle_t
     */
    static void rawpkt_closed( uv_handle_t *handle );

    /**
     * \brief received_packet callback for any received ethernet frames
     *
     * \param rawpkt The uv_rawpkt_t object that received the ethernet frames
     * \param nread The number of ethernet frames received
     * \param buf The list of nread uv_buf_t's containing the ethernet frames
     */
    static void received_ethernet_frame( uv_rawpkt_t *rawpkt,
                                         ssize_t nread,
                                         const uv_buf_t *buf );

};

}
#endif
