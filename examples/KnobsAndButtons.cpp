#include "JDKSAvdecc.h"
#include "Ethernet.h"
#include "SPI.h"

#define REFRESH_TIME (1000)

using namespace JDKSAvdecc;

/// This MAC address is based on the J.D. Koftinoff Software, Ltd. assigned MAC-S (OUI36): 70:b3:d5:ed:c
/// JDKS reserves the MAC range from 70:b3:d5:ed:cf:f0 to 70:b3:d5:ed:cf:f7 inclusive for experimental devices only

jdksavdecc_eui48 my_mac = { { 0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf0 } };

/// This AVDECC Entity is based on the mac address (insert ff ff)
jdksavdecc_eui64 my_entity_id = { {0x70, 0xb3, 0xd5, 0xff, 0xff, 0xed, 0xcf, 0xf0 } };

/// This AVDECC Entity Model ID is based on the J.D. Koftinoff Software, Ltd. assigned MAC-S (OUI36): 70:b3:d5:ed:c
jdksavdecc_eui64 my_entity_model_id = { {0x70, 0xb3, 0xd5, 0xed, 0xc0, 0x00, 0x00, 0x00 } };

/// All of the controls are targetting the entity of the second JDKS Experimental MAC address
jdksavdecc_eui64 target_entity_id = { {0x70, 0xb3, 0xd5, 0xff, 0xff, 0xed, 0xcf, 0xf1 } };

/// All of the controls are targetting the second JDKS experimental MAC address 70:b3:d5:ed:cf:f1
jdksavdecc_eui48 target_mac_address = { { 0x70, 0xb3, 0xd5, 0xed, 0xcf, 0xf1 } };

/// the W5100 chip Raw Ethernet handler object
WizNetIO rawnet(my_mac);

/// The ADP manager is told about the entity id, model_id, entity capabilities, controller capabilities, and valid time in seconds
ADPManager adp_manager( 
  rawnet, 
  my_entity_id, 
  my_entity_model_id, 
  JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED,
  JDKSAVDECC_ADP_CONTROLLER_CAPABILITY_IMPLEMENTED,
  20
  );

//ControllerEntity my_entity(adp_manager);

/// The shared sequence ID for transmitted messages
uint16_t sequence_id = 0;

/// The mapping of Knob 1 to control descriptor 0x0000. 2 byte payload, refresh time of 1000 ms
ControlSender knob1(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0000,
  2,
  REFRESH_TIME
  );

/// The mapping of Knob 2 to control descriptor 0x0001. 2 byte payload, refresh time of 1000 ms
ControlSender knob2(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0001,
  2,
  REFRESH_TIME
  );

/// The mapping of Knob 2 to control descriptor 0x0002. 2 byte payload, refresh time of 1000 ms
ControlSender knob3(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0002,
  2,
  REFRESH_TIME
  );

/// The mapping of Button 1 to control descriptor 0x0003. 2 byte payload, refresh time of 1000 ms
ControlSender button1(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0003,
  1,
  REFRESH_TIME
  );

/// The mapping of Button 2 to control descriptor 0x0004. 2 byte payload, refresh time of 1000 ms
ControlSender button2(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0004,
  1,
  REFRESH_TIME
  );

/// The mapping of Button 3 to control descriptor 0x0005. 2 byte payload, refresh time of 1000 ms
ControlSender button3(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0005,
  1,
  REFRESH_TIME
  );

/// The mapping of Button 3 to control descriptor 0x0006. 2 byte payload, refresh time of 1000 ms
ControlSender button4(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0006,
  1,
  REFRESH_TIME
  );

/// The mapping of Button 3 to control descriptor 0x0007. 2 byte payload, refresh time of 1000 ms
ControlSender button5(
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0007,
  1,
  REFRESH_TIME
  );


/// The ValueMapper scans the A/D's and Digital input pins at the specified rate
/// and applies the new values to the appropriate knob or button
class ValueMapper : public Handler {
private:
  /// The update rate in milliseconds
  uint32_t m_update_rate_in_millis;
  
  /// The time that the last update happened
  uint32_t m_last_update_time;
public:  

  ValueMapper( uint32_t update_rate_in_millis ) 
  : m_update_rate_in_millis(update_rate_in_millis) 
  , m_last_update_time(0) {
  }
  
  virtual void Tick( uint32_t time_in_millis ) {
    if( time_in_millis > m_last_update_time + m_update_rate_in_millis ) {
      m_last_update_time = time_in_millis;
      
      knob1.SetValueDoublet( analogRead(0)); // A/D values range from 0 to 0x3ff
      knob2.SetValueDoublet( analogRead(1));
      knob3.SetValueDoublet( analogRead(2));
      
      button1.SetValueOctet( digitalRead(2) ? 0x00 : 0xff ); // DLI reads true when not pressed, reverse logic
      button2.SetValueOctet( digitalRead(3) ? 0x00 : 0xff );
      button3.SetValueOctet( digitalRead(4) ? 0x00 : 0xff );
      button4.SetValueOctet( digitalRead(5) ? 0x00 : 0xff );  
      button5.SetValueOctet( digitalRead(6) ? 0x00 : 0xff );  
  
      digitalWrite( A5, !digitalRead(2) );
      digitalWrite( A4, !digitalRead(3) );      
    }
  }
};

/// Create the mapper which polls at 50 ms
ValueMapper value_mapper(50);

/// Create a HandlerGroup which can manage up to 16 handlers
HandlerGroup<16> all_handlers;

void setup() {
  // Set up the I/O pins for 3 knobs, 5 buttons, and 2 LED's
  pinMode(9,OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);

  // Initialize the serial port for debug logs  
  Serial.begin(9600);
  
  // Initialize the W5100 chip 
  rawnet.Initialize();
  
  // Put all the handlers into the HandlerGroup
  
  all_handlers.Add( &value_mapper );
  all_handlers.Add( &adp_manager );
  all_handlers.Add( &knob1 );
  all_handlers.Add( &knob2 );
  all_handlers.Add( &knob3 );
  all_handlers.Add( &button1 );
  all_handlers.Add( &button2 );
  all_handlers.Add( &button3 );  
  all_handlers.Add( &button4 );
  all_handlers.Add( &button5 );
//  all_handlers.Add( &my_entity );
}

extern "C" {
void avr_debug_log(const char *str, uint16_t v ) {
  char txt[64];
  char pdu[256];
  static uint16_t logging_sequence_id=0;
  uint16_t r;
  sprintf( txt, "%s %u", str, (unsigned)v );
  r=jdksavdecc_jdks_log_control_generate(
        &my_entity_id,
        8, // Control index 8
        &sequence_id,
        &logging_sequence_id,
        JDKSAVDECC_JDKS_LOG_INFO,
        0,
        txt,
        pdu,
        14,
        sizeof(pdu));
  if( r>0 )
  {
      memcpy(pdu+JDKSAVDECC_FRAME_HEADER_SA_OFFSET,my_mac.value,6);    
      net->SendRawNet((uint8_t*)pdu,r);
  }
  else {
    Serial.println(" error ");    
  }
  
#if 0  
  Serial.print(str);
  Serial.print(" ");
  Serial.println(v);
#endif
}
}

void loop() {
  static uint32_t last_second=0;
  // Get the current time in milliseconds
  uint32_t cur_time = millis();
  
  // Tell all the handlers to do their periodic jobs
  all_handlers.Tick(cur_time);
  if( cur_time/1000 != last_second ) {
    last_second = cur_time/1000;
    avr_debug_log("Time from KnobsAndButtons is:",last_second);
  }
}

