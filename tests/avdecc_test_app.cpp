#include "JDKSAvdeccMCU.hpp"

using namespace JDKSAvdeccMCU;

class TestApcStateMachine;
class TestApsStateMachine;

class TestApcStateMachine : public ApcStateMachine
{
  public:
    class TestStateVariables;
    class TestStateActions;
    class TestStates;
    class TestStateEvents;

    class TestStateVariables : public StateVariables
    {
      public:
        TestStateVariables() {}
        void dump( std::ostream &ostr ) {}
    };

    class TestStateActions : public StateActions
    {
      public:
        TestStateActions() {}
        void dump( std::ostream &ostr ) {}

        virtual void closeTcpConnection() {}
        virtual void connectToProxy( std::string const &addr ) {}
        virtual void notifyProxyAvailable() {}
        virtual void notifyProxyUnavailable() {}
        virtual void notifyLinkStatus( AppMessage const &linkMsg ) {}
        virtual void processMsg( AppMessage const &apsMsg ) {}
        virtual void notifyNewEntityId( Eui64 const &entity_id ) {}
    };

    class TestStates : public States
    {
      public:
        void dump( std::ostream &ostr ) {}
    };

    class TestStateEvents : public StateEvents
    {
      public:
        TestStateEvents()
            : StateEvents( &m_http_client_parser, "/" )
            , m_http_client_parser( &m_http_response, this )
        {
        }

        void dump( std::ostream &ostr ) {}
        virtual void sendTcpData( uint8_t const *data, ssize_t len ) {}

        HttpClientParserSimple m_http_client_parser;
        HttpResponse m_http_response;
    };

    TestApcStateMachine()
        : ApcStateMachine( &m_test_variables,
                           &m_test_actions,
                           &m_test_events,
                           &m_test_states )
    {
    }

    TestStateVariables m_test_variables;
    TestStateActions m_test_actions;
    TestStates m_test_states;
    TestStateEvents m_test_events;
};

class TestApsStateMachine : public ApsStateMachine
{
  public:
    class TestStateVariables;
    class TestStateActions;
    class TestStates;
    class TestStateEvents;

    class TestStateVariables : public StateVariables
    {
      public:
        TestStateVariables() {}
        void dump( std::ostream &ostr ) {}
    };

    class TestStateActions : public StateActions
    {
      public:
        TestStateActions() {}
        void dump( std::ostream &ostr ) {}

        virtual void sendAvdeccToL2( AppMessage const *msg ) {}
        virtual void closeTcpConnection() {}
        virtual void closeTcpServer() {}
    };

    class TestStates : public States
    {
      public:
        void dump( std::ostream &ostr ) {}
    };

    class TestStateEvents : public StateEvents
    {
      public:
        TestStateEvents( std::string const &path )
            : StateEvents( &m_http_server_parser, path )
            , m_http_server_parser( &m_http_server_request, this )
        {
        }

        void dump( std::ostream &ostr ) {}
        virtual void sendTcpData( uint8_t const *data, ssize_t len ) {}

        HttpServerParserSimple m_http_server_parser;
        HttpRequest m_http_server_request;
    };

    TestApsStateMachine()
        : ApsStateMachine( &m_test_variables,
                           &m_test_actions,
                           &m_test_events,
                           &m_test_states )
        , m_test_events( "/" )
    {
    }

    TestStateVariables m_test_variables;
    TestStateActions m_test_actions;
    TestStates m_test_states;
    TestStateEvents m_test_events;
};

int main()
{
    int r = 255;

    TestApsStateMachine aps;
    TestApcStateMachine apc;

    return r;
}
