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
#include "JDKSAvdeccMCU_Apc.hpp"

namespace JDKSAvdeccMCU
{

void ApcStateMachine::StateVariables::clear()
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

bool ApcStateMachine::States::run()
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

void ApcStateMachine::States::doBegin() { goToInitialize(); }

void ApcStateMachine::States::goToInitialize()
{
    m_current_state = &States::doInitialize;
    getActions()->initialize();
    getActions()->connectToProxy( getVariables()->m_addr );
}

void ApcStateMachine::States::doInitialize() { goToWaitForConnect(); }

void ApcStateMachine::States::goToWaitForConnect()
{
    m_current_state = &States::doWaitForConnect;
}

void ApcStateMachine::States::doWaitForConnect()
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

void ApcStateMachine::States::goToConnected()
{
    m_current_state = &States::doConnected;

    getVariables()->m_responseValid = false;
    getVariables()->m_responseReceived = false;
    getActions()->sendHttpRequest( getVariables()->m_request );
}

void ApcStateMachine::States::doConnected()
{
    if ( getVariables()->m_responseReceived
         && getVariables()->m_responseValid == false )
    {
        goToClosed();
    }
    else if ( getVariables()->m_responseReceived
              && getVariables()->m_responseValid == true )
    {
        goToStartTransfer();
    }
}

void ApcStateMachine::States::goToStartTransfer()
{
    m_current_state = &States::doStartTransfer;
    getActions()->sendIdRequest( getVariables()->m_primaryMac,
                                 getVariables()->m_entityId );
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
}

void ApcStateMachine::States::doStartTransfer() { goToWaiting(); }

void ApcStateMachine::States::goToWaiting()
{
    m_current_state = &States::doWaiting;
}

void ApcStateMachine::States::doWaiting()
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

void ApcStateMachine::States::goToClosed()
{
    m_current_state = &States::doClosed;
    getActions()->notifyProxyUnavailable();
}

void ApcStateMachine::States::doClosed() { goToFinish(); }

void ApcStateMachine::States::goToLinkStatus()
{
    m_current_state = &States::doLinkStatus;
    getActions()->notifyLinkStatus( getVariables()->m_linkMsg );
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
    getVariables()->m_linkStatusMsg = false;
}

void ApcStateMachine::States::doLinkStatus() { goToWaiting(); }

void ApcStateMachine::States::goToReceiveMsg()
{
    getActions()->processMsg( getVariables()->m_apsMsg );
    getVariables()->m_apsMsgIn = false;
}

void ApcStateMachine::States::doReceiveMsg() { goToWaiting(); }

void ApcStateMachine::States::goToSendMsg()
{
    getActions()->sendMsgToAps( getVariables()->m_apcMsg );
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
    getVariables()->m_apcMsgOut = false;
}

void ApcStateMachine::States::doSendMsg() { goToWaiting(); }

void ApcStateMachine::States::goToEntityIdAssigned()
{
    getVariables()->m_entityId = getVariables()->m_newId;
    getActions()->notifyNewEntityId( getVariables()->m_newId );
    getActions()->notifyProxyAvailable();
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
    getVariables()->m_idAssigned = false;
}

void ApcStateMachine::States::doEntityIdAssigned() { goToWaiting(); }

void ApcStateMachine::States::goToSendNop()
{
    getActions()->sendNopToAps();
    getVariables()->m_nopTimeout = getVariables()->m_currentTime + 10;
}

void ApcStateMachine::States::doSendNop() { goToWaiting(); }

void ApcStateMachine::States::goToFinish()
{
    m_current_state = &States::doFinish;
    getActions()->closeTcpConnection();
}

void ApcStateMachine::States::doFinish() { m_current_state = 0; }

void ApcStateMachine::StateActions::closeTcpConnection() {}

void ApcStateMachine::StateActions::connectToProxy( const std::string &addr ) {}

bool ApcStateMachine::StateActions::getHttpResponse() {}

void ApcStateMachine::StateActions::initialize() {}

void
    ApcStateMachine::StateActions::notifyLinkStatus( const AppMessage &linkMsg )
{
}

void ApcStateMachine::StateActions::processMsg( const AppMessage &apsMsg ) {}

void ApcStateMachine::StateActions::sendIdRequest( const Eui48 &primaryMac,
                                                   const Eui64 &entity_id )
{
}

void
    ApcStateMachine::StateActions::sendHttpRequest( const HttpRequest &request )
{
}

void ApcStateMachine::StateActions::sendMsgToAps( const AppMessage &apcMsg ) {}

void ApcStateMachine::StateActions::sendNopToAps() {}

void ApcStateMachine::StateActions::notifyNewEntityId( const Eui64 &entity_id )
{
}

void ApcStateMachine::StateEvents::clear() {}

void ApcStateMachine::StateEvents::onIncomingTcpConnection() {}

void ApcStateMachine::StateEvents::onTcpConnectionClosed() {}

ssize_t ApcStateMachine::StateEvents::onIncomingTcpData( const uint8_t *data,
                                                         ssize_t len )
{
}

void ApcStateMachine::StateEvents::onNetAvdeccMessageReceived(
    const Frame &frame )
{
}

void ApcStateMachine::StateEvents::onTimeTick( uint32_t time_in_seconds ) {}

ssize_t
    ApcStateMachine::StateEvents::onIncomingTcpHttpData( const uint8_t *data,
                                                         ssize_t len )
{
}

bool ApcStateMachine::StateEvents::onIncomingHttpResponse(
    const HttpResponse &request )
{
}

ssize_t ApcStateMachine::StateEvents::onIncomingTcpAppData( const uint8_t *data,
                                                            ssize_t len )
{
}

void ApcStateMachine::StateEvents::onAppNop( const AppMessage &msg ) {}

void ApcStateMachine::StateEvents::onAppEntityIdRequest( const AppMessage &msg )
{
}

void
    ApcStateMachine::StateEvents::onAppEntityIdResponse( const AppMessage &msg )
{
}

void ApcStateMachine::StateEvents::onAppLinkUp( const AppMessage &msg ) {}

void ApcStateMachine::StateEvents::onAppLinkDown( const AppMessage &msg ) {}

void ApcStateMachine::StateEvents::onAppAvdeccFromAps( const AppMessage &msg )
{
}

void ApcStateMachine::StateEvents::onAppAvdeccFromApc( const AppMessage &msg )
{
}

void ApcStateMachine::StateEvents::onAppVendor( const AppMessage &msg ) {}

ApcStateMachine::ApcStateMachine( ApcStateMachine::StateVariables *variables,
                                  ApcStateMachine::StateActions *actions,
                                  ApcStateMachine::StateEvents *events,
                                  ApcStateMachine::States *states )
    : m_variables( variables )
    , m_actions( actions )
    , m_events( events )
    , m_states( states )
{
    m_variables->setOwner( this );
    m_actions->setOwner( this );
    m_events->setOwner( this );
    m_states->setOwner( this );
}

ApcStateMachine::~ApcStateMachine()
{
    m_variables->setOwner( 0 );
    m_actions->setOwner( 0 );
    m_events->setOwner( 0 );
    m_states->setOwner( 0 );
}

bool ApcStateMachine::run() { return getStates()->run(); }

void ApcStateMachine::clear()
{
    m_variables->clear();
    m_actions->clear();
    m_events->clear();
    m_states->clear();
}
}
