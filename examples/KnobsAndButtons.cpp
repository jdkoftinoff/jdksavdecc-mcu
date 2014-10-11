
#include "JDKSAvdeccMCU_Arduino.h"

#define REFRESH_TIME ( 1000 )

using namespace JDKSAvdeccMCU;

/// This MAC address is based on the J.D. Koftinoff Software, Ltd. assigned
/// MAC-S (OUI36): 70:b3:d5:ed:c
/// JDKS reserves the MAC range from 70:b3:d5:ed:cf:f0 to 70:b3:d5:ed:cf:f7
/// inclusive for experimental devices only

jdksavdecc_eui48 my_mac = {{0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf0}};

/// Ethernet handler object

#if defined( JDKSAVDECCMCU_BARE_METAL )
RawSocketWizeNet rawnet( my_mac,
                         JDKSAVDECC_AVTP_ETHERTYPE,
                         &jdksavdecc_multicast_adp_acmp ); // For embedded
                                                           // systems
#elif defined( JDKSAVDECCMCU_ENABLE_RAW )
RawSocketPcap rawnet( "en0",
                      JDKSAVDECC_AVTP_ETHERTYPE,
                      &jdksavdecc_multicast_adp_acmp ); // For non-embedded
                                                        // systems
#else
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

// ControllerEntity my_entity(adp_manager);

/// The shared sequence ID for transmitted messages
uint16_t sequence_id = 0;

/// The mapping of Knob 1 to control descriptor 0x0000. 2 byte payload, refresh
/// time of 1000 ms
ControlSender knob1( rawnet,
                     my_entity_id,
                     target_entity_id,
                     target_mac_address,
                     sequence_id,
                     0x0000,
                     2,
                     REFRESH_TIME );

/// The mapping of Knob 2 to control descriptor 0x0001. 2 byte payload, refresh
/// time of 1000 ms
ControlSender knob2( rawnet,
                     my_entity_id,
                     target_entity_id,
                     target_mac_address,
                     sequence_id,
                     0x0001,
                     2,
                     REFRESH_TIME );

/// The mapping of Knob 2 to control descriptor 0x0002. 2 byte payload, refresh
/// time of 1000 ms
ControlSender knob3( rawnet,
                     my_entity_id,
                     target_entity_id,
                     target_mac_address,
                     sequence_id,
                     0x0002,
                     2,
                     REFRESH_TIME );

/// The mapping of Button 1 to control descriptor 0x0003. 2 byte payload,
/// refresh time of 1000 ms
ControlSender button1( rawnet,
                       my_entity_id,
                       target_entity_id,
                       target_mac_address,
                       sequence_id,
                       0x0003,
                       1,
                       REFRESH_TIME );

/// The mapping of Button 2 to control descriptor 0x0004. 2 byte payload,
/// refresh time of 1000 ms
ControlSender button2( rawnet,
                       my_entity_id,
                       target_entity_id,
                       target_mac_address,
                       sequence_id,
                       0x0004,
                       1,
                       REFRESH_TIME );

/// The mapping of Button 3 to control descriptor 0x0005. 2 byte payload,
/// refresh time of 1000 ms
ControlSender button3( rawnet,
                       my_entity_id,
                       target_entity_id,
                       target_mac_address,
                       sequence_id,
                       0x0005,
                       1,
                       REFRESH_TIME );

/// The mapping of Button 3 to control descriptor 0x0006. 2 byte payload,
/// refresh time of 1000 ms
ControlSender button4( rawnet,
                       my_entity_id,
                       target_entity_id,
                       target_mac_address,
                       sequence_id,
                       0x0006,
                       1,
                       REFRESH_TIME );

/// The mapping of Button 3 to control descriptor 0x0007. 2 byte payload,
/// refresh time of 1000 ms
ControlSender button5( rawnet,
                       my_entity_id,
                       target_entity_id,
                       target_mac_address,
                       sequence_id,
                       0x0007,
                       1,
                       REFRESH_TIME );

/// The ValueMapper scans the A/D's and Digital input pins at the specified rate
/// and applies the new values to the appropriate knob or button
class ValueMapper : public Handler
{
  private:
    /// The update rate in milliseconds
    uint32_t m_update_rate_in_millis;

    /// The time that the last update happened
    uint32_t m_last_update_time;

  public:
    ValueMapper( jdksavdecc_timestamp_in_milliseconds update_rate_in_millis )
        : m_update_rate_in_millis( update_rate_in_millis )
        , m_last_update_time( 0 )
    {
    }

    virtual void tick()
    {
        jdksavdecc_timestamp_in_milliseconds time_in_millis
            = rawnet.getTimeInMilliseconds();

        if ( time_in_millis > m_last_update_time + m_update_rate_in_millis )
        {
            m_last_update_time = time_in_millis;

            knob1.setValueDoublet(
                analogRead( 0 ) ); // A/D values range from 0 to 0x3ff
            knob2.setValueDoublet( analogRead( 1 ) );
            knob3.setValueDoublet( analogRead( 2 ) );

            button1.setValueOctet(
                digitalRead( 2 ) ? 0x00 : 0xff ); // DLI reads true when not
                                                  // pressed, reverse logic
            button2.setValueOctet( digitalRead( 3 ) ? 0x00 : 0xff );
            button3.setValueOctet( digitalRead( 4 ) ? 0x00 : 0xff );
            button4.setValueOctet( digitalRead( 5 ) ? 0x00 : 0xff );
            button5.setValueOctet( digitalRead( 6 ) ? 0x00 : 0xff );

            digitalWrite( A5, !digitalRead( 2 ) );
            digitalWrite( A4, !digitalRead( 3 ) );
        }
    }
};

/// Create the mapper which polls at 50 ms
ValueMapper value_mapper( 50 );

/// Create a HandlerGroup which can manage up to 16 handlers
HandlerGroup<16> all_handlers( rawnet );

void setup()
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

    // Initialize the serial port for debug logs
    Serial.begin( 9600 );

    // Initialize the ethernet chip
    rawnet.initialize();

    // Put all the handlers into the HandlerGroup

    all_handlers.add( &value_mapper );
    all_handlers.add( &adp_manager );
    all_handlers.add( &knob1 );
    all_handlers.add( &knob2 );
    all_handlers.add( &knob3 );
    all_handlers.add( &button1 );
    all_handlers.add( &button2 );
    all_handlers.add( &button3 );
    all_handlers.add( &button4 );
    all_handlers.add( &button5 );
    //  all_handlers.Add( &my_entity );
}

void jdksavdeccmcu_debug_log( const char *str, uint16_t v )
{
    char txt[64];
    Frame<256> pdu;
    uint16_t r;
    sprintf( txt, "%s %u", str, (unsigned)v );
    r = jdksavdecc_jdks_log_control_generate( &my_entity_id,
                                              8, // Control index 8
                                              &sequence_id,
                                              JDKSAVDECC_JDKS_LOG_INFO,
                                              0,
                                              txt,
                                              pdu.getBuf(),
                                              pdu.getMaxLength() );
    if ( r > 0 )
    {
        pdu.setLength( r );
        memcpy(
            pdu.getBuf() + JDKSAVDECC_FRAME_HEADER_SA_OFFSET, my_mac.value, 6 );
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
