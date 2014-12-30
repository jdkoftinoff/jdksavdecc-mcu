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

void ApcStateMachine::States::doBegin() {}

void ApcStateMachine::States::goToInitialize() {}

void ApcStateMachine::States::doInitialize() {}

void ApcStateMachine::States::goToWaitForConnect() {}

void ApcStateMachine::States::doWaitForConnect() {}

void ApcStateMachine::States::goToConnected() {}

void ApcStateMachine::States::doConnected() {}

void ApcStateMachine::States::goToStartTransfer() {}

void ApcStateMachine::States::doStartTransfer() {}

void ApcStateMachine::States::goToWaiting() {}

void ApcStateMachine::States::doWaiting() {}

void ApcStateMachine::States::goToClosed() {}

void ApcStateMachine::States::doClosed() {}

void ApcStateMachine::States::goToLinkStatus() {}

void ApcStateMachine::States::doLinkStatus() {}

void ApcStateMachine::States::goToReceiveMsg() {}

void ApcStateMachine::States::doReceiveMsg() {}

void ApcStateMachine::States::goToSendMsg() {}

void ApcStateMachine::States::doSendMsg() {}

void ApcStateMachine::States::goToEntityIdAssigned() {}

void ApcStateMachine::States::doEntityIdAssigned() {}

void ApcStateMachine::States::goToSendNop() {}

void ApcStateMachine::States::doSendNop() {}

void ApcStateMachine::States::goToFinish() {}

void ApcStateMachine::States::doFinish() {}

void ApcStateMachine::StateActions::closeTcpConnection() {}

void ApcStateMachine::StateActions::connectToProxy( const HttpRequest &addr ) {}

bool ApcStateMachine::StateActions::getHttpResponse() {}

void ApcStateMachine::StateActions::initialize() {}

void ApcStateMachine::StateActions::notifyLinkStatus(
    const jdksavdecc_appdu &linkMsg )
{
}

void ApcStateMachine::StateActions::processMsg( const jdksavdecc_appdu &apsMsg )
{
}

void ApcStateMachine::StateActions::sendIdRequest( const Eui48 &primaryMac,
                                                   const Eui64 &entity_id )
{
}

void
    ApcStateMachine::StateActions::sendHttpRequest( const HttpRequest &request )
{
}

void ApcStateMachine::StateActions::sendMsgToAps(
    const jdksavdecc_appdu &apcMsg )
{
}

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
