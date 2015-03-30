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
#include "JDKSAvdeccMCU/Apc.hpp"

namespace JDKSAvdeccMCU
{

ApcStateMachine::ApcStateMachine( ApcStateVariables *variables,
                                  ApcStateActions *actions,
                                  ApcStateEvents *events,
                                  ApcStates *states )
    : m_variables( variables ), m_actions( actions ), m_events( events ), m_states( states )
{
}

ApcStateMachine::~ApcStateMachine() {}

void ApcStateMachine::setup() { clear(); }

void ApcStateMachine::setPrimaryMac( Eui48 mac ) { getVariables()->m_primaryMac = mac; }

void ApcStateMachine::setEntityId( const Eui64 entity_id )
{
    getVariables()->m_newId = entity_id;
    getVariables()->m_entityId = entity_id;
}

Eui64 ApcStateMachine::getEntityId() const { return getVariables()->m_newId; }

void ApcStateMachine::setApsAddress( const std::string &addr ) { getVariables()->m_addr = addr; }

void ApcStateMachine::setPath( const std::string &path )
{
    std::vector<std::string> headers;
    getVariables()->m_request.setCONNECT( "/", headers );
}

bool ApcStateMachine::run() { return getStates()->run(); }

void ApcStateMachine::onTimeTick( uint32_t time_in_seconds ) { getEvents()->onTimeTick( time_in_seconds ); }

void ApcStateMachine::onTcpConnectionClosed() { return m_events->onTcpConnectionClosed(); }

void ApcStateMachine::closeTcpConnection() {}

void ApcStateMachine::connectToProxy( const std::string &addr ) {}

void ApcStateMachine::notifyProxyAvailable() {}

void ApcStateMachine::notifyProxyUnavailable() {}

void ApcStateMachine::notifyLinkStatus( const AppMessage &linkMsg ) {}

void ApcStateMachine::processMsg( const AppMessage &apsMsg ) {}

void ApcStateMachine::notifyNewEntityId( const Eui64 &entity_id ) {}

ssize_t ApcStateMachine::onIncomingTcpData( const uint8_t *data, ssize_t len ) { return m_events->onIncomingTcpData( data, len ); }

void ApcStateMachine::sendTcpData( const uint8_t *data, ssize_t len ) {}

void ApcStateMachine::onNetAvdeccMessageReceived( const Frame &frame ) { getEvents()->onNetAvdeccMessageReceived( frame ); }

void ApcStateMachine::clear()
{
    m_variables->setOwner( this );
    m_actions->setOwner( this );
    m_events->setOwner( this );
    m_states->setOwner( this );
    m_variables->clear();
    m_actions->clear();
    m_events->clear();
    m_states->clear();
}

void ApcStateEvents::clear()
{
    m_in_http = true;
    m_http_parser->clear();
    m_app_parser.clear();
}

void ApcStateEvents::onIncomingTcpConnection()
{
    m_in_http = true;
    m_http_parser->clear();
    getVariables()->m_tcpConnected = true;
}

void ApcStateEvents::onTcpConnectionClosed()
{
    clear();
    getVariables()->m_incomingTcpClosed = true;
}

ssize_t ApcStateEvents::onIncomingTcpData( const uint8_t *data, ssize_t len )
{
    ssize_t r = -1;
    if ( m_in_http )
    {
        r = onIncomingTcpHttpData( data, len );

        // If the http request was accepted
        if ( r >= 0 && r < len && m_in_http == false )
        {
            ssize_t r1 = onIncomingTcpAppData( data + r, len - r );
            if ( r1 > 0 )
            {
                r += r1;
            }
            else
            {
                r = -1;
            }
        }
    }
    else
    {
        r = onIncomingTcpAppData( data, len );
    }
    return r;
}

void ApcStateEvents::onNetAvdeccMessageReceived( const Frame &frame )
{
    getVariables()->m_apcMsg.clear();
    getVariables()->m_apcMsg.setAvdeccFromApc( frame );
    getVariables()->m_apcMsgOut = true;
}

void ApcStateEvents::onTimeTick( uint32_t time_in_seconds ) { getVariables()->m_currentTime = time_in_seconds; }

void ApcStateEvents::sendTcpData( const uint8_t *data, ssize_t len ) { getOwner()->sendTcpData( data, len ); }

ssize_t ApcStateEvents::onIncomingTcpHttpData( const uint8_t *data, ssize_t len )
{
    return m_http_parser->onIncomingHttpData( data, len );
}

bool ApcStateEvents::onIncomingHttpResponse( const HttpResponse &request )
{
    bool r = false;
    if ( request.m_status_code == "200" )
    {
        m_in_http = false;
        getVariables()->m_responseReceived = true;
        getVariables()->m_responseValid = true;
        r = true;
    }
    return r;
}

ssize_t ApcStateEvents::onIncomingTcpAppData( const uint8_t *data, ssize_t len )
{
    ssize_t r = 0;

    for ( ssize_t r = 0; r < len; ++r )
    {
        m_app_parser.parse( data[r] );
    }

    return r;
}

void ApcStateEvents::onAppNop( const AppMessage &msg )
{
    // Do nothing
}

void ApcStateEvents::onAppEntityIdRequest( const AppMessage &msg )
{
    // Do nothing
}

void ApcStateEvents::onAppEntityIdResponse( const AppMessage &msg )
{
    getVariables()->m_newId = msg.getEntityIdResponseEntityId();
    getVariables()->m_idAssigned = true;
}

void ApcStateEvents::onAppLinkUp( const AppMessage &msg )
{
    getVariables()->m_linkMsg = msg;
    getVariables()->m_linkStatusMsg = true;
}

void ApcStateEvents::onAppLinkDown( const AppMessage &msg )
{
    getVariables()->m_linkMsg = msg;
    getVariables()->m_linkStatusMsg = true;
}

void ApcStateEvents::onAppAvdeccFromAps( const AppMessage &msg )
{
    getVariables()->m_apsMsg = msg;
    getVariables()->m_apsMsgIn = true;
}

void ApcStateEvents::onAppAvdeccFromApc( const AppMessage &msg )
{
    // Do nothing
}

void ApcStateEvents::onAppVendor( const AppMessage &msg )
{
    // Do nothing
}

void ApcStateEvents::onAppUnknown( const AppMessage &msg )
{
    // Do nothing
}

void ApcStateActions::clear()
{
    // Do nothing
}

void ApcStateActions::closeTcpConnection() { getOwner()->closeTcpConnection(); }

void ApcStateActions::connectToProxy( const std::string &addr ) { getOwner()->connectToProxy( addr ); }

void ApcStateActions::notifyProxyAvailable() { getOwner()->notifyProxyAvailable(); }

void ApcStateActions::notifyProxyUnavailable() { getOwner()->notifyProxyUnavailable(); }

void ApcStateActions::notifyLinkStatus( const AppMessage &linkMsg ) { getOwner()->notifyLinkStatus( linkMsg ); }

void ApcStateActions::processMsg( const AppMessage &apsMsg ) { getOwner()->processMsg( apsMsg ); }

void ApcStateActions::notifyNewEntityId( const Eui64 &entity_id ) { getOwner()->notifyNewEntityId( entity_id ); }

void ApcStateActions::initialize()
{
    getVariables()->m_apcMsgOut = false;
    getVariables()->m_apsMsgIn = false;
    getVariables()->m_finished = false;
    getVariables()->m_idAssigned = false;
    getVariables()->m_incomingTcpClosed = false;
    getVariables()->m_linkStatusMsg = false;
    getVariables()->m_responseValid = false;
    getVariables()->m_responseReceived = false;
    getVariables()->m_tcpConnected = false;
}

void ApcStateActions::sendIdRequest( const Eui48 &primaryMac, const Eui64 &entity_id )
{
    AppMessage outmsg;
    getVariables()->m_primaryMac = primaryMac;
    getVariables()->m_entityId = entity_id;
    outmsg.setEntityIdRequest( primaryMac, entity_id );

    sendMsgToAps( outmsg );
}

void ApcStateActions::sendHttpRequest( const HttpRequest &request )
{
    std::string buf;
    request.flatten( &buf );
    getEvents()->sendTcpData( reinterpret_cast<const uint8_t *>( buf.data() ), buf.length() );
}

void ApcStateActions::sendMsgToAps( const AppMessage &apcMsg )
{
    FixedBufferWithSize<1500> msg_as_octets;
    if ( apcMsg.store( &msg_as_octets ) )
    {
        getEvents()->sendTcpData( msg_as_octets.getBuf(), msg_as_octets.getLength() );
    }
}

void ApcStateActions::sendNopToAps()
{
    AppMessage msg;
    msg.setNOP();
    sendMsgToAps( msg );
}

ApcStateVariables::~ApcStateVariables()
{
    // Do nothing
}

void ApcStateVariables::clear()
{
    m_addr.clear();
    m_request.clear();
    m_apcMsg.clear();
    m_apcMsgOut = false;
    m_apsMsg.clear();
    m_apsMsgIn = false;
    m_currentTime = 0;
    m_finished = false;
    m_entityId = Eui64();
    m_idAssigned = false;
    m_incomingTcpClosed = false;
    m_linkMsg.clear();
    m_linkStatusMsg = false;
    m_newId = Eui64();
    m_nopTimeout = false;
    m_primaryMac = Eui48();
    m_responseReceived = false;
    m_responseValid = false;
    m_tcpConnected = false;
}

void ApcStates::clear() { m_current_state = &ApcStates::doBegin; }

bool ApcStates::run()
{
    bool r = false;

    state_proc last_state;

    // call the current state function
    do
    {
        last_state = m_current_state;

        // only call state function if it is set
        if ( m_current_state )
        {
            ( this->*m_current_state )();
            r = true;
        }

        // if the current state transitioned, repeat
    } while ( last_state != m_current_state );

    // return true if there was activity,
    // return false if there is no state
    return r;
}

void ApcStates::doBegin() { goToInitialize(); }

void ApcStates::goToInitialize()
{
    m_current_state = &ApcStates::doInitialize;
    getActions()->initialize();
    getActions()->connectToProxy( getVariables()->m_addr );
}

void ApcStates::doInitialize() { goToWaitForConnect(); }

void ApcStates::goToWaitForConnect() { m_current_state = &ApcStates::doWaitForConnect; }

void ApcStates::doWaitForConnect()
{
    if ( getVariables()->m_finished )
    {
        goToFinish();
    }
    else if ( getVariables()->m_tcpConnected )
    {
        goToConnected();
    }
}

void ApcStates::goToConnected()
{
    m_current_state = &ApcStates::doConnected;

    getVariables()->m_responseValid = false;
    getVariables()->m_responseReceived = false;
    getActions()->sendHttpRequest( getVariables()->m_request );
}

void ApcStates::doConnected()
{
    if ( getVariables()->m_responseReceived && getVariables()->m_responseValid == false )
    {
        goToClosed();
    }
    else if ( getVariables()->m_responseReceived && getVariables()->m_responseValid == true )
    {
        goToStartTransfer();
    }
}

void ApcStates::goToStartTransfer()
{
    m_current_state = &ApcStates::doStartTransfer;
    getActions()->sendIdRequest( getVariables()->m_primaryMac, getVariables()->m_entityId );
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
}

void ApcStates::doStartTransfer() { goToWaiting(); }

void ApcStates::goToWaiting() { m_current_state = &ApcStates::doWaiting; }

void ApcStates::doWaiting()
{
    if ( getVariables()->m_incomingTcpClosed )
    {
        goToClosed();
    }
    else if ( getVariables()->m_finished )
    {
        goToClosed();
    }
    else if ( getVariables()->m_linkStatusMsg )
    {
        goToLinkStatus();
    }
    else if ( getVariables()->m_apsMsgIn )
    {
        goToReceiveMsg();
    }
    else if ( getVariables()->m_apcMsgOut )
    {
        goToSendMsg();
    }
    else if ( getVariables()->m_idAssigned )
    {
        goToEntityIdAssigned();
    }
    else if ( getVariables()->m_currentTime > getVariables()->m_nopTimeout )
    {
        goToSendNop();
    }
}

void ApcStates::goToClosed()
{
    m_current_state = &ApcStates::doClosed;
    getActions()->notifyProxyUnavailable();
}

void ApcStates::doClosed() { goToFinish(); }

void ApcStates::goToLinkStatus()
{
    m_current_state = &ApcStates::doLinkStatus;
    getActions()->notifyLinkStatus( getVariables()->m_linkMsg );
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
    getVariables()->m_linkStatusMsg = false;
}

void ApcStates::doLinkStatus() { goToWaiting(); }

void ApcStates::goToReceiveMsg()
{
    getActions()->processMsg( getVariables()->m_apsMsg );
    getVariables()->m_apsMsgIn = false;
}

void ApcStates::doReceiveMsg() { goToWaiting(); }

void ApcStates::goToSendMsg()
{
    getActions()->sendMsgToAps( getVariables()->m_apcMsg );
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
    getVariables()->m_apcMsgOut = false;
}

void ApcStates::doSendMsg() { goToWaiting(); }

void ApcStates::goToEntityIdAssigned()
{
    getVariables()->m_entityId = getVariables()->m_newId;
    getActions()->notifyNewEntityId( getVariables()->m_newId );
    getActions()->notifyProxyAvailable();
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
    getVariables()->m_idAssigned = false;
}

void ApcStates::doEntityIdAssigned() { goToWaiting(); }

void ApcStates::goToSendNop()
{
    getActions()->sendNopToAps();
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
}

void ApcStates::doSendNop() { goToWaiting(); }

void ApcStates::goToFinish()
{
    m_current_state = &ApcStates::doFinish;
    getActions()->closeTcpConnection();
}

void ApcStates::doFinish() { m_current_state = 0; }
}
