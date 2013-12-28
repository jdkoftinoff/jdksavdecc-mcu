#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"
#include "JDKSAvdeccFrame.h"
#include "JDKSAvdeccHandler.h"

namespace JDKSAvdecc {

class ControlSender : public Handler {
public:

    /// Construct the SetControlSender object
    ControlSender(NetIO &net,
               jdksavdecc_eui64 const &entity_id,
               jdksavdecc_eui64 const &target_entity_id,
               jdksavdecc_eui48 const &target_mac_address,
               uint16_t &sequence_id,
               uint16_t target_descriptor_index,
               uint16_t value_length,
               uint32_t update_rate_in_millis);

    /// Send the SET_CONTROL message if it is time to
    virtual void Tick( uint32_t time_in_millis );

    /// Handle incoming PDU
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );

    /// Set a one byte value. If it actually changed, then force Tick to send ASAP
    void SetValueOctet( uint8_t val );

    /// Set a doublet value. If it actually changed, then force Tick to send ASAP
    void SetValueDoublet( uint16_t val );

    /// Set a quadlet value. If it actually changed, then force Tick to send ASAP
    void SetValueQuadlet( uint32_t val );


    /// Formulate the ADPDU and send it
    void SendSetControl();


    jdksavdecc_eui64 const &GetEntityID() const { return m_entity_id; }
    jdksavdecc_eui64 const &GetTargetEntityID() const { return m_target_entity_id; }
    jdksavdecc_eui48 const &GetTargetMACAddress() const { return m_target_mac_address; }
    uint16_t GetSequenceID() const { return m_sequence_id; }


protected:
    NetIO &m_net;
    jdksavdecc_eui64 m_entity_id;
    jdksavdecc_eui64 m_target_entity_id;
    jdksavdecc_eui48 m_target_mac_address;
    uint16_t &m_sequence_id;
    uint16_t m_target_descriptor_index;
    uint16_t m_value_length;
    uint8_t m_value[4];
    uint32_t m_update_rate_in_millis;
    uint32_t m_next_send_time_millis;
};

}
