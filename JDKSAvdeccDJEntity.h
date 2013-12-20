#pragma once

#include "jdksavdecc-c/include/jdksavdecc.h"

#ifdef __avr__
#include "EEProm.h"
#endif

class JDKSAvdeccDJEntity {

    struct ControlInfo {
        jdksavdecc_eui64 target_entity_id;
        uint16_t target_descriptor_type;
        uint16_t target_descriptor_index;
        uint16_t target_command;
        uint16_t target_control_value_length;
        uint16_t target_control_value_type;
        uint32_t target_min;
        uint32_t target_max;
        uint32_t target_default;
    };
    struct EEPromStorage {
        uint32_t magic_number;
        jdksavdecc_eui64 entity_model_id;
        jdksavdecc_eui48 mac_address;
        jdksavdecc_string entity_name;
        ControlInfo controls[10];
    };

    JDKSAvdeccDJEntity();
    jdksavdecc_eui64 ReadEntityModelID();
    jdksavdecc_eui48 ReadEntityMacAddress();
    ControlInfo GetControlInfo(uint16_t descriptor_index);
    ControlInfo SetControlInfoFromPacket();
    
    bool Poll();
    void PollADC();
    void PollGPIO();
    bool PollEthernet();

    void SendRawPacket(uint16_t len);
    void SendADP();
    void SendControl(uint16_t descriptor_index, uint16_t control_value );
    void ReceiveLEDControl(uint16_t descriptor_index);

    void AcquireEntity();
    void ControllerAvailableResponse(); 

    void ReadDescriptor();
    void ReadDescriptorEntity();
    void ReadDescriptorConfiguration();
    void ReadDescriptorControl();
    void ReadDescriptorLocale();
    void ReadDescriptorStrings();

    void WriteDescriptorEntity();
    void WriteDescriptorControl();
    void SetName();
    void GetName();

    void SendPollForLEDs();
 
    jdksavdecc_eui64 acquired_by_controller_entity_id;
    uint32_t available_index;
    uint8_t raw_packet[640];
    uint16_t raw_packet_len;
};


