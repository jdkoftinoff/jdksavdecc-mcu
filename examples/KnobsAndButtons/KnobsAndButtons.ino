#include "JDKSAvdeccWorld.h"
#include "Ethernet.h"
#include "SPI.h"
#include "JDKSAvdeccWizNetIO.h"
#include "JDKSAvdeccFrame.h"
#include "JDKSAvdeccADPManager.h"
#include "JDKSAvdeccSetControlSender.h"

using namespace JDKSAvdecc;

jdksavdecc_eui48 my_mac = { { 0x90, 0xe0, 0xf0, 0x00, 0x00, 0x01 } };
jdksavdecc_eui64 my_entity_id = { {0x90, 0xe0, 0xf0, 0xff, 0xfe, 0x00, 0x01 } };
jdksavdecc_eui64 my_entity_model_id = { {0x90, 0xe0, 0xf0, 0xff, 0xfe, 0x00, 0x02 } };

WizNetIO rawnet(my_mac);

ADPManager adp_manager( 
  rawnet, 
  my_entity_id, 
  my_entity_model_id, 
  JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED,
  JDKSAVDECC_ADP_CONTROLLER_CAPABILITY_IMPLEMENTED,
  20
  );

jdksavdecc_eui64 target_entity_id = { { 0x90, 0xe0, 0xf0, 0xff, 0xfe, 0x00, 0x00, 0x02 } };
jdksavdecc_eui48 target_mac_address = { { 0x90, 0xe0, 0xf0, 0x00, 0x00, 0x02 } };
uint16_t sequence_id = 0;

SetControlSender knob1(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0000,
  2,
  1000
  );

SetControlSender knob2(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0001,
  2,
  1000
  );

SetControlSender knob3(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0002,
  2,
  1000
  );

SetControlSender button1(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0003,
  1,
  1000
  );

SetControlSender button2(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0004,
  1,
  1000
  );

SetControlSender button3(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0005,
  1,
  1000
  );

SetControlSender button4(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0006,
  1,
  1000
  );

SetControlSender button5(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0007,
  1,
  1000
  );

void setup() {
  pinMode(9,OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);
  rawnet.Initialize();
}



void loop() {
  uint32_t cur_time = millis();
  static uint32_t last_poll_switch_time = 0;
  
  // Is it time to poll the switches and knobs?
  if( cur_time-last_poll_switch_time > 50 ) {  
    last_poll_switch_time=cur_time;
    
    // Yes, read the knobs and swithes and tell the SetControlSender objects about the new values
    knob1.SetValueDoublet( analogRead(0) );
    knob2.SetValueDoublet( analogRead(1) );
    knob3.SetValueDoublet( analogRead(2) );
    button1.SetValueOctet( digitalRead(2) ? 0x00 : 0xff );
    button2.SetValueOctet( digitalRead(3) ? 0x00 : 0xff );
    button3.SetValueOctet( digitalRead(4) ? 0x00 : 0xff );
    button4.SetValueOctet( digitalRead(5) ? 0x00 : 0xff );  
    button5.SetValueOctet( digitalRead(6) ? 0x00 : 0xff );  
  }
  
  // Send any ADP messages  
  adp_manager.Tick( cur_time );
  
  // Send any SET_CONTROL messages
  knob1.Tick( cur_time );
  knob2.Tick( cur_time );
  knob3.Tick( cur_time );
  button1.Tick( cur_time );
  button2.Tick( cur_time );
  button3.Tick( cur_time );
  button4.Tick( cur_time );
  button5.Tick( cur_time );
  
  // Receive any ethernet frames and handle them  
  {
    // up to 640 bytes long
    uint8_t buf[640];
    uint16_t len;
    len = rawnet.ReceiveRawNet(buf,sizeof(buf));
    if( len>0 ) {
      
      bool handled=false;
      // Is it an AVTP/AVDECC message?
      if( (buf[12] == ((JDKSAVDECC_AVTP_ETHERTYPE>>8)&0xff))
        && (buf[13] == ((JDKSAVDECC_AVTP_ETHERTYPE>>0)&0xff))) 
        {          
          // Yes, dispatch ADP to the adp manager
          if( buf[14] == (0x80 + JDKSAVDECC_SUBTYPE_ADP) ) {
              handled = adp_manager.HandleIncomingADP( buf, len );
          }
          // dispatch AECP to the entity
          if( buf[14] == (0x80 + JDKSAVDECC_SUBTYPE_AECP) ) {
              // AECP command
          }          
          // And if we wanted to deal with ACMP, we would do that here.
          if( buf[14] == (0x80 + JDKSAVDECC_SUBTYPE_ACMP) ) {
              // ACMP message  
          }          
      }
    }      
  } 
}


