#include "JDKSAvdeccMCU.hpp"

using namespace JDKSAvdeccMCU;

int main()
{
    RawSocketMulti net( JDKSAVDECC_AVTP_ETHERTYPE, jdksavdecc_multicast_adp_acmp );

    if ( RawSocketTracker::num_rawsockets > 0 )
    {
        Eui64 entity_id( 0x70, 0xb3, 0xd5, 0xff, 0xfe, 0xed, 0xc0, 0x01 );
        ADPCoreInfo adp_info( Eui64( 0x70, 0xb3, 0xd5, 0xff, 0xfe, 0xed, 0xc0, 0x02 ),
                              JDKSAVDECC_ADP_ENTITY_CAPABILITY_ADDRESS_ACCESS_SUPPORTED
                              | JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED,
                              JDKSAVDECC_ADP_CONTROLLER_CAPABILITY_IMPLEMENTED);

        ADPManager adp_manager( net,
                                entity_id,
                                adp_info);

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
