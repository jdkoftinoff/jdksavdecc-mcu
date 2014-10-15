#if defined( __APPLE__ ) || defined( __linux__ )
#define JDKSAVDECCMCU_ARDUINO 1
#else
#include <SPI.h>
#include <Ethernet.h>
#endif

#include "JDKSAvdeccMCU_Arduino.h"

#define REFRESH_TIME ( 1000 )

using namespace JDKSAvdeccMCU;

/// This MAC address is based on the J.D. Koftinoff Software, Ltd. assigned
/// MAC-S (OUI36): 70:b3:d5:ed:c
/// JDKS reserves the MAC range from 70:b3:d5:ed:cf:f0 to 70:b3:d5:ed:cf:f7
/// inclusive for experimental devices only

jdksavdecc_eui48 my_mac = {{0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf0}};

/// Ethernet handler object

#if JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET == 1
RawSocketWizNet rawnet( my_mac,
                        JDKSAVDECC_AVTP_ETHERTYPE,
                        &jdksavdecc_multicast_adp_acmp ); // For embedded
                                                          // systems
#elif JDKSAVDECCMCU_ENABLE_RAWSOCKETMACOSX
RawSocketMacOSX rawnet( "en0",
                        JDKSAVDECC_AVTP_ETHERTYPE,
                        &jdksavdecc_multicast_adp_acmp ); // For non-embedded
                                                          // systems
#elif JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE
RawSocketPcapFile rawnet( JDKSAVDECC_AVTP_ETHERTYPE,
                          "input.pcap",
                          "output.pcap",
                          &jdksavdecc_multicast_adp_acmp );
#endif

/// This AVDECC Entity is based on the mac address (insert ff ff)
jdksavdecc_eui64 my_entity_id
    = {{0x70, 0xb3, 0xd5, 0xff, 0xff, 0xed, 0xcf, 0xf0}};

/// This AVDECC Entity Model ID is based on the J.D. Koftinoff Software, Ltd.
/// assigned MAC-S (OUI36): 70:b3:d5:ed:c
jdksavdecc_eui64 my_entity_model_id
    = {{0x70, 0xb3, 0xd5, 0xed, 0xc0, 0x00, 0x00, 0x00}};

/// All of the controls are targetting the entity of the second JDKS
/// Experimental MAC address
jdksavdecc_eui64 target_entity_id
    = {{0x70, 0xb3, 0xd5, 0xff, 0xff, 0xed, 0xcf, 0xf1}};

/// All of the controls are targetting the second JDKS experimental MAC address
/// 70:b3:d5:ed:cf:f1
jdksavdecc_eui48 target_mac_address = {{0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf1}};

/// The ADP manager is told about the entity id, model_id, entity capabilities,
/// controller capabilities, and valid time in
/// seconds
ADPManager adp_manager( rawnet,
                        my_entity_id,
                        my_entity_model_id,
                        JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED,
                        JDKSAVDECC_ADP_CONTROLLER_CAPABILITY_IMPLEMENTED,
                        20 );

class MyEntityState : public EntityState
{
  public:
    MyEntityState( RawSocket &net, ADPManager &adp_manager )
        : m_controller_entity( net, adp_manager, this )
        , m_update_rate_in_millis( 50 )
        , m_last_update_time( 0 )
        , m_knobs_sender( m_controller_entity,
                          target_entity_id,
                          target_mac_address,
                          0x0000,
                          REFRESH_TIME,
                          &m_knobs_storage )
        , m_buttons_sender( m_controller_entity,
                            target_entity_id,
                            target_mac_address,
                            0x0001,
                            REFRESH_TIME,
                            &m_buttons_storage )
    {
        // Set up the I/O pins for 3 knobs, 5 buttons, and 2 LED's
        pinMode( 9, OUTPUT );
        pinMode( 2, INPUT_PULLUP );
        pinMode( 3, INPUT_PULLUP );
        pinMode( 4, INPUT_PULLUP );
        pinMode( 5, INPUT_PULLUP );
        pinMode( 6, INPUT_PULLUP );
        pinMode( A4, OUTPUT );
        pinMode( A5, OUTPUT );
    }

    virtual void addToHandlerGroup( HandlerGroup &group )
    {
        group.add( this );
        group.add( &m_knobs_sender );
        group.add( &m_buttons_sender );
        group.add( &m_controller_entity );
    }

    virtual void tick()
    {
        jdksavdecc_timestamp_in_milliseconds time_in_millis
            = rawnet.getTimeInMilliseconds();

        if ( time_in_millis > m_last_update_time + m_update_rate_in_millis )
        {
            m_last_update_time = time_in_millis;

            for ( uint8_t i = 0; m_knobs_storage.getNumItems(); ++i )
            {
                m_knobs_storage.setDoublet( analogRead( i ), i );
            }

            for ( uint8_t i = 0; m_buttons_storage.getNumItems(); ++i )
            {
                m_buttons_storage.setDoublet(
                    digitalRead( i + 2 ) ? 0x00 : 0xff, i );
            }

            digitalWrite( A5, !digitalRead( 2 ) );
            digitalWrite( A4, !digitalRead( 3 ) );
        }
    }

