#include "JDKSAvdeccMCU.hpp"

using namespace JDKSAvdeccMCU;

int main()
{
    AppMessage msg;
    AppMessageHandler *handler = 0;
    AppMessageParser parser(*handler);
    parser.parse(0x00);

    Eui48 join_multicast = jdksavdecc_multicast_adp_acmp;

    RawSocketMulti net( JDKSAVDECC_AVTP_ETHERTYPE, join_multicast );

    if ( RawSocketTracker::num_rawsockets > 0 )
    {
        Eui64 entity_id( 0x70, 0xb3, 0xd5, 0xff, 0xfe, 0xed, 0xc0, 0x01 );
        Eui64 entity_model_id( 0x70, 0xb3, 0xd5, 0xff, 0xfe, 0xed, 0xc0, 0x02 );
        uint32_t entity_capabilities
            = JDKSAVDECC_ADP_ENTITY_CAPABILITY_ADDRESS_ACCESS_SUPPORTED
              | JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED;
        uint32_t controller_capabilities
            = JDKSAVDECC_ADP_CONTROLLER_CAPABILITY_IMPLEMENTED;
        uint16_t valid_time_in_seconds = 40;
        ADPManager adp_manager( net,
                                entity_id,
                                entity_model_id,
                                entity_capabilities,
                                controller_capabilities,
                                valid_time_in_seconds );

        net.setNonblocking();

        FrameWithSize<1522> frame;
        HandlerGroupWithSize<4> handlers( &frame );

        handlers.add( &adp_manager );
        while ( 1 )
        {
            handlers.tick( net.getTimeInMilliseconds() );
#if defined( _WIN32 )
            Sleep( 10 );
#else
            usleep( 10000 );
#endif
        }
    }
}
