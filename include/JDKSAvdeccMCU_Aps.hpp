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
#include "JDKSAvdeccMCU_Eui.hpp"
#include "JDKSAvdeccMCU_AppMessage.hpp"
#include "JDKSAvdeccMCU_AppMessageParser.hpp"
#include "JDKSAvdeccMCU_Http.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief The ApsStateMachine class
///
/// See IEEE Std 1722.1-2013 Annex C
/// And IEEE 1722.1 Corregendum Annex C
///
class ApsStateMachine
{
  public:
    class StateVariables;
    class StateActions;
    class States;
    class StateEvents;

    ///
    /// \brief The StateVariables class
    /// See Annex C.5.2.1
    ///
    class StateVariables
    {
      public:
        StateVariables();
        virtual ~StateVariables();

        ///
        /// \brief setOwner set the owner
        /// \param owner ApsStateMachine
        ///
        virtual void setOwner( ApsStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief clear all variables
        ///
        /// Note, m_linkMac needs to be set
        ///
        virtual void clear();

        ///
        /// \brief m_owner
        ///
        /// The ApsStateMachine that owns these variables
        ///
        ApsStateMachine *m_owner;

        ///
        /// \brief m_finished See Annex C.5.2.1.5
        ///
        /// The finished variable is a Boolean that is set
        /// to TRUE when the APS is required to stop operation.
        ///
        bool m_finished;

        ///
        /// \brief m_tcpConnected See Annex C.5.2.1.15
        ///
        /// The tcpConnected variable is a Boolean that is set to TRUE
        /// when an APC connects to the APS.
        ///
        bool m_tcpConnected;

        ///
        /// \brief m_requestValid See Annex C.5.2.1.14 and Cor 1
        ///
        /// The requestValid variable is set to a value of -1 until
        /// a HTTP request is received when it is set to the appropriate
        /// HTTP response code for the HTTP request, as defined in
        /// RFC 2616 Section 6.1.1
        ///
        int m_requestValid;

        ///
        /// \brief m_incomingTcpClosed See Annex C.5.2.1.8
        ///
        /// The incomingTcpClosed variable is a Boolean that is
        /// set to TRUE when the incoming socket from the APC
        /// is closed.
        ///
        bool m_incomingTcpClosed;

        ///
        /// \brief m_linkMac See Corrigendum 1 Annex C
        ///
        /// The linkMac variable contains an EUI48 of the ethernet
        /// port on the AVDECC network
        ///
        Eui48 m_linkMac;

        ///
        /// \brief m_linkStatus See Annex C.5.2.1.9
        ///
        /// The linkStatus variable is a Boolean that is set to
        /// TRUE when the APS’s network port has an active link.
        ///
        bool m_linkStatus;

        ///
        /// \brief m_linkStatusChanged See Annex C.5.2.1.10
        ///
        /// The linkStatusChanged variable is a Boolean that is
        /// set to TRUE when the APS’s network port link status changed.
        ///
        bool m_linkStatusChanged;

        ///
        /// \brief m_apcMsg See Annex C.5.2.1.2
        ///
        /// The apcMsg variable is a Boolean that is set to TRUE
        /// if and only if the out variable contains an AVDECC PDU
        /// from the APC.
        ///
        bool m_apcMsg;

        ///
        /// \brief m_L2Msg See Annex C.5.2.1.11
        ///
        /// The L2Msg variable is a Boolean that is set to TRUE
        /// when the in variable contains an AVDECC PDU from the
        /// layer 2 network with the AVTPDU subtype field matching
        /// one of the subtypes listed in Table C.6.
        ///
        bool m_L2Msg;

        ///
        /// \brief m_assignIdRequest See Annex C.5.2.1.3
        ///
        /// The assignEntityIdRequest variable is a Boolean
        /// that is set to TRUE when the APS receives an
        /// ENTITY_ID_REQUEST message.
        ///
        bool m_assignEntityIdRequest;

        ///
        /// \brief m_a See Annex C.5.2.1.1
        ///
        /// The a variable is set to the address field of the message
        /// received from the APC.
        ///
        Eui48 m_a;

        ///
        /// \brief m_entity_id See Annex C.5.2.1.6
        ///
        /// The entity_id variable is set to the ID payload of the
        /// received ENTITY_ID_REQUEST message.
        ///
        Eui64 m_entity_id;

        ///
        /// \brief m_nopTimeout See Annex C.5.2.1.12
        ///
        /// The nopTimeout variable is an unsigned integer that
        /// represents the currentTime value when a NOP message is
        /// to be sent to the APC.
        ///
        uint32_t m_nopTimeout;

        ///
        /// \brief m_currentTime See Annex C.5.2.1.4
        ///
        /// The currentTime variable is an unsigned integer that
        /// increases by one every second.
        ///
        uint32_t m_currentTime;

        ///
        /// \brief m_out See Annex C.5.2.1.13
        ///
        /// The out variable contains the AVDECC PDU and destination
        /// address received from the APC in the AVDECC_FROM_APC message.
        ///
        AppMessage m_out;

        ///
        /// \brief m_in See Annex C.5.2.1.7
        ///
        /// The in variable contains the AVDECC PDU and source
        /// address received from a layer 2 network port.
        ///
        AppMessage m_in;
    };