    virtual uint8_t readDescriptorEntity( jdksavdecc_aecpdu_aem const &aem,
                                          Frame &pdu,
                                          uint16_t configuration_index,
                                          uint16_t descriptor_type,
                                          uint16_t descriptor_index )
    {
        uint8_t status = JDKSAVDECC_AEM_STATUS_BAD_ARGUMENTS;

        if ( configuration_index == 0 && descriptor_index == 0 )
        {
            pdu.setLength(
                JDKSAVDECC_FRAME_HEADER_LEN
                + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_COMMAND_OFFSET_DESCRIPTOR_TYPE );
            pdu.putDoublet( JDKSAVDECC_DESCRIPTOR_ENTITY );

            // descriptor_index
            pdu.putDoublet( 0 );

            // entity_id
            pdu.putEUI64( m_controller_entity.getEntityID() );

            // entity_model_id
            pdu.putEUI64( adp_manager.getEntityModelID() );

            // entity_capabilities
            pdu.putQuadlet( JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED );

            // talker_stream_sources
            pdu.putDoublet( 0 );

            // talker_capabilities
            pdu.putDoublet( 0 );

            // listener_stream_sources
            pdu.putDoublet( 0 );

            // listener_stream_sinks
            pdu.putDoublet( 0 );

            // listener_capabilities
            pdu.putDoublet( 0 );

            // controller_capabilities
            pdu.putQuadlet( JDKSAVDECC_ADP_CONTROLLER_CAPABILITY_IMPLEMENTED );

            // available_index
            pdu.putQuadlet( adp_manager.getAvailableIndex() );

            // association_id = 0
            pdu.putEUI64();

            // entity_name
            pdu.putAvdeccString( "JDKSAvdecc-MCU" );

            // vendor_name_string
            pdu.putDoublet( JDKSAVDECC_NO_STRING );

            // model_name_string
            pdu.putDoublet( JDKSAVDECC_NO_STRING );

            // firmware_version
            pdu.putAvdeccString( "V3.0" );

            // group_name
            pdu.putAvdeccString();

            // serial_number
            pdu.putAvdeccString( "12345678" );

            // configurations_count
            pdu.putDoublet( 1 );

            // current_configuration
            pdu.putDoublet( 0 );

            m_controller_entity.sendResponses( false, false, pdu );
            status = JDKSAVDECC_AEM_STATUS_SUCCESS;
        }

        return status;
    }

  private:
    ControllerEntity m_controller_entity;

    /// The update rate in milliseconds
    uint32_t m_update_rate_in_millis;

    /// The time that the last update happened
    uint32_t m_last_update_time;

    /// The mapping of Knob 1,2,3 to control descriptor 0x0000. 6 byte payload,
    /// one doublet each
    ControlValueHolderWithStorage<uint16_t, 3> m_knobs_storage;
    ControlSender m_knobs_sender;

    /// The mapping of Button 1 to control descriptor 0x0001. 5 byte payload,
    /// one octet each
    ControlValueHolderWithStorage<uint8_t, 5> m_buttons_storage;
    ControlSender m_buttons_sender;
};

MyEntityState my_entity_state( rawnet, adp_manager );

/// Create a HandlerGroup which can manage up to 16 handlers
HandlerGroupWithSize<16> all_handlers( rawnet );

void setup()
{

    // Initialize the serial port for debug logs
    Serial.begin( 9600 );

    // Initialize the ethernet chip
    rawnet.initialize();

    // Put all the handlers into the HandlerGroup

    adp_manager.addToHandlerGroup( all_handlers );
    my_entity_state.addToHandlerGroup( all_handlers );
}

void jdksavdeccmcu_debug_log( const char *str, uint16_t v )
{
    static uint16_t debug_sequence_id = 0;
    char txt[64];
    FrameWithSize<256> pdu;
    uint16_t r;
    sprintf( txt, "%s %u", str, (unsigned)v );
    r = jdksavdecc_jdks_log_control_generate( &my_entity_id,
                                              8, // Control index 8
                                              &debug_sequence_id,
                                              JDKSAVDECC_JDKS_LOG_INFO,
                                              0,
                                              txt,
                                              pdu.getBuf(),
                                              pdu.getMaxLength() );
    if ( r > 0 )
    {
        pdu.setLength( r );
        memcpy(
            pdu.getBuf( JDKSAVDECC_FRAME_HEADER_SA_OFFSET ), my_mac.value, 6 );
        RawSocket::multiSendFrame( pdu );
    }
    else
    {
        Serial.println( " error " );
    }
}

void loop()
{
    static jdksavdecc_timestamp_in_milliseconds last_second = 0;
    // Get the current time in milliseconds
    jdksavdecc_timestamp_in_milliseconds cur_time = millis();

    // Tell all the handlers to do their periodic jobs
    all_handlers.tick();
    if ( cur_time / 1000 != last_second )
    {
        last_second = cur_time / 1000;
        jdksavdeccmcu_debug_log( "Time from KnobsAndButtons is:", last_second );
    }
}

#ifndef __AVR__
int main( int argc, char **argv )
{
    (void)argc;
    (void)argv;
    setup();
    while ( true )
    {
        loop();
    }
}

#endif
