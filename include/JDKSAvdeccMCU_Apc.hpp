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
#include "JDKSAvdeccMCU_App.hpp"

namespace JDKSAvdeccMCU
{

struct jdksnet_packet_slots;
struct jdksnet_packet_signals;

struct jdksnet_packet_signals
{
    void *m_target;

    /**
     * External Networking Event: The network port obtained link
     */
    void ( *link_up )( struct jdksnet_packet_signals *self,
                       uint64_t bps,
                       struct jdksavdecc_eui48 link_addr );

    /**
     * External Networking Event: The network port lost link
     */
    void ( *link_down )( struct jdksnet_packet_signals *self,
                         uint64_t bps,
                         struct jdksavdecc_eui48 link_addr );

    /**
     * External Networking Event: The socket was readable and some data was read
     */
    void ( *readable )( struct jdksnet_packet_signals *self,
                        const struct jdksavdecc_frame *frame );

    /**
     * External Networking Event: The socket is writable now
     */
    void ( *writable )( struct jdksnet_packet_signals *self );

    /**
     * External Networking Event: The socket was closed
     */
    void ( *closed )( struct jdksnet_packet_signals *self );

    /**
     * External Networking Event: Some time passed
     */
    void ( *tick )( struct jdksnet_packet_signals *self,
                    jdksavdecc_timestamp_in_microseconds timestamp );
};

struct jdksnet_packet_slots;
struct jdksnet_packet_signals;

struct jdksnet_packet_slots
{
    /**
     * Ask the object to terminate
     */
    void ( *terminate )( struct jdksnet_packet_slots *self );

    /**
     * Ask the object to send signals to the destination_signals object
     */
    void ( *connect_signals )(
        struct jdksnet_packet_slots *self,
        struct jdksnet_packet_signals *destination_signals );

    /**
     * Ask the object to disconnect sending of signals to the
     * destinaiton_signals object
     */
    void ( *disconnect_signals )(
        struct jdksnet_packet_slots *self,
        struct jdksnet_packet_signals *destination_signals );

    /**
     * External Networking Request: The client object wants to wake up when the
     * socket is writable
     */
    void ( *wake_on_writable )( struct jdksnet_packet_slots *self,
                                bool enable );

    /**
     * External Networking Request: The client object wants to send a frame
     */
    ssize_t ( *send )( struct jdksnet_packet_slots *self,
                       const struct jdksavdecc_frame *frame );

    /**
     * External Networking Request: The client object wants to be woken up in
     * the future
     */
    void ( *wake_up )( struct jdksnet_packet_slots *self,
                       uint64_t delta_time_in_microseconds );
};

struct jdksnet_stream_slots;
struct jdksnet_stream_signals;

/** Collection of function pointers that will be called when a networking event
 * occurs
 */
struct jdksnet_stream_signals
{
    void *m_target;

    /**
     * External Networking Event: The socket was readable and some data was read
     */
    void ( *readable )( struct jdksnet_stream_signals *self,
                        const struct jdksavdecc_frame *frame );

    /**
     * External Networking Event: The socket was connected
     */
    void ( *connected )( struct jdksnet_stream_signals *self,
                         const struct sockaddr *local_addr,
                         socklen_t local_addr_len,
                         const struct sockaddr *remote_addr,
                         socklen_t remote_addr_len );

    /**
     * External Networking Event: The socket is writable now
     */
    void ( *writable )( struct jdksnet_stream_signals *self );

    /**
     * External Networking Event: The socket was closed
     */
    void ( *closed )( struct jdksnet_stream_signals *self );

    /**
     * External Networking Event: Some time passed
     */
    void ( *tick )( struct jdksnet_stream_signals *self,
                    jdksavdecc_timestamp_in_microseconds timestamp );
};

struct jdksnet_stream_slots;
struct jdksnet_stream_signals;

/** Collection of function pointers used to control a TCP socket
 */
struct jdksnet_stream_slots
{
    void *m_target;

    /**
     * Ask the object to terminate
     */
    void ( *terminate )( struct jdksnet_stream_slots *self );

    /**
     * Ask the object to send signals to the destination_signals object
     */
    void ( *connect_signals )(
        struct jdksnet_stream_slots *self,
        struct jdksnet_stream_signals *destination_signals );

    /**
     * Ask the object to disconnect sending of signals to the
     * destinaiton_signals object
     */
    void ( *disconnect_signals )(
        struct jdksnet_stream_slots *self,
        struct jdksnet_stream_signals *destination_signals );

    /**
     * External Networking Request: The client object wants to wake up when the
     * socket is writable
     */
    void ( *wake_on_writable )( struct jdksnet_stream_slots *self,
                                bool enable );

    /**
     * External Networking Request: The client object wants to connect to a
     * destination
     */
    void ( *connect )( struct jdksnet_stream_slots *self,
                       struct sockaddr const *addr,
                       socklen_t addr_len );

