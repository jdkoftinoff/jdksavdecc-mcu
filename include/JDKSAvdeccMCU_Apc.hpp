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
#include "JDKSAvdeccMCU_Http.hpp"

namespace JDKSAvdeccMCU
{

class ApcStateMachine
{
  public:
    class StateVariables;
    class StateActions;
    class States;
    class StateEvents;

    class StateVariables
    {
      public:
        virtual ~StateVariables();

        ///
        /// \brief setOwner set the owner
        /// \param owner ApsStateMachine
        ///
        virtual void setOwner( ApcStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief clear all variables
        ///
        virtual void clear();

        ///
        /// \brief m_owner
        ///
        /// The ApsStateMachine that owns these variables
        ///
        ApcStateMachine *m_owner;

        /**
         * See IEEE 1722.1 Annex C.5.3.1.1
         */
        HttpRequest m_addr;

        /**
         * See IEEE 1722.1 Annex C.5.3.1.2
         */
        AppMessage m_apcMsg;

        /**
         * See IEEE 1722.1 Annex C.5.3.1.3
         */
        bool m_apcMsgOut;

        /**
         * See IEEE 1722.1 Annex C.5.3.1.4
         */
        AppMessage m_apsMsg;

        /**
         * See IEEE 1722.1 Annex C.5.3.1.5
         */
        bool m_apsMsgIn;

        /**
         * See IEEE 1722.1 Annex C.5.3.1.6
         */
        uint32_t m_currentTime;

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
        AppMessage m_linkMsg;

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
    };

    class StateActions
    {
      public:
        StateActions() {}
        virtual ~StateActions() {}

        ///
        /// \brief setOwner Sets the owner
        /// \param owner ApsStateMachine
        ///
        virtual void setOwner( ApcStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief getOwner gets the owner
        /// \return ApsStateMachine
        ///
        ApcStateMachine *getOwner() { return m_owner; }

        ///
        /// \brief getVariables
        ///
        /// Get the state machine variables
        ///
        /// \return StateVariables
        ///
        StateVariables *getVariables() { return m_owner->getVariables(); }

        ///
        /// \brief clear any additional
        /// state
        ///
        virtual void clear();

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.1
         */
        virtual void closeTcpConnection();

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.2
         */
        virtual void connectToProxy( HttpRequest const &addr );

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.3
         */
        virtual bool getHttpResponse();

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.4
         */
        virtual void initialize();

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.5
         */
        virtual void notifyLinkStatus( jdksavdecc_appdu const &linkMsg );

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.6
         */
        virtual void processMsg( jdksavdecc_appdu const &apsMsg );

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.7
         */
        virtual void sendIdRequest( Eui48 const &primaryMac,
                                    Eui64 const &entity_id );

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.8
         */
        virtual void sendHttpRequest( HttpRequest const &request );

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.9
         */
        virtual void sendMsgToAps( jdksavdecc_appdu const &apcMsg );

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.10
         */
        virtual void sendNopToAps();

        /**
         * See IEEE Std 1722.1 Annex C.5.3.2.X
         */
        virtual void notifyNewEntityId( Eui64 const &entity_id );

      protected:
        ApcStateMachine *m_owner;
    };

    class States
    {
      public:
        ///
        /// The state_proc member is a pointer to the
        /// current state handling procedure, or 0 if done
        ///
        typedef void ( States::*state_proc )();

        ///
        /// \brief States constructor
        ///
        /// Start the state machine in the begin state
        ///
        States() : m_owner( 0 ), m_current_state( &States::doBegin ) {}

        ///
        /// \brief ~States
        ///
        virtual ~States() {}

