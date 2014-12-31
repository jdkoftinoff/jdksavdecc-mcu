#include "JDKSAvdeccMCU.hpp"

using namespace JDKSAvdeccMCU;

class TestApcStateMachine;
class TestApsStateMachine;

class TestApcStateMachine : public ApcStateMachine
{
  public:
    TestApcStateMachine()
        : ApcStateMachine( &m_test_variables,
                           &m_test_actions,
                           &m_test_events,
                           &m_test_states )
        , m_test_events( &m_http_parser_simple, "/" )
        , m_http_parser_simple( &m_http_response, &m_test_events )
    {
    }

    virtual void closeTcpConnection()
    {
        std::cout << "APC: closeTCPConnection()" << std::endl;
        m_server->getEvents()->onTcpConnectionClosed();
    }

    virtual void connectToProxy( std::string const &addr )
    {
        std::cout << "APC: connectToProxy( " << addr << " )" << std::endl;
        m_server->getEvents()->onIncomingTcpConnection();
        getEvents()->onIncomingTcpConnection();
    }

    virtual void notifyProxyAvailable()
    {
        std::cout << "APC: notifyProxyAvailable()" << std::endl;
    }

    virtual void notifyProxyUnavailable()
    {
        std::cout << "APC: notifyProxyUnavailable()" << std::endl;
    }

    virtual void notifyLinkStatus( AppMessage const &linkMsg )
    {
        std::cout << "APC: notifyLinkStatus( " << std::hex
                  << linkMsg.getAddress().convertToUint64() << " "
                  << ( linkMsg.getMessageType() == AppMessage::LINK_UP ? 1 : 0 )
                  << " )" << std::endl;
    }

    virtual void processMsg( AppMessage const &apsMsg )
    {
        std::cout << "APC: processMsg( " << apsMsg.getMessageType() << " )"
                  << std::endl;
    }

    virtual void notifyNewEntityId( Eui64 const &entity_id )
    {
        std::cout << "APC: notifyNewEntityId( " << std::hex
                  << entity_id.convertToUint64() << " )" << std::endl;
    }

    virtual void sendTcpData( uint8_t const *data, ssize_t len )
    {
        std::cout << "APC: sendTcpData( " << len << " )" << std::endl;

        m_server->getEvents()->onIncomingTcpData( data, len );
    }

    StateVariables m_test_variables;
    StateActions m_test_actions;
    States m_test_states;
    StateEvents m_test_events;

    HttpClientParserSimple m_http_parser_simple;
    HttpResponse m_http_response;

    ApsStateMachine *m_server;
};

class TestApsStateMachine : public ApsStateMachine
{
  public:
    TestApsStateMachine()
        : ApsStateMachine( &m_test_variables,
                           &m_test_actions,
                           &m_test_events,
                           &m_test_states )
        , m_test_events( &m_http_server_parser, "/" )
        , m_http_server_parser( &m_http_server_request, &m_test_events )
    {
    }

    virtual void sendAvdeccToL2( AppMessage const *msg )
    {
        std::cout << "APS: sendAvdeccToL2( " << msg->getMessageType() << " )"
                  << std::endl;
    }

    virtual void closeTcpConnection()
    {
        std::cout << "APS: closeTcpConnection( "
                  << " )" << std::endl;

        m_client->getEvents()->onTcpConnectionClosed();
    }

    virtual void closeTcpServer()
    {
        std::cout << "APS: closeTcpServer( "
                  << " )" << std::endl;
    }

    virtual void sendTcpData( uint8_t const *data, ssize_t len )
    {
        std::cout << "APS: sendTcpData( " << len << " )" << std::endl;

        m_client->getEvents()->onIncomingTcpData( data, len );
    }

    StateVariables m_test_variables;
    StateActions m_test_actions;
    States m_test_states;
    StateEvents m_test_events;

    HttpServerParserSimple m_http_server_parser;
    HttpRequest m_http_server_request;

    ApcStateMachine *m_client;
};

TestApsStateMachine *aps;
TestApcStateMachine *apc;

uint32_t time_in_seconds = 0;

void tick()
{
    std::cout << "tick()" << std::endl;

    ++time_in_seconds;
    aps->getEvents()->onTimeTick( time_in_seconds );
    apc->getEvents()->onTimeTick( time_in_seconds );
    aps->run();
    apc->run();
}

void formADP( Frame *adp, Eui48 sa, uint16_t valid_time_in_seconds )
{
    uint32_t entity_capabilities
        = JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED;
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
    adp->putOctet( JDKSAVDECC_ADPDU_LEN
                   - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN );

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

    aps->start();
    apc->start();

    apc->getVariables()->m_primaryMac
        = Eui48( 0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf0 );
    apc->getVariables()->m_entityId
        = Eui64( 0x70, 0xb3, 0xd5, 0xff, 0xfe, 0xed, 0xcf, 0xf0 );
    apc->getVariables()->m_addr = "APSADDR";

    {
        std::vector<std::string> headers;
        apc->getVariables()->m_request.setCONNECT( "/", headers );
    }

    aps->getVariables()->m_linkMac
        = Eui48( 0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf1 );
    aps->getVariables()->m_linkStatus = true;

    tick();
    tick();

    tick();
    tick();

    tick();
    tick();

    {
        FrameWithMTU adp;
        formADP( &adp, apc->getVariables()->m_primaryMac, 30 );

        apc->getEvents()->onNetAvdeccMessageReceived( adp );
    }

    tick();
    tick();

    {
        FrameWithMTU adp;
        formADP( &adp, apc->getVariables()->m_primaryMac, 30 );
        aps->getEvents()->onNetAvdeccMessageReceived( adp );
    }

    tick();
    tick();

    apc->closeTcpConnection();

    return r;
}

int main()
{
    int r = 255;

    {
        TestApsStateMachine my_aps;
        TestApcStateMachine my_apc;
        my_aps.m_client = &my_apc;
        my_apc.m_server = &my_aps;
        aps = &my_aps;
        apc = &my_apc;

        r = test1();
    }

    return r;
}
