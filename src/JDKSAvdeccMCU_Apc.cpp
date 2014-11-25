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

#if defined( TODO )
namespace JDKSAvdeccMCU
{

void ApcStateMachine::start() {}

void ApcStateMachine::finish() {}

void ApcStateMachine::executeState()
{
    switch ( m_state )
    {
    case StateBegin:
        executeStateBegin();
        break;
    case StateInitialize:
        executeStateInitialize();
        break;
    case StateWaitForConnect:
        executeStateWaitForConnect();
        break;
    case StateAccept:
        executeStateAccept();
        break;
    case StateStartTransfer:
        executeStateStartTransfer();
        break;
    case StateWaiting:
        executeStateWaiting();
        break;
    case StateClosed:
        executeStateClosed();
        break;
    case StateLinkStatus:
        executeStateLinkStatus();
        break;
    case StateReceiveMsg:
        executeStateReceiveMsg();
        break;
    case StateSendMsg:
        executeStateSendMsg();
        break;
    case StateEntityIdAssigned:
        executeStateEntityIdAssigned();
        break;
    case StateSendNop:
        executeStateSendNop();
        break;
    case StateFinish:
        executeStateFinish();
        break;
    case StateEnd:
        executeStateEnd();
        break;
    default:
        executeStateEnd();
        break;
    }
}

void ApcStateMachine::gotoStateBegin() { m_state = StateBegin; }

void ApcStateMachine::executeStateBegin() { gotoStateInitialize(); }

void ApcStateMachine::gotoStateInitialize()
{
    m_state = StateInitialize;
    initialize();
    connectToProxy( m_addr );
}

void ApcStateMachine::executeStateInitialize() { gotoStateWaitForConnect(); }

void ApcStateMachine::gotoStateWaitForConnect()
{
    m_state = StateWaitForConnect;
}

void ApcStateMachine::executeStateWaitForConnect()
{
    if ( m_tcpConnected )
    {
        gotoStateAccept();
    }
    else if ( m_finished )
    {
        gotoStateFinish();
    }
}

void ApcStateMachine::gotoStateAccept()
{
    m_state = StateAccept;
    sendHttpRequest( m_addr );
}

void ApcStateMachine::executeStateAccept()
{
    if ( getHttpResponse() )
    {
        gotoStateStartTransfer();
    }
    else
    {
        gotoStateClosed();
    }
}

void ApcStateMachine::gotoStateStartTransfer() {}

void ApcStateMachine::executeStateStartTransfer() {}

void ApcStateMachine::gotoStateWaiting() {}
}

#endif