    /**
     * External Networking Request: The client object wants to close the socket
     */
    void ( *close )( struct jdksnet_stream_slots *self,
                     struct jdksnet_stream_signals *net_event_handler );

    /**
     * External Networking Request: The client object wants to send a frame
     */
    ssize_t ( *send )( struct jdksnet_stream_slots *self,
                       const struct jdksavdecc_frame *frame );

    /**
     * External Networking Request: The client object wants to be woken up in
     * the future
     */
    void ( *wake_up )( struct jdksnet_stream_slots *self,
                       uint64_t delta_time_in_microseconds );
};

struct jdksapc_Addr
{
    struct sockaddr *m_addr;
    // socklen_t m_addr_len;
    char m_path[4096];
    char m_host[1024];
    char m_port[128];
    char *m_additional_request_headers;
};

/**
 * @brief The jdksapc_state enum
 *
 * See IEEE Std 1722.1 Annex C.5.3.3, Figure C.3
 *
 */
enum jdksapc_state
{
    jdksapc_state_BEGIN = 0,
    jdksapc_state_INITIALIZE,
    jdksapc_state_WAIT_FOR_CONNECT,
    jdksapc_state_ACCEPT,
    jdksapc_state_START_TRANSFER,
    jdksapc_state_WAITING,
    jdksapc_state_CLOSED,
    jdksapc_state_LINK_STATUS,
    jdksapc_state_RECEIVE_MSG,
    jdksapc_state_SEND_MSG,
    jdksapc_state_ENTITY_ID_ASSIGNED,
    jdksapc_state_SEND_NOP,
    jdksapc_state_FINISH,
    jdksapc_state_END
};

struct jdksapc
{
    struct jdksnet_stream_signals m_server_stream_events;
    struct jdksnet_stream_slots *m_server_stream;

    struct jdksnet_packet_signals m_client_packet_events;
    struct jdksnet_packet_slots *m_client_packet;

