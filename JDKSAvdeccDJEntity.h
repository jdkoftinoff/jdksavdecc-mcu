#pragma once

#include "JDKSAvdeccEntity.h"

namespace JDKSAvdecc {

class DJEntity : public Entity {
public:

    DJEntity();
    jdksavdecc_eui64 ReadEntityModelID();
    jdksavdecc_eui48 ReadEntityMacAddress();
    ControlInfo GetControlInfo(uint16_t descriptor_index);
    ControlInfo SetControlInfoFromPacket();

    bool Poll();
    bool PollEthernet();

    void SendRawPacket();
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

    void PollADC();
    void PollGPIO();
    void SendPollForLEDs();

};

}

