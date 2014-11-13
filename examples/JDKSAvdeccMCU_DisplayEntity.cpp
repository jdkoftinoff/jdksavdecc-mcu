#ifdef __AVR__
#include "JDKSAvdeccMCU_Arduino.hpp"
#include "Picaso_Serial_4DLib.h"
#include "Picaso_Const4D.h"
#include "Ethernet.h"
#include "SPI.h"

Picaso_Serial_4DLib Display( &Serial );

using namespace JDKSAvdeccMCU;

/// This MAC address is based on the J.D. Koftinoff Software, Ltd. assigned
/// MAC-S (OUI36): 70:b3:d5:ed:c
/// JDKS reserves the MAC range from 70:b3:d5:ed:cf:f0 to 70:b3:d5:ed:cf:f7
/// inclusive for experimental devices only

jdksavdecc_eui48 my_mac = {{0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf1}};

/// This AVDECC Entity is based on the mac address (insert ff ff)
jdksavdecc_eui64 my_entity_id
    = {{0x70, 0xb3, 0xd5, 0xff, 0xff, 0xed, 0xcf, 0xf1}};

/// This AVDECC Entity Model ID is based on the J.D. Koftinoff Software, Ltd.
/// assigned MAC-S (OUI36): 70:b3:d5:ed:c
jdksavdecc_eui64 my_entity_model_id
    = {{0x70, 0xb3, 0xd5, 0xed, 0xc0, 0x00, 0x00, 0x01}};

/// the W5100 chip Raw Ethernet handler object
WizNetIO rawnet( my_mac );

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

/// A VisualHandler is an object which is a Handler, can hold a control value,
/// And contains GUI position, width, and height properties.  It
/// Also keeps track of a maximum update rate; new changes immediately
/// cause an update unless the last update was too recent.
class VisualHandler : public ControlValueHolder
{
  protected:
    uint16_t m_x;
    uint16_t m_y;
    uint16_t m_w;
    uint16_t m_h;
    uint32_t m_last_update_time_millis;
    uint16_t m_max_update_rate_millis;

  public:
    VisualHandler( uint8_t value_length,
                   uint16_t x,
                   uint16_t y,
                   uint16_t w,
                   uint16_t h,
                   uint16_t max_update_rate_millis = 50 )
        : ControlValueHolder( value_length )
        , m_x( x )
        , m_y( y )
        , m_w( w )
        , m_h( h )
        , m_last_update_time_millis( 0 )
        , m_max_update_rate_millis( max_update_rate_millis )
    {
    }

    virtual void tick( jdksavdecc_timestamp_in_milliseconds time_in_millis )
    {
        if ( time_in_millis > m_last_update_time_millis
                              + m_max_update_rate_millis )
        {
            if ( isDirty() )
            {
                Draw();
                clearDirty();
                m_last_update_time_millis = time_in_millis;
            }
        }
    }

    virtual void Draw() {}
};

class VisualSlider : public VisualHandler
{
  protected:
    uint16_t m_scale;

  public:
    VisualSlider( uint8_t value_length,
                  uint16_t x,
                  uint16_t y,
                  uint16_t w,
                  uint16_t h,
                  uint16_t scale,
                  uint16_t max_update_rate_millis = 50 )
        : VisualHandler( value_length, x, y, w, h, max_update_rate_millis )
        , m_scale( scale )
    {
    }

    virtual void Draw()
    {
        Display.gfx_Slider( SLIDER_RAISED,
                            m_x,
                            m_y,
                            m_x + m_w,
                            m_y + m_h,
                            GRAY,
                            m_scale,
                            m_scale - getValueDoublet() );
    }
};

class VisualLight : public VisualHandler
{
  public:
    VisualLight( uint8_t value_length,
                 uint16_t x,
                 uint16_t y,
                 uint16_t w,
                 uint16_t max_update_rate_millis = 50 )
        : VisualHandler( value_length, x, y, w, w, max_update_rate_millis )
    {
    }

    virtual void Draw()
    {
        Display.gfx_Rectangle( m_x, m_y, m_x + m_w, m_y + m_h, WHITE );
        Display.gfx_RectangleFilled( m_x + 1,
                                     m_y + 1,
                                     m_x + m_w - 1,
                                     m_y + m_h - 1,
                                     getValue() == 0 ? BLACK : LIGHTBLUE );
    }
};

#define WIDGET_W ( 40 )
#define WIDGET_H_SPACING ( WIDGET_W + 8 )
#define SLIDER_H ( 150 )
#define PANEL_Y ( 50 )
#define PANEL_X ( 50 )

VisualSlider slider1(
    2, PANEL_X + WIDGET_H_SPACING * 0, PANEL_Y, WIDGET_W, SLIDER_H, 0x400 );
VisualSlider slider2(
    2, PANEL_X + WIDGET_H_SPACING * 1, PANEL_Y, WIDGET_W, SLIDER_H, 0x400 );
VisualSlider slider3(
    2, PANEL_X + WIDGET_H_SPACING * 2, PANEL_Y, WIDGET_W, SLIDER_H, 0x400 );

