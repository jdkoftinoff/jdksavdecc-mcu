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
#include "JDKSAvdeccMCU_AppMessage.hpp"
#include "JDKSAvdeccMCU_AppMessageParser.hpp"
#include "JDKSAvdeccMCU_AppMessageHandler.hpp"

#if defined( TODO )
namespace JDKSAvdeccMCU
{

class RawSocketSignals
{
  public:
    virtual ~RawSocketSignals() = 0;

    /**
     * External Networking Event: The network port obtained link
     */
    virtual void linkUp( uint64_t bps, const Eui48 &link_addr ) = 0;

    /**
     * External Networking Event: The network port lost link
     */
    virtual void linkDown( uint64_t bps, const Eui48 &link_addr ) = 0;

    /**
     * External Networking Event: The socket was readable and some data was read
     */
    virtual void readable( const Frame &frame ) = 0;

    /**
     * External Networking Event: The socket is writable now
     */
    virtual void writable() = 0;

    /**
     * External Networking Event: The socket was closed
     */
    virtual void closed() = 0;

    /**
     * External Networking Event: Some time passed
     */
    virtual void tick( jdksavdecc_timestamp_in_milliseconds timestamp ) = 0;
};

class RawSocketSlots
{
  public:
    virtual ~RawSocketSlots() = 0;

    /**
     * External Networking Request: The client object wants to wake up when the
     * socket is writable
     */
    virtual void wakeOnWritable( bool enable );

    /**
     * External Networking Request: The client object wants to send a frame
     */
    virtual ssize_t send( const Frame &frame );

    /**
     * External Networking Request: The client object wants to be woken up in
     * the future
     */
    virtual void wakeUpIn( uint32_t delta_time_in_milliseconds );
};

/** Collection of function pointers that will be called when a networking event
 * occurs
 */
class ApcTcp
{
  public:
    /**
     * External Networking Event: The socket was readable and some data was read
     */
    virtual void readable( const Frame &frame ) = 0;

    /**
     * External Networking Event: The socket was connected
     */
    virtual void connected( const sockaddr *local_addr,
                            socklen_t local_addr_len,
                            const sockaddr *remote_addr,
                            socklen_t remote_addr_len ) = 0;

    /**
     * External Networking Event: The socket is writable now
     */
    virtual void writable() = 0;

    /**
     * External Networking Event: The socket was closed
     */
    virtual void closed() = 0;

    /**
     * External Networking Event: Some time passed
     */
    virtual void tick( jdksavdecc_timestamp_in_milliseconds timestamp ) = 0;

    /**
     * External Networking Request: The client object wants to wake up when the
     * socket is writable
     */
    virtual void wakeOnWritable( bool enable ) = 0;

    /**
     * External Networking Request: The client object wants to connect to a
     * destination
     */
    virtual void connect( sockaddr const *addr, socklen_t addr_len ) = 0;

    /**
     * External Networking Request: The client object wants to close the socket
     */
    virtual void close() = 0;

    /**
     * External Networking Request: The client object wants to send a frame
     */
    virtual ssize_t send( const Frame &frame ) = 0;

    /**
     * External Networking Request: The client object wants to be woken up in
     * the future
     */
    virtual void wakeUpIn( uint32_t delta_time_in_milliseconds ) = 0;
};

struct ApcAddr
{
    struct sockaddr *m_addr;
    ssize_t m_addr_len;
    char m_path[4096];
    char m_host[1024];
    char m_port[128];
    char *m_additional_request_headers;
};

class ApcActions : public ApcTcp
{
  public:
    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.1
     */
    virtual void closeTcpConnection() = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.2
     */
    virtual void connectToProxy( ApcAddr const &addr ) = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.3
     */
    virtual bool getHttpResponse() = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.4
     */
    virtual void initialize() = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.5
     */
    virtual void notifyLinkStatus( jdksavdecc_appdu const &linkMsg ) = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.6
     */
    virtual void processMsg( jdksavdecc_appdu const &apsMsg ) = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.7
     */
    virtual void sendIdRequest( Eui48 const &primaryMac,
                                Eui64 const &entity_id ) = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.8
     */
    virtual void sendHttpRequest( ApcAddr const &addr ) = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.9
     */
    virtual void sendMsgToAps( jdksavdecc_appdu const &apcMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.10
     */
    virtual void sendNopToAps() = 0;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.X
     */
    virtual void notifyNewEntityId( Eui64 const &entity_id );
};

class ApcStateMachine : public ApcActions
{
  public:
    /**
     * @brief The Apc State enum
     *
     * See IEEE Std 1722.1 Annex C.5.3.3, Figure C.3
     *
     */
    enum State
    {
        StateBegin = 0,
        StateInitialize,
        StateWaitForConnect,
        StateAccept,
        StateStartTransfer,
        StateWaiting,
        StateClosed,
        StateLinkStatus,
        StateReceiveMsg,
        StateSendMsg,
        StateEntityIdAssigned,
        StateSendNop,
        StateFinish,
        StateEnd
    };

    /**
     * The Current State
     */
    State m_state;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.1
     */
    ApcAddr m_addr;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.2
     */
    jdksavdecc_appdu m_apcMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.3
     */
    bool m_apcMsgOut;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.4
     */
    jdksavdecc_appdu m_apsMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.5
     */
    bool m_apsMsgIn;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.6
     */
    jdksavdecc_timestamp_in_milliseconds m_currentTime;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.7
     */
    bool m_finished;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.8
     */
    Eui64 m_entityId;

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
    jdksavdecc_appdu m_linkMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.12
     */
    bool m_linkStatusMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.13
     */
    Eui64 m_newId;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.14
     */
    bool m_nopTimeout;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.15
     */
    Eui48 m_primaryMac;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.16
     */
    bool m_responseValid;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.17
     */
    bool m_tcpConnected;

    void start();

    void finish();

    void executeState();

  private:
    void gotoStateBegin();

    void executeStateBegin();

    void gotoStateInitialize();

    void executeStateInitialize();

    void gotoStateWaitForConnect();

    void executeStateWaitForConnect();

    void gotoStateAccept();

    void executeStateAccept();

    void gotoStateStartTransfer();

    void executeStateStartTransfer();

    void gotoStateWaiting();

    void executeStateWaiting();

    void gotoStateClosed();

    void executeStateClosed();

    void gotoStateLinkStatus();

    void executeStateLinkStatus();

    void gotoStateReceiveMsg();

    void executeStateReceiveMsg();

    void gotoStateSendMsg();

    void executeStateSendMsg();

    void gotoStateEntityIdAssigned();

    void executeStateEntityIdAssigned();

    void gotoStateSendNop();

    void executeStateSendNop();

    void gotoStateFinish();

    void executeStateFinish();

    void gotoStateEnd();

    void executeStateEnd();
};
}
#endif
