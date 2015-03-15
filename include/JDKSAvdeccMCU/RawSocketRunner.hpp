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
#include "JDKSAvdeccMCU/Frame.hpp"
#include "JDKSAvdeccMCU/RawSocket.hpp"
#include "JDKSAvdeccMCU/HandlerGroup.hpp"

namespace JDKSAvdeccMCU
{

class RawSocketRunnerNotification
{
  public:
    virtual ~RawSocketRunnerNotification() {}

    virtual bool rawSocketFound( RawSocket *rawsocket ) = 0;
    virtual void rawSocketRemoved( RawSocket *rawsocket ) = 0;
    virtual void frameReceived( RawSocket *rawsocket, Frame &incoming_frame )
        = 0;
};

class RawSocketRunner
{
  public:
    RawSocketRunner() {}
    virtual ~RawSocketRunner() {}
    virtual void setTarget( RawSocketRunnerNotification *notification_target ) = 0;
    virtual bool run( int timeout_ms ) = 0;
};

class SimpleRawSocketRunner : public RawSocketRunner
{
  public:
    SimpleRawSocketRunner( RawSocket *the_socket ) : m_the_socket( the_socket ), m_notification_target( 0 ) {}

    virtual ~SimpleRawSocketRunner()
    {
        if ( m_notification_target && m_the_socket )
        {
            m_notification_target->rawSocketRemoved( m_the_socket );
        }
    }

    virtual void setTarget( RawSocketRunnerNotification *notification_target )
    {
        m_notification_target = notification_target;
        if ( m_notification_target )
        {
            if ( !m_notification_target->rawSocketFound( m_the_socket ) )
            {
                m_the_socket = 0;
            }
        }
    }

    virtual bool run( int timeout_ms )
    {
        // TODO: poll socket and dispatch
        return false;
    }

  private:
    RawSocket *m_the_socket;
    RawSocketRunnerNotification *m_notification_target;
};
}