        ///
        /// \brief setOwner
        /// Set the owner of this state machine
        ///
        /// \param owner
        ///
        virtual void setOwner( ApcStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief getOwner
        /// get the owner
        ///
        /// \return ApsStateMachine
        ///
        ApcStateMachine *getOwner() { return m_owner; }

        ///
        /// \brief getActions
        ///
        /// Ask the owning state machine for the actions object
        ///
        /// \return StateActions
        ///
        StateActions *getActions() { return m_owner->getActions(); }

        ///
        /// \brief getVariables
        ///
        /// As the owning state machine for the state variables object
        ///
        /// \return StateVariables
        ///
        StateVariables *getVariables() { return m_owner->getVariables(); }

        ///
        /// \brief clear
        /// Clear all variables and initialize the initial state
        ///
        virtual void clear();

        ///
        /// \brief run Run the state machine
        ///
        /// \return false when the state machine is complete
        ///
        virtual bool run();

      protected:
        ///
        /// \brief doBegin
        ///
        /// Implement the BEGIN state
        ///
        /// See Figure C.3 'Begin'
        ///
        virtual void doBegin();

        ///
        /// \brief goToInitialize
        ///
        /// Transition to the INITIALIZE state
        ///
        /// Perform the functions listed in Figure C.3 'INITIALIZE'
        /// state
        ///
        virtual void goToInitialize();

        ///
        /// \brief doInitialize
        ///
        /// Implement the INITIALIZE state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'INITIALIZE' state
        ///
        virtual void doInitialize();

        ///
        /// \brief goToWaitForConnect
        ///
        /// Transition to the WAIT_FOR_CONNECT state
        ///
        /// Perform the functions listed in Figure C.3,
        /// 'WAIT_FOR_CONNECT' state
        ///
        virtual void goToWaitForConnect();

        ///
        /// \brief doWaitForConnect
        ///
        /// Implement the WAIT_FOR_CONNECT state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'WAIT_FOR_CONNECT' state
        ///
        virtual void doWaitForConnect();

        ///
        /// \brief goToConnected
        ///
        /// Transition to the CONNECTED state
        ///
        /// Perform the functions listed in Figure C.3
        /// 'CONNECTED' state
        ///
        virtual void goToConnected();

        ///
        /// \brief doConnected
        ///
        /// Implement the CONNECTED state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'CONNECTED' state
        ///
        virtual void doConnected();

        ///
        /// \brief goToStartTransfer
        ///
        /// Transition to the START_TRANSFER state
        ///
        /// Perform the functions listed in Figure C.3
        /// 'START_TRANSFER' state
        ///
        virtual void goToStartTransfer();

        ///
        /// \brief doStartTransfer
        ///
        /// Implement the START_TRANSFER state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'START_TRANSFER' state
        ///
        virtual void doStartTransfer();

        ///
        /// \brief goToWaiting
        ///
        /// Transition to the WAITING state
        ///
        /// Perform the functions listed in Figure C.3
        /// 'WAITING' state
        ///
        virtual void goToWaiting();

        ///
        /// \brief doWaiting
        ///
        /// Implement the WAITING state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'WAITING' state
        ///
        virtual void doWaiting();

        ///
        /// \brief goToClosed
        ///
        /// Transition to the CLOSED state
        ///
        /// Perform the functions listed in Figur C.3
        /// 'CLOSED' state
        ///
        virtual void goToClosed();

        ///
        /// \brief doClosed
        ///
        /// Implement the CLOSED state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'CLOSED" state
        ///
        virtual void doClosed();

        ///
        /// \brief goToLinkStatus
        ///
        /// Transition to the LINK_STATUS state
        ///
        /// Perform the functions listed in Figure C.3
        /// 'LINK_STATUS' state
        ///
        virtual void goToLinkStatus();

        ///
        /// \brief doLinkStatus
        ///
        /// Implement the LINK_STATUS state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'LINK_STATUS' state
        ///
        virtual void doLinkStatus();

        ///
        /// \brief goToReceiveMsg
        ///
        /// Transition to the RECEIVE_MSG state
        ///
        /// Peform the functions listed in Figure C.3
        /// 'RECEIVE_MSG' state
        ///
        virtual void goToReceiveMsg();

        ///
        /// \brief doReceiveMsg
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'RECEIVE_MSG' state
        ///
        virtual void doReceiveMsg();

        ///
        /// \brief goToSendMsg
        ///
        /// Transition to the SEND_MSG state
        ///
        /// Peform the functions listed in Figure C.3
        /// 'SEND_MSG' state
        ///
        virtual void goToSendMsg();

        ///
        /// \brief doSendMsg
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'SEND_MSG' state
        ///
        virtual void doSendMsg();

        ///
        /// \brief goToEntityIdAssigned
        ///
        /// Transition to the ENTITY_ID_ASSIGNED state
        ///
        /// Peform the functions listed in Figure C.3
        /// 'ENTITY_ID_ASSIGNED' state
        ///
        virtual void goToEntityIdAssigned();

        ///
        /// \brief doEntityIdAssigned
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'ENTITY_ID_ASSIGNED' state
        ///
        virtual void doEntityIdAssigned();

        ///
        /// \brief goToSendNop
        ///
        /// Transition to the SEND_NOP state
        ///
        /// Peform the functions listed in Figure C.3
        /// 'SEND_NOP' state
        ///
        virtual void goToSendNop();

        ///
        /// \brief doSendNop
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.3 'SEND_NOP' state
        ///
        virtual void doSendNop();

        ///
        /// \brief goToFinish
        ///
        /// Transition to the FINISH state
        ///
        /// Perform the functions listed in Figure C.3
        /// 'FINISH' state
        ///
        virtual void goToFinish();

        ///
        /// \brief doFinish
        ///
        /// Implement the FINISH state
        ///
        ///
        virtual void doFinish();

      protected:
        ApcStateMachine *m_owner;
        state_proc m_current_state;
    };

    class StateEvents : protected AppMessageHandler, protected HttpClientHandler
    {
      public:
        StateEvents( HttpClientParser *http_parser, std::string path )
            : m_owner( 0 )
            , m_http_parser( http_parser )
            , m_path( path )
            , m_app_parser( *this )
        {
        }

        virtual ~StateEvents() {}

