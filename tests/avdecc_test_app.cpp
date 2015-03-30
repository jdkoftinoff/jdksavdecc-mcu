#include "JDKSAvdeccMCU.hpp"

using namespace JDKSAvdeccMCU;

void dump( std::ostream &ostr, uint8_t const *buf, uint16_t len )
{
    ostr << "Length: " << std::dec << len << std::endl;
    for ( ssize_t i = 0; i < len; ++i )
    {
        ostr << std::hex << ( uint16_t )( buf[i] ) << " ";
    }
    ostr << std::dec << std::endl;
}

void dump( std::ostream &ostr, AppMessage const &msg )
{
    ostr << "AppMessage: ";
    switch ( msg.getMessageType() )
    {
    case AppMessage::NOP:
        ostr << "NOP" << std::endl;
        break;
    case AppMessage::AVDECC_FROM_APC:
        ostr << "AVDECC_FROM_APC" << std::endl;
        ostr << "Address: " << std::hex << msg.getAddress().convertToUint64() << std::endl;
        ostr << std::dec;
        dump( ostr, msg.getPayload(), msg.getPayloadLength() );
        break;
    case AppMessage::AVDECC_FROM_APS:
        ostr << "AVDECC_FROM_APS" << std::endl;
        ostr << "Address: " << std::hex << msg.getAddress().convertToUint64() << std::endl;
        ostr << std::dec;
        dump( ostr, msg.getPayload(), msg.getPayloadLength() );
        break;
    case AppMessage::ENTITY_ID_REQUEST:
        ostr << "ENTITY_ID_REQUEST" << std::endl;
        ostr << "Address: " << std::hex << msg.getAddress().convertToUint64() << std::endl;
        ostr << "EntityID: " << std::hex << msg.getEntityIdRequestEntityId().convertToUint64() << std::endl;
        ostr << std::dec;
        break;
    case AppMessage::ENTITY_ID_RESPONSE:
        ostr << "ENTITY_ID_RESPONSE" << std::endl;
        ostr << "Address: " << std::hex << msg.getAddress().convertToUint64() << std::endl;
        ostr << "EntityID: " << std::hex << msg.getEntityIdResponseEntityId().convertToUint64() << std::endl;
        ostr << std::dec;
        break;
    case AppMessage::LINK_UP:
        ostr << "LINK_UP" << std::endl;
        ostr << "Address: " << std::hex << msg.getAddress().convertToUint64() << std::endl;
        break;
    case AppMessage::LINK_DOWN:
        ostr << "LINK_DOWN" << std::endl;
        ostr << "Address: " << std::hex << msg.getAddress().convertToUint64() << std::endl;
        break;
    case AppMessage::VENDOR:
        ostr << "VENDOR" << std::endl;
        ostr << "vendor message: " << std::hex << msg.getAddress().convertToUint64() << std::endl;
        break;
    }
}

class TestApcStateMachine;
class TestApsStateMachine;

class TestApcStateMachine : public ApcStateMachine
{
  public:
    class TestStates : public States
    {
      protected:
        virtual void goToInitialize()
        {
            std::cout << "APC: INITIALIZE" << std::endl;
            States::goToInitialize();
        }

        virtual void goToWaitForConnect()
        {
            std::cout << "APC: WAIT_FOR_CONNECT" << std::endl;
            States::goToWaitForConnect();
        }

        virtual void goToConnected()
        {
            std::cout << "APC: CONNECTED" << std::endl;
            States::goToConnected();
        }

        virtual void goToStartTransfer()
        {
            std::cout << "APC: START_TRANSFER" << std::endl;
            States::goToStartTransfer();
        }

        virtual void goToWaiting()
        {
            std::cout << "APC: WAITING" << std::endl;
            States::goToWaiting();
        }

        virtual void goToClosed()
        {
            std::cout << "APC: CLOSED" << std::endl;
            States::goToClosed();
        }

        virtual void goToLinkStatus()
        {
            std::cout << "APC: LINK_STATUS" << std::endl;
            States::goToLinkStatus();
        }