    ///
    /// \brief The StateActions class
    ///
    /// See Annex C.5.2.2 (functions)
    ///
    class StateActions
    {
      public:
        StateActions() {}
        virtual ~StateActions() {}

        ///
        /// \brief setOwner Sets the owner
        /// \param owner ApsStateMachine
        ///
        virtual void setOwner( ApsStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief getOwner gets the owner
        /// \return ApsStateMachine
        ///
        ApsStateMachine *getOwner() { return m_owner; }

        ///
        /// \brief getVariables
        ///
        /// Get the state machine variables
        ///
        /// \return StateVariables
        ///
        StateVariables *getVariables() { return m_owner->getVariables(); }

        ///
        /// \brief getActions
        ///
        /// Ask the owning state machine for the actions object
        ///
        /// \return StateActions
        ///
        StateActions *getActions() { return m_owner->getActions(); }

        ///
        /// \brief getEvents
        ///
        /// Ask the owning state machine for the state events object
        ///
        /// \return
        ///
        StateEvents *getEvents() { return m_owner->getEvents(); }

        ///
        /// \brief clear any additional
        /// state
        ///
        virtual void clear() {}

        ///
        /// \brief initialize see Annex C.5.2.2.2
        ///
        /// The initialize() function sets the following:
        ///
        /// * apcMsg to FALSE
        /// * assignEntityIdRequest to FALSE
        /// * currentTime to zero (0)
        /// * finished to FALSE
        /// * L2Msg to FALSE
        /// * linkStatus to FALSE
        /// * nopTimeout to zero (0)
        /// * tcpConnected to FALSE
        ///
        virtual void initialize();

        ///
        /// \brief sendHttpResponse See Annex C.5.2.2.6
        ///
        /// The sendHttpResponse(httpCode) function forms and sends
        /// an HTTP response as defined in Section 6 of RFC 2616 to
        /// the APC with the httpCode as the HTTP Status-Code.
        ///
        /// \param requestValid HTTP Status-Code to send
        ///
        virtual void sendHttpResponse( int requestValid );

        ///
        /// \brief sendMsgToApc
        ///
        /// The sendMsgToApc function sends the apsMsg to the APC
        ///
        /// \param apsMsg
        ///
        virtual void sendMsgToApc( AppMessage const &apsMsg );

        ///
        /// \brief sendLinkStatus see Annex C.5.2.2.7
        ///
        /// The sendLinkStatus(linkStatus) function forms and sends
        /// a LINK_UP message to the APC if linkStatus is TRUE, or
        /// a LINK_DOWN message to the APC if linkStatus is FALSE.
        /// This function also sets the linkStatusChanged flag to FALSE.
        ///
        /// \param link_mac The MAC Address of ethernet port
        /// \param linkStatus true if link is up
        ///
        virtual void sendLinkStatus( Eui48 link_mac, bool linkStatus );

        ///
        /// \brief sendAvdeccToL2 See Annex C.5.2.2.4
        ///
        /// The sendAvdeccToL2(out) function takes the AVDECC PDU
        /// and destination address in the out parameter, validates it,
        /// and sends it to the layer 2 network port. This function
        /// also sets the apcMsg variable to FALSE.
        ///
        /// Validation limits the AVTPDUs that can be transferred to
        /// those subtypes listed in Table C.6. If the AVTPDU’s subtype
        /// field is not set to one of these values, this function sets
        /// the incomingTcpClosed variable to TRUE.
        ///
        /// \param msg AppMessage containing AVDECC msg to send to network
        ///
        virtual void sendAvdeccToL2( AppMessage const *msg );

        ///
        /// \brief sendAvdeccToApc See Annex C.5.2.2.3
        ///
        /// The sendAvdeccToApc(in) function forms the AVDECC_FROM_APS
        /// message with the address and payload from the in parameter
        /// and sends it over the TCP socket to the APC. This function
        /// also sets the L2Msg variable to FALSE.
        ///
        /// \param msg AppMessage containing AVDECC msg to send to APC
        ///
        virtual void sendAvdeccToApc( AppMessage const *msg );

        ///
        /// \brief sendEntityIdAssignment See Annex C.5.2.2.5
        ///
        /// The sendEntityIdAssignment() function validates the
        /// MAC-48 address in the a parameter and the entity_id
        /// parameter, which are from the ENTITY_ID_REQUEST message.
        /// If the entity_id is known to not be unique, then an
        /// appropriate Entity ID assignment is calculated.
        ///
        /// The a parameter is stored in the address field, and the
        /// appropriate assigned Entity ID is stored in the payload
        /// field of the ENTITY_ID_RESPONSE message.
        ///
        /// This function also sets the assignEntityIdRequest variable
        /// to FALSE.
        ///
        /// \param a Primary MAC Address of the APC
        /// \param entity_id Requested entity_id
        ///
        virtual void sendEntityIdAssignment( Eui48 a, Eui64 entity_id );

        ///
        /// \brief sendNopToApc See Annex C.5.2.2.8
        ///
        /// The sendNopToApc() function sends a NOP message to the APC.
        ///
        virtual void sendNopToApc();

        ///
        /// \brief closeTcpConnection see Annex C.5.2.2.1
        ///
        /// The closeTcpConnection() function closes the TCP connection with
        /// the APC and sets the incomingTcpClosed variable to FALSE.
        ///
        virtual void closeTcpConnection();

        ///
        /// \brief closeTcpServer
        ///
        /// Close the tcp server
        ///
        virtual void closeTcpServer();

      protected:
        ///
        /// \brief m_owner the owner
        ///
        ApsStateMachine *m_owner;
    };