        ///
        /// \brief setOwner
        /// Set the owner of this state events object
        ///
        /// \param owner
        ///
        virtual void setOwner( ApcStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief getOwner
        /// get the owner
        ///
        /// \return ApsStateMachine
        ///
        ApcStateMachine *getOwner() { return m_owner; }

        ///
        /// \brief getActions
        ///
        /// Ask the owning state machine for the actions object
        ///
        /// \return StateActions
        ///
        StateActions *getActions() { return m_owner->getActions(); }

        ///
        /// \brief getVariables
        ///
        /// As the owning state machine for the state variables object
        ///
        /// \return StateVariables
        ///
        StateVariables *getVariables() { return m_owner->getVariables(); }

        ///
        /// \brief clear
        ///
        /// Clear the events object
        ///
        virtual void clear();

        ///
        /// \brief onTcpConnection
        ///
        /// Notify the state machine that a tcp connection
        /// to the APS has completed
        ///
        virtual void onIncomingTcpConnection();

        ///
        /// \brief onTcpConnectionClosed
        ///
        /// Notify the state machine that the TCP connection
        /// was closed
        ///
        ///
        virtual void onTcpConnectionClosed();

        ///
        /// \brief onIncomingTcpData
        ///
        /// Notify the state machine that some data was received
        /// from the APS
        ///
        /// \param data ptr to octets
        /// \param len lenth of data in octets
        /// \return length of consumed data
        ///
        virtual ssize_t onIncomingTcpData( uint8_t const *data, ssize_t len );

        ///
        /// \brief onNetAvdeccMessageReceived
        ///
        /// Notify the state machine that an AVDECC message
        /// destined for the APS was received
        ///
        /// \param frame Ethernet frame
        ///
        virtual void onNetAvdeccMessageReceived( Frame const &frame );

        ///
        /// \brief onTimeTick
        ///
        /// Notify the state machine that some time has passed
        /// and the new time in seconds is updated
        ///
        /// \param time_in_seconds
        ///
        virtual void onTimeTick( uint32_t time_in_seconds );

      protected:
        ///
        /// \brief onIncomingTcpHttpData
        ///
        /// Notify the state machine that some data was received
        /// from the APC during the HTTP header section
        ///
        /// \param data ptr to octets
        /// \param len lenth of data in octets
        /// \return length of consumed data
        ///
        virtual ssize_t onIncomingTcpHttpData( uint8_t const *data,
                                               ssize_t len );

        ///
        /// \brief onIncomingHttpResponse
        /// \param request
        /// \return
        ///
        virtual bool onIncomingHttpResponse( HttpResponse const &request );

        ///
        /// \brief onIncomingTcpAppData
        ///
        /// Notify the state machine that some data was received
        /// from the APS during the APP message section
        ///
        /// \param data ptr to octets
        /// \param len lenth of data in octets
        /// \return length of consumed data
        ///
        virtual ssize_t onIncomingTcpAppData( uint8_t const *data,
                                              ssize_t len );

      protected:
        ///
        /// \brief onAppNop
        ///
        /// Received NOP from APC
        ///
        /// \param msg
        ///
        virtual void onAppNop( AppMessage const &msg );

        ///
        /// \brief onAppEntityIdRequest
        ///
        /// Received ENTITY_ID_REQUEST from APC
        ///
        /// \param msg
        ///
        virtual void onAppEntityIdRequest( AppMessage const &msg );

        ///
        /// \brief onAppEntityIdResponse
        ///
        /// Received ENTITY_ID_RESPONSE from APC
        ///
        /// \param msg
        ///
        virtual void onAppEntityIdResponse( AppMessage const &msg );

        ///
        /// \brief onAppLinkUp
        ///
        /// Received LINK_UP from APC
        ///
        /// \param msg
        ///
        virtual void onAppLinkUp( AppMessage const &msg );

        ///
        /// \brief onAppLinkDown
        ///
        /// Received LINK_DOWN from APC
        ///
        /// \param msg
        ///
        virtual void onAppLinkDown( AppMessage const &msg );

        ///
        /// \brief onAppAvdeccFromAps
        ///
        /// Received AVDECC_FROM_APS from APC
        ///
        /// \param msg
        ///
        virtual void onAppAvdeccFromAps( AppMessage const &msg );

        ///
        /// \brief onAppAvdeccFromApc
        ///
        /// Received AVDECC_FROM_APC from APC
        ///
        /// \param msg
        ///
        virtual void onAppAvdeccFromApc( AppMessage const &msg );

        ///
        /// \brief onAppVendor
        ///
        /// Received VENDOR from APC
        ///
        /// \param msg
        ///
        virtual void onAppVendor( AppMessage const &msg );

      protected:
        ApcStateMachine *m_owner;

        bool m_in_http;
        HttpClientParser *m_http_parser;
        std::string m_path;
        AppMessageParser m_app_parser;
    };

    StateVariables *getVariables() { return m_variables; }
    StateActions *getActions() { return m_actions; }
    StateEvents *getEvents() { return m_events; }
    States *getStates() { return m_states; }

    ApcStateMachine( StateVariables *variables,
                     StateActions *actions,
                     StateEvents *events,
                     States *states );

    virtual ~ApcStateMachine();

    virtual bool run();

    virtual void clear();

  protected:
    StateVariables *m_variables;
    StateActions *m_actions;
    StateEvents *m_events;
    States *m_states;
};
}