        virtual void goToReceiveMsg()
        {
            std::cout << "APC: RECEIVE_MSG" << std::endl;
            States::goToReceiveMsg();
        }

        virtual void goToSendMsg()
        {
            std::cout << "APC: SEND_MSG" << std::endl;
            States::goToSendMsg();
        }

        virtual void goToEntityIdAssigned()
        {
            std::cout << "APC: ENTITY_ID_ASSIGNED" << std::endl;
            States::goToEntityIdAssigned();
        }

        virtual void goToSendNop()
        {
            std::cout << "APC: SEND_NOP" << std::endl;
            States::goToSendNop();
        }

        virtual void goToFinish()
        {
            std::cout << "APC: FINISH" << std::endl;
            States::goToFinish();
        }
    };

    TestApcStateMachine()
        : ApcStateMachine( &m_test_variables, &m_test_actions, &m_test_events, &m_test_states )
        , m_test_events( &m_http_parser_simple, "/" )
        , m_http_parser_simple( &m_http_response, &m_test_events )
    {
    }

    virtual void closeTcpConnection()
    {
        std::cout << "APC: closeTCPConnection()" << std::endl;
        ApcStateMachine::closeTcpConnection();
        m_server->onTcpConnectionClosed();
    }

    virtual void connectToProxy( std::string const &addr )
    {
        std::cout << "APC: connectToProxy( " << addr << " )" << std::endl;
        ApcStateMachine::connectToProxy( addr );
        getEvents()->onIncomingTcpConnection();
    }

    virtual void notifyProxyAvailable()
    {
        std::cout << "APC: notifyProxyAvailable()" << std::endl;
        ApcStateMachine::notifyProxyAvailable();
    }

    virtual void notifyProxyUnavailable()
    {
        std::cout << "APC: notifyProxyUnavailable()" << std::endl;
        ApcStateMachine::notifyProxyUnavailable();
    }

    virtual void notifyLinkStatus( AppMessage const &linkMsg )
    {
        std::cout << "APC: notifyLinkStatus( " << std::hex << linkMsg.getAddress().convertToUint64() << " "
                  << ( linkMsg.getMessageType() == AppMessage::LINK_UP ? 1 : 0 ) << " )" << std::endl;
        ApcStateMachine::notifyLinkStatus( linkMsg );
    }

    virtual void processMsg( AppMessage const &apsMsg )
    {
        std::cout << "APC: processMsg( " << apsMsg.getMessageType() << " )" << std::endl;
        dump( std::cout, apsMsg );
        ApcStateMachine::processMsg( apsMsg );
    }

    virtual void notifyNewEntityId( Eui64 const &entity_id )
    {
        std::cout << "APC: notifyNewEntityId( " << std::hex << entity_id.convertToUint64() << " )" << std::endl;
        ApcStateMachine::notifyNewEntityId( entity_id );
    }

    virtual void sendTcpData( uint8_t const *data, ssize_t len )
    {
        std::cout << "APC: sendTcpData( " << len << " )" << std::endl;
        ApcStateMachine::sendTcpData( data, len );

        m_server->onIncomingTcpData( data, len );
    }

    StateVariables m_test_variables;
    StateActions m_test_actions;
    TestStates m_test_states;
    StateEvents m_test_events;

    HttpClientParserSimple m_http_parser_simple;
    HttpResponse m_http_response;

    ApsStateMachine *m_server;
};

class TestApsStateMachine : public ApsStateMachine
{
  public:
    class TestStates : public ApsStates
    {
      protected:
        virtual void goToInitialize()
        {
            std::cout << "APS: INITIALIZE" << std::endl;
            ApsStates::goToInitialize();
        }

        virtual void goToWaitForConnect()
        {
            std::cout << "APS: WAIT_FOR_CONNECT" << std::endl;
            ApsStates::goToWaitForConnect();
        }

        virtual void goToAccept()
        {
            std::cout << "APS: ACCEPT" << std::endl;
            ApsStates::goToAccept();
        }