    ///
    /// \brief The States class
    ///
    /// See Corrigendum 1, Figure C.2
    ///
    /// This class implements the state transition
    /// diagram in Figure C.2
    ///
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
        virtual void setOwner( ApsStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief getOwner
        /// get the owner
        ///
        /// \return ApsStateMachine
        ///
        ApsStateMachine *getOwner() { return m_owner; }

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
        /// Ask the owning state machine for the state variables object
        ///
        /// \return StateVariables
        ///
        StateVariables *getVariables() { return m_owner->getVariables(); }

        ///
        /// \brief getEvents
        ///
        /// Ask the owning state machine for the state events object
        ///
        /// \return
        ///
        StateEvents *getEvents() { return m_owner->getEvents(); }

        ///
        /// \brief clear
        ///
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
        /// See Figure C.2 'Begin'
        ///
        virtual void doBegin();

        ///
        /// \brief goToInitialize
        ///
        /// Transition to the INITIALIZE state
        ///
        /// Perform the functions listed in Figure C.2 'INITIALIZE'
        /// state
        ///
        virtual void goToInitialize();

        ///
        /// \brief doInitialize
        ///
        /// Implement the INITIALIZE state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'INITIALIZE' state
        ///
        virtual void doInitialize();

        ///
        /// \brief goToWaitForConnect
        ///
        /// Transition to the WAIT_FOR_CONNECT state
        ///
        /// Perform the functions listed in Figure C.2,
        /// 'WAIT_FOR_CONNECT' state
        ///
        virtual void goToWaitForConnect();

        ///
        /// \brief doWaitForConnect
        ///
        /// Implement the WAIT_FOR_CONNECT state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'WAIT_FOR_CONNECT' state
        ///
        virtual void doWaitForConnect();

        ///
        /// \brief goToAccept
        ///
        /// Transition to the ACCEPT state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'ACCEPT' state
        ///
        virtual void goToAccept();

        ///
        /// \brief doAccept
        ///
        /// Implement the ACCEPT state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'ACCEPT' state
        ///
        virtual void doAccept();

        ///
        /// \brief goToReject
        ///
        /// Transition to the REJECT state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'REJECT' state
        ///
        virtual void goToReject();

        ///
        /// \brief doReject
        ///
        /// Implement the REJECT state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'REJECT' state
        ///
        virtual void doReject();

        ///
        /// \brief goToClosed
        ///
        /// Transition to the CLOSED State
        ///
        /// Perform the functions listed in Figure C.2
        /// 'CLOSED' state
        ///
        virtual void goToClosed();

        ///
        /// \brief doClosed
        ///
        /// Implement the CLOSED state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'CLOSED' state
        ///
        virtual void doClosed();

        ///
        /// \brief goToStartTransfer
        ///
        /// Transition to the START_TRANSFER state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'START_TRANSFER' state
        ///
        virtual void goToStartTransfer();

        ///
        /// \brief doStartTransfer
        ///
        /// Implement the START_TRANSFER state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'START_TRANSFER' state
        ///
        virtual void doStartTransfer();

        ///
        /// \brief goToWaiting
        ///
        /// Transition to the WAITING state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'WAITING' state
        ///
        virtual void goToWaiting();

        ///
        /// \brief doWaiting
        ///
        /// Implement the WAITING state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'WAITING' state
        ///
        virtual void doWaiting();

        ///
        /// \brief goToLinkStatus
        ///
        /// Transition to the LINK_STATUS state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'LINK_STATUS' state
        ///
        virtual void goToLinkStatus();

        ///
        /// \brief doLinkStatus
        ///
        /// Implement the LINK_STATUS state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'LINK_STATUS' state
        ///
        virtual void doLinkStatus();

        ///
        /// \brief goToTransferToL2
        ///
        /// Transition to the TRANSFER_TO_L2 state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'TRANSFER_TO_L2' state
        ///
        virtual void goToTransferToL2();

        ///
        /// \brief doTransferToL2
        ///
        /// Implement the TRANSFER_TO_L2 state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'TRANSFER_TO_L2' state
        ///
        virtual void doTransferToL2();

        ///
        /// \brief goToTransferToApc
        ///
        /// Transition to TRANSFER_TO_APC state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'TRANSFER_TO_APC' state
        ///
        virtual void goToTransferToApc();

        ///
        /// \brief doTransferToApc
        ///
        /// Implement the TRANSFER_TO_APC state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'TRANSFER_TO_APC' state
        ///
        virtual void doTransferToApc();

        ///
        /// \brief goToAssignEntityId
        ///
        /// Transition to the ASSIGN_ENTITY_ID state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'ASSIGN_ENTITY_ID' state
        ///
        virtual void goToAssignEntityId();

        ///
        /// \brief doAssignEntityId
        ///
        /// Implement the ASSIGN_ENTITY_ID state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'ASSIGN_ENTITY_ID' state
        ///
        virtual void doAssignEntityId();

        ///
        /// \brief goToSendNop
        ///
        /// Transition to the SEND_NOP state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'SEND_NOP' state
        ///
        virtual void goToSendNop();

        ///
        /// \brief doSendNop
        ///
        /// Implement the SEND_NOP state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'SEND_NOP' state
        ///
        virtual void doSendNop();

        ///
        /// \brief goToCloseAndFinish
        ///
        /// Transition to the CLOSE_AND_FINISH state
        ///
        /// Perform the functions listed in Figure C.2
        /// 'CLOSE_AND_FINISH' state
        ///
        virtual void goToCloseAndFinish();

        ///
        /// \brief doCloseAndFinish
        ///
        /// Implement the CLOSE_AND_FINISH state
        ///
        /// Perform the tests for the paths leaving Figure
        /// C.2 'CLOSE_AND_FINISH' state
        ///
        virtual void doCloseAndFinish();

        ///
        /// \brief goToFinish
        ///
        /// Transition to the FINISH state
        ///
        /// Perform the functions listed in Figure C.2
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
        ApsStateMachine *m_owner;
        state_proc m_current_state;
    };