    /**
     * The Current State
     */
    enum jdksapc_state m_state;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.1
     */
    struct jdksapc_Addr m_addr;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.2
     */
    struct jdksavdecc_appdu m_apcMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.3
     */
    bool m_apcMsgOut;
    /**
     * See IEEE 1722.1 Annex C.5.3.1.4
     */
    struct jdksavdecc_appdu m_apsMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.5
     */
    bool m_apsMsgIn;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.6
     */
    jdksavdecc_timestamp_in_microseconds m_currentTime;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.7
     */
    bool m_finished;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.8
     */
    struct jdksavdecc_eui64 m_entityId;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.9
     */
    bool m_idAssigned;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.10
     */
    bool m_incomingTcpClosed;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.11
     */
    struct jdksavdecc_appdu m_linkMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.12
     */
    bool m_linkStatusMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.13
     */
    struct jdksavdecc_eui64 m_newId;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.14
     */
    bool m_nopTimeout;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.15
     */
    struct jdksavdecc_eui48 m_primaryMac;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.16
     */
    bool m_responseValid;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.17
     */
    bool m_tcpConnected;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.1
     */
    void ( *closeTcpConnection )( struct jdksapc *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.2
     */
    void ( *connectToProxy )( struct jdksapc *self,
                              struct jdksapc_Addr const *addr );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.3
     */
    bool ( *getHttpResponse )( struct jdksapc *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.4
     */
    void ( *initialize )( struct jdksapc *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.5
     */
    void ( *notifyLinkStatus )( struct jdksapc *self,
                                struct jdksavdecc_appdu const *linkMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.6
     */
    void ( *processMsg )( struct jdksapc *self,
                          struct jdksavdecc_appdu const *apsMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.7
     */
    void ( *sendIdRequest )( struct jdksapc *self,
                             struct jdksavdecc_eui48 primaryMac,
                             struct jdksavdecc_eui64 entity_id );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.8
     */
    void ( *sendHttpRequest )( struct jdksapc *self,
                               struct jdksapc_Addr const *addr );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.9
     */
    void ( *sendMsgToAps )( struct jdksapc *self,
                            struct jdksavdecc_appdu const *apcMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.10
     */
    void ( *sendNopToAps )( struct jdksapc *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.X
     */
    void ( *notifyNewEntityId )( struct jdksapc *self,
                                 struct jdksavdecc_eui64 entity_id );
};

void jdksapc_init( struct jdksapc *self,
                   struct jdksnet_stream_slots *server_stream,
                   struct jdksnet_packet_slots *client_packets );

void jdksapc_terminate( struct jdksapc *self );

void jdksapc_start( struct jdksapc *self,
                    struct jdksapc_Addr const *addr,
                    struct jdksavdecc_eui48 primaryMac,
                    struct jdksavdecc_eui64 preferred_entity_id );

void jdksapc_execute_state( struct jdksapc *self );

void jdksapc_finish( struct jdksapc *self );

void jdksapc_server_stream_readable( struct jdksnet_stream_signals *self,
                                     const struct jdksavdecc_frame *frame );

void jdksapc_server_stream_connected( struct jdksnet_stream_signals *self,
                                      const struct sockaddr *local_addr,
                                      socklen_t local_addr_len,
                                      const struct sockaddr *remote_addr,
                                      socklen_t remote_addr_len );

void jdksapc_server_stream_writable( struct jdksnet_stream_signals *self );

void jdksapc_server_stream_closed( struct jdksnet_stream_signals *self );

void jdksapc_server_stream_tick(
    struct jdksnet_stream_signals *self,
    jdksavdecc_timestamp_in_microseconds timestamp );

void jdksapc_client_packet_link_up( struct jdksnet_packet_signals *self,
                                    uint64_t bps,
                                    struct jdksavdecc_eui48 link_addr );

void jdksapc_client_packet_link_down( struct jdksnet_packet_signals *self,
                                      uint64_t bps,
                                      struct jdksavdecc_eui48 link_addr );

void jdksapc_client_packet_readable( struct jdksnet_packet_signals *self,
                                     const struct jdksavdecc_frame *frame );

void jdksapc_client_packet_writable( struct jdksnet_packet_signals *self );

void jdksapc_client_packet_closed( struct jdksnet_packet_signals *self );

void jdksapc_client_packet_tick(
    struct jdksnet_packet_signals *self,
    jdksavdecc_timestamp_in_microseconds timestamp );

void jdksapc_goto_state_BEGIN( struct jdksapc *self );

void jdksapc_execute_state_BEGIN( struct jdksapc *self );

void jdksapc_goto_state_INITIALIZE( struct jdksapc *self );

void jdksapc_execute_state_INITIALIZE( struct jdksapc *self );

void jdksapc_goto_state_WAIT_FOR_CONNECT( struct jdksapc *self );

void jdksapc_execute_state_WAIT_FOR_CONNECT( struct jdksapc *self );

void jdksapc_goto_state_ACCEPT( struct jdksapc *self );

void jdksapc_execute_state_ACCEPT( struct jdksapc *self );

void jdksapc_goto_state_START_TRANSFER( struct jdksapc *self );

void jdksapc_execute_state_START_TRANSFER( struct jdksapc *self );

void jdksapc_goto_state_WAITING( struct jdksapc *self );

void jdksapc_execute_state_WAITING( struct jdksapc *self );

void jdksapc_goto_state_CLOSED( struct jdksapc *self );

void jdksapc_execute_state_CLOSED( struct jdksapc *self );

void jdksapc_goto_state_LINK_STATUS( struct jdksapc *self );

void jdksapc_execute_state_LINK_STATUS( struct jdksapc *self );

void jdksapc_goto_state_RECEIVE_MSG( struct jdksapc *self );

void jdksapc_execute_state_RECEIVE_MSG( struct jdksapc *self );

void jdksapc_goto_state_SEND_MSG( struct jdksapc *self );

void jdksapc_execute_state_SEND_MSG( struct jdksapc *self );

void jdksapc_goto_state_ENTITY_ID_ASSIGNED( struct jdksapc *self );

void jdksapc_execute_state_ENTITY_ID_ASSIGNED( struct jdksapc *self );

void jdksapc_goto_state_SEND_NOP( struct jdksapc *self );

void jdksapc_execute_state_SEND_NOP( struct jdksapc *self );

void jdksapc_goto_state_FINISH( struct jdksapc *self );

void jdksapc_execute_state_FINISH( struct jdksapc *self );

void jdksapc_goto_state_END( struct jdksapc *self );

void jdksapc_execute_state_END( struct jdksapc *self );

bool jdksapc_send( struct jdksapc *self,
                   struct jdksavdecc_frame const *avdecc_frame );

void jdksapc_CloseTcpConnection( struct jdksapc *self );

void jdksapc_ConnectToProxy( struct jdksapc *self,
                             struct jdksapc_Addr const *addr );

bool jdksapc_GetHttpResponse( struct jdksapc *self );

void jdksapc_Initialize( struct jdksapc *self );

void jdksapc_NotifyLinkStatus( struct jdksapc *self,
                               struct jdksavdecc_appdu const *linkMsg );

void jdksapc_ProcessMsg( struct jdksapc *self,
                         struct jdksavdecc_appdu const *apsMsg );

void jdksapc_SendIdRequest( struct jdksapc *self,
                            struct jdksavdecc_eui48 primaryMac,
                            struct jdksavdecc_eui64 entity_id );

void jdksapc_SendHttpRequest( struct jdksapc *self,
                              struct jdksapc_Addr const *addr );

void jdksapc_SendMsgToAps( struct jdksapc *self,
                           struct jdksavdecc_appdu const *apcMsg );

void jdksapc_SendNopToAps( struct jdksapc *self );

void jdksapc_NotifyNewEntityId( struct jdksapc *self,
                                struct jdksavdecc_eui64 entity_id );
}