        virtual void goToReject()
        {
            std::cout << "APS: REJECT" << std::endl;
            ApsStates::goToReject();
        }

        virtual void goToClosed()
        {
            std::cout << "APS: CLOSED" << std::endl;
            ApsStates::goToClosed();
        }

        virtual void goToStartTransfer()
        {
            std::cout << "APS: START_TRANSFER" << std::endl;
            ApsStates::goToStartTransfer();
        }

        virtual void goToWaiting()
        {
            std::cout << "APS: WAITING" << std::endl;
            ApsStates::goToWaiting();
        }

        virtual void goToLinkStatus()
        {
            std::cout << "APS: LINK_STATUS" << std::endl;
            ApsStates::goToLinkStatus();
        }

        virtual void goToTransferToL2()
        {
            std::cout << "APS: TRANSFER_TO_L2" << std::endl;
            ApsStates::goToTransferToL2();
        }

        virtual void goToTransferToApc()
        {
            std::cout << "APS: TRANSFER_TO_APC" << std::endl;
            ApsStates::goToTransferToApc();
        }

        virtual void goToAssignEntityId()
        {
            std::cout << "APS: ASSIGN_ENTITY_ID" << std::endl;
            ApsStates::goToAssignEntityId();
        }

        virtual void goToSendNop()
        {
            std::cout << "APS: SEND_NOP" << std::endl;
            ApsStates::goToSendNop();
        }

        virtual void goToCloseAndFinish()
        {
            std::cout << "APS: CLOSE_AND_FINISH" << std::endl;
            ApsStates::goToCloseAndFinish();
        }

        virtual void goToFinish()
        {
            std::cout << "APS: FINISH" << std::endl;
            ApsStates::goToFinish();
        }
    };

    TestApsStateMachine( uint16_t &assigned_id_count, active_connections_type &active_connections )
        : ApsStateMachine(
              &m_test_variables, &m_test_actions, &m_test_events, &m_test_states, assigned_id_count, active_connections )
        , m_test_events( &m_http_server_parser, "/" )
        , m_http_server_parser( &m_http_server_request, &m_test_events )
    {
    }

    virtual void sendAvdeccToL2( Frame const &frame )
    {
        std::cout << "APS: sendAvdeccToL2( " << frame.getLength() << " )" << std::endl;
        dump( std::cout, frame.getBuf(), frame.getLength() );
    }

    virtual void closeTcpConnection()
    {
        std::cout << "APS: closeTcpConnection( "
                  << " )" << std::endl;
        ApsStateMachine::closeTcpConnection();
        m_client->onTcpConnectionClosed();
    }

    virtual void closeTcpServer()
    {
        std::cout << "APS: closeTcpServer( "
                  << " )" << std::endl;
        ApsStateMachine::closeTcpServer();
    }

    virtual void sendTcpData( uint8_t const *data, ssize_t len )
    {
        std::cout << "APS: sendTcpData( " << len << " )" << std::endl;
        ApsStateMachine::sendTcpData( data, len );
        m_client->onIncomingTcpData( data, len );
    }

    ApsStateVariables m_test_variables;
    ApsStateActions m_test_actions;
    TestStates m_test_states;
    ApsStateEvents m_test_events;

    HttpServerParserSimple m_http_server_parser;
    HttpRequest m_http_server_request;

    ApcStateMachine *m_client;
};

TestApsStateMachine *aps;
TestApcStateMachine *apc;

uint32_t time_in_seconds = 0;

void tick()
{
    ++time_in_seconds;

    std::cout << "tick( " << std::dec << time_in_seconds << " )" << std::endl;

    aps->onTimeTick( time_in_seconds );
    apc->onTimeTick( time_in_seconds );
    aps->run();
    apc->run();
}

