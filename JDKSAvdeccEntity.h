#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"

namespace JDKSAvdecc {

class Entity {
public:
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

    struct Storage {
        jdksavdecc_eui64 entity_model_id;
        jdksavdecc_eui48 mac_address;
        jdksavdecc_string entity_name;
    };

    Entity( NetIO &net ) : m_net( net ) {}

    jdksavdecc_eui64 ReadEntityModelID();
    jdksavdecc_eui48 ReadEntityMacAddress();
    virtual ControlInfo GetControlInfo(uint16_t descriptor_index);
    virtual ControlInfo SetControlInfoFromPacket();

    virtual bool Poll();
    virtual bool PollEthernet();

    virtual void SendRawPacket();
    virtual void SendADP();
    virtual void SendControlToTarget(uint16_t local_descriptor_index, uint16_t control_value );
    virtual void ReceiveControlFromTarget();
    virtual void SendRequestForControlToTarget(uint16_t local_descriptor_index);

    virtual void AcquireEntity();
    virtual void ControllerAvailableResponse();

    virtual void ReadDescriptor();
    virtual void ReadDescriptorEntity();
    virtual void ReadDescriptorConfiguration();
    virtual void ReadDescriptorControl();
    virtual void ReadDescriptorLocale();
    virtual void ReadDescriptorStrings();

    virtual void WriteDescriptorEntity();
    virtual void WriteDescriptorControl();
    virtual void SetName();
    virtual void GetName();

protected:
    NetIO &m_net;
    jdksavdecc_eui64 m_acquired_by_controller_entity_id;
    uint32_t m_available_index;
    uint8_t m_raw_packet[640];
    uint16_t m_raw_packet_len;
};

}