VisualLight light1( 1,
                    PANEL_X + WIDGET_H_SPACING * 0,
                    SLIDER_H + PANEL_Y + 20,
                    WIDGET_W );
VisualLight light2( 1,
                    PANEL_X + WIDGET_H_SPACING * 1,
                    SLIDER_H + PANEL_Y + 20,
                    WIDGET_W );
VisualLight light3( 1,
                    PANEL_X + WIDGET_H_SPACING * 2,
                    SLIDER_H + PANEL_Y + 20,
                    WIDGET_W );

VisualLight light4( 1,
                    PANEL_X + WIDGET_H_SPACING * 0,
                    SLIDER_H + PANEL_Y + 20 + WIDGET_H_SPACING,
                    WIDGET_W );
VisualLight light5( 1,
                    PANEL_X + WIDGET_H_SPACING * 1,
                    SLIDER_H + PANEL_Y + 20 + WIDGET_H_SPACING,
                    WIDGET_W );

ControlReceiver<16> control_receiver( rawnet, my_entity_id );

/// Create a HandlerGroup which can manage up to 16 handlers
HandlerGroup<16> all_handlers;

void display_callback( int ErrCode, unsigned char Errorbyte )
{
    // Toggle the LED connected to digital pin 9 if the communication to the
    // display fails
    {
        avr_debug_log( "Unable to communicate with LCD - Rebooting", 0 );
        digitalWrite( 9, HIGH ); // turn the LED on (HIGH is the voltage level)
        delay( 400 );            // wait
        digitalWrite( 9, LOW );  // turn the LED off by making the voltage LOW
        delay( 400 );            // wait
    }
    asm volatile( "  jmp 0" ); // restart
}

void setup()
{
    // Initialize the W5100 chip
    net->initialize();
    // set pin 9 to be output for LED status
    pinMode( 9, OUTPUT );
    digitalWrite( 9, LOW ); // turn the LED off by making the voltage LOW

    // Set serial baud rate for the display to 19200
    Display.TimeLimit4D = 5000;
    Display.Callback4D = display_callback;
    Serial.begin( 9600 );
    delay( 50 );
    Serial.flush();
    Display.gfx_Cls();

    // Tell the control receiver which widgets are mapped to which control
    // descriptors
    control_receiver.addDescriptor( &slider1 );
    control_receiver.addDescriptor( &slider2 );
    control_receiver.addDescriptor( &slider3 );
    control_receiver.addDescriptor(
        &light3 ); // Board layout has them in this order
    control_receiver.addDescriptor( &light2 );
    control_receiver.addDescriptor( &light1 );
    control_receiver.addDescriptor( &light4 );
    control_receiver.addDescriptor( &light5 );

    // Put all the handlers into the HandlerGroup
    all_handlers.add( &adp_manager );
    all_handlers.add( &control_receiver );
    all_handlers.add( &slider1 );
    all_handlers.add( &slider2 );
    all_handlers.add( &slider3 );
    all_handlers.add( &light1 );
    all_handlers.add( &light2 );
    all_handlers.add( &light3 );
    all_handlers.add( &light4 );
    all_handlers.add( &light5 );

    // Initialize the graphics display
    Display.gfx_Cls();
    Display.txt_MoveCursor( 0, 0 );
    Display.txt_FGcolour( WHITE );

    // put the header info nicely on top
    Display.putstr(
        " JDKSAvdeccArduino Example #2\n      By Jeff Koftinoff\n   "
        "http://avb.statusbar.com" );
    // fill the rest of the screen with dark blue
    Display.gfx_RectangleFilled( 0, PANEL_Y - 10, 239, 319, DARKBLUE );
}

uint16_t sequence_id = 0;

extern "C" {
void avr_debug_log( const char *str, uint16_t v )
{
    uint16_t r;
    {
        char txt[64];
        char pdu[256];
        sprintf( txt, "%s %u", str, (unsigned)v );
        r = jdksavdecc_jdks_log_control_generate( &my_entity_id,
                                                  8, // Control index 8
                                                  &sequence_id,
                                                  JDKSAVDECC_JDKS_LOG_INFO,
                                                  0,
                                                  txt,
                                                  pdu,
                                                  sizeof( pdu ) );
        if ( r > 0 )
        {
            memcpy( pdu + JDKSAVDECC_FRAME_HEADER_SA_OFFSET, my_mac.value, 6 );
            net->sendRawNet( (uint8_t *)pdu, r );
        }
    }
}
}

void loop()
{
    static uint16_t last_second = 0xffff;
    jdksavdecc_timestamp_in_milliseconds cur_time = millis();
    all_handlers.tick( cur_time );
    if ( cur_time / 1000 != last_second )
    {
        last_second = cur_time / 1000;
        avr_debug_log( ":time:", last_second );
    }
}

#else

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU.hpp"

using namespace JDKSAvdeccMCU;

#endif

#if JDKSAVDECCMCU_BARE_METAL == 0

int main( int, char ** ) { return 0; }

#endif