    class StateEvents : public AppMessageHandler, public HttpServerHandler
    {
      public:
        StateEvents( HttpServerParser *http_parser, std::string path );

        virtual ~StateEvents() {}

        ///
        /// \brief setOwner
        /// Set the owner of this state events object
        ///
        /// \param owner
        ///
        virtual void setOwner( ApsStateMachine *owner ) { m_owner = owner; }

        ///
        /// \brief getOwner
        ///
        /// get the owner
        ///
        /// \return ApsStateMachine
        ///
        ApsStateMachine *getOwner() { return m_owner; }

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
        /// \brief sendTcpData
        ///
        /// Send data to the TCP socket
        ///
        /// \param data
        /// \param len
        ///
        virtual void sendTcpData( uint8_t const *data, ssize_t len );

        ///
        /// \brief onIncomingTcpConnection
        ///
        /// Notify the state machine that an incoming tcp connection
        /// has happened
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
        /// from the APC
        ///
        /// \param data ptr to octets
        /// \param len lenth of data in octets
        /// \return length of consumed data
        ///
        virtual ssize_t onIncomingTcpData( uint8_t const *data, ssize_t len );

        ///
        /// \brief onNetLinkStatusUpdated
        ///
        /// Notify the state machine that the link status
        /// of the L2 network port has been updated
        ///
        /// \param link_mac MAC Address of link
        /// \param link_status true if link is up
        ///
        virtual void onNetLinkStatusUpdated( Eui48 link_mac, bool link_status );