void formADP( Frame *adp, Eui48 sa, uint16_t valid_time_in_seconds )
{
    uint32_t entity_capabilities = JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED;
    uint16_t talker_stream_sources = 0;
    uint16_t talker_capabilities = 0;
    uint16_t listener_stream_sinks = 0;
    uint16_t listener_capabilities = 0;
    uint32_t controller_capabilities = 0;
    uint32_t available_index = 1;
    Eui64 gptp_grandmaster_id;

    adp->clear();
    adp->putEUI48( sa );
    adp->putEUI48( Eui48( jdksavdecc_multicast_adp_acmp ) );
    adp->putDoublet( JDKSAVDECC_AVTP_ETHERTYPE );
    adp->putOctet( JDKSAVDECC_1722A_SUBTYPE_ADP );

    // sv=0, version=0, message_type = ENTITY_AVAILABLE
    adp->putOctet( 0x00 + JDKSAVDECC_ADP_MESSAGE_TYPE_ENTITY_AVAILABLE );

    // valid_time is in 2 second steps. top 3 bits of control_data_length is 0
    adp->putOctet( ( valid_time_in_seconds / 2 ) << 3 );

    // control_data_length field is 56 - See 1722.1 Clause 6.2.1.7
    adp->putOctet( JDKSAVDECC_ADPDU_LEN - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN );

    adp->putEUI64( apc->getVariables()->m_entityId );
    adp->putEUI64( Eui64( 0x70, 0xb3, 0xd5, 0xff, 0xfe, 0xed, 0xcf, 0xf7 ) );
    adp->putQuadlet( entity_capabilities );

    adp->putDoublet( talker_stream_sources );
    adp->putDoublet( talker_capabilities );

    adp->putDoublet( listener_stream_sinks );
    adp->putDoublet( listener_capabilities );

    adp->putQuadlet( controller_capabilities );

    adp->putQuadlet( available_index );

    adp->putEUI64( gptp_grandmaster_id );

    // gptp_domain_number reserved0, identify_control_index, interface_index
    // association_id, reserved1
    // 20 octets total, all 0
    adp->putZeros( 20 );
}

int test1()
{
    int r = 255;

    std::cout << "APC: setup" << std::endl;

    apc->setup();

    apc->setPrimaryMac( Eui48( 0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf0 ) );
    apc->setEntityId( Eui64( 0x70, 0xb3, 0xd5, 0xff, 0xfe, 0xed, 0xcf, 0xf0 ) );
    apc->setApsAddress( "APSADDR" );
    apc->setPath( "/" );

    std::cout << "APS: setup" << std::endl;

    aps->setup();
    aps->setLinkMac( Eui48( 0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf1 ) );

    std::cout << "APS: run and connect" << std::endl;
    aps->run();
    aps->onIncomingTcpConnection();

    tick();
    tick();

    tick();
    tick();

    tick();
    tick();

    std::cout << "APC: send ADP to APS" << std::endl;

    {
        FrameWithMTU adp;
        formADP( &adp, apc->getVariables()->m_primaryMac, 30 );

        apc->onNetAvdeccMessageReceived( adp );
    }

    tick();
    tick();

    std::cout << "APS: send ADP to APC" << std::endl;
    {
        FrameWithMTU adp;
        formADP( &adp, apc->getVariables()->m_primaryMac, 30 );
        aps->onNetAvdeccMessageReceived( adp );
    }

    tick();
    tick();
    tick();
    tick();
    tick();
    tick();
    tick();
    tick();
    tick();
    tick();

    std::cout << "APS: LINK_UP on link" << std::endl;
    aps->onNetLinkStatusUpdated( aps->getLinkMac(), true );

    tick();
    tick();

    tick();
    tick();

    std::cout << "APC: close the connection" << std::endl;
    apc->closeTcpConnection();

    tick();
    tick();

    tick();
    tick();

    return r;
}

int main()
{
    int r = 255;

    {
        uint16_t assigned_id_count = 0;
        TestApsStateMachine::active_connections_type active_connections;

        TestApsStateMachine my_aps( assigned_id_count, active_connections );
        TestApcStateMachine my_apc;
        my_aps.m_client = &my_apc;
        my_apc.m_server = &my_aps;
        aps = &my_aps;
        apc = &my_apc;

        r = test1();
    }

    return r;
}
