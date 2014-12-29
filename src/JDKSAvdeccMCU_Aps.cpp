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
#include "JDKSAvdeccMCU_Aps.hpp"

namespace JDKSAvdeccMCU
{

ApsStateMachine::ApsStateMachine( ApsStateMachine::StateVariables *variables,
                                  ApsStateMachine::StateActions *actions,
                                  ApsStateMachine::StateEvents *events,
                                  ApsStateMachine::States *states )
    : m_variables( variables )
    , m_actions( actions )
    , m_events( events )
    , m_states( states )
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

ApsStateMachine::~ApsStateMachine()
{
    m_variables->setOwner( 0 );
    m_actions->setOwner( 0 );
    m_events->setOwner( 0 );
    m_states->setOwner( 0 );
}

bool ApsStateMachine::run() { return getStates()->run(); }

void ApsStateMachine::clear()
{
    m_variables->clear();
    m_actions->clear();
    m_events->clear();
    m_states->clear();
}

void ApsStateMachine::StateEvents::clear()
{
    m_in_http = true;
    m_http_parser->clear();
    m_app_parser.clear();
}

void ApsStateMachine::StateEvents::onIncomingTcpConnection()
{
    m_in_http = true;
    m_http_parser->clear();
}

ssize_t ApsStateMachine::StateEvents::onIncomingTcpData( const uint8_t *data,
                                                         ssize_t len )
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

ssize_t
    ApsStateMachine::StateEvents::onIncomingTcpHttpData( const uint8_t *data,
                                                         ssize_t len )
{
    return m_http_parser->onIncomingHttpData( data, len );
}

bool ApsStateMachine::StateEvents::onIncomingHttpRequest(
    const HttpRequest &request )
{
    bool r = false;
    if ( request.m_method == "CONNECT" && request.m_path == m_path
         && request.m_version == "HTTP/1.1" )
    {
        m_in_http = false;
        r = true;
    }
    return r;
}

ssize_t ApsStateMachine::StateEvents::onIncomingTcpAppData( const uint8_t *data,
                                                            ssize_t len )
{
    ssize_t r = 0;

    for ( ssize_t r = 0; r < len; ++r )
    {
        m_app_parser.parse( data[r] );
    }

    return r;
}

void ApsStateMachine::StateEvents::onTcpConnectionClosed() {}

void ApsStateMachine::StateEvents::onNetLinkStatusUpdated( Eui48 link_mac,
                                                           bool link_status )
{
}

void ApsStateMachine::StateEvents::onNetAvdeccMessageReceived(
    const Frame &frame )
{
}

void ApsStateMachine::StateEvents::onTimeTick( uint32_t time_in_seconds ) {}

void ApsStateMachine::StateEvents::onAppNop( const AppMessage &msg ) {}

void ApsStateMachine::StateEvents::onAppEntityIdRequest( const AppMessage &msg )
{
}

void
    ApsStateMachine::StateEvents::onAppEntityIdResponse( const AppMessage &msg )
{
}

void ApsStateMachine::StateEvents::onAppLinkUp( const AppMessage &msg ) {}

void ApsStateMachine::StateEvents::onAppLinkDown( const AppMessage &msg ) {}

void ApsStateMachine::StateEvents::onAppAvdeccFromAps( const AppMessage &msg )
{
}

void ApsStateMachine::StateEvents::onAppAvdeccFromApc( const AppMessage &msg )
{
}

void ApsStateMachine::StateEvents::onAppVendor( const AppMessage &msg ) {}
}