        ///
        /// \brief onNetAvdeccMessageReceived
        ///
        /// Notify the state machine that an AVDECC message
        /// was received from the L2 network
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
        /// \brief onIncomingHttpRequest
        /// \param request
        /// \return
        ///
        virtual bool onIncomingHttpRequest( HttpRequest const &request );

        ///
        /// \brief onIncomingTcpAppData
        ///
        /// Notify the state machine that some data was received
        /// from the APC during the APP message section
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
        ApsStateMachine *m_owner;

        bool m_in_http;
        HttpServerParser *m_http_parser;
        std::string m_path;
        AppMessageParser m_app_parser;
    };

    StateVariables *getVariables() { return m_variables; }
    StateActions *getActions() { return m_actions; }
    StateEvents *getEvents() { return m_events; }
    States *getStates() { return m_states; }

    ApsStateMachine( StateVariables *variables,
                     StateActions *actions,
                     StateEvents *events,
                     States *states );

    virtual ~ApsStateMachine();

    virtual void start();

    virtual bool run();

    virtual void clear();

    virtual void closeTcpConnection() = 0;
    virtual void closeTcpServer() = 0;
    virtual void sendTcpData( uint8_t const *data, ssize_t len ) = 0;
    virtual void sendAvdeccToL2( uint8_t const *data, ssize_t len ) = 0;

  protected:
    StateVariables *m_variables;
    StateActions *m_actions;
    StateEvents *m_events;
    States *m_states;
};
}
