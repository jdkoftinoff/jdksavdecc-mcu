#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"
#include "JDKSAvdeccFrame.h"

#include "JDKSAvdeccHandler.h"

namespace JDKSAvdecc {

class ADPManager : public Handler {
public:

    /// Construct the ADPManager object
    ADPManager(NetIO &net,
               jdksavdecc_eui64 const &entity_id,
               jdksavdecc_eui64 const &entity_model_id,
               uint32_t entity_capabilities,
               uint32_t controller_capabilities,
               uint16_t valid_time_in_seconds );

    /// Send the ENTITY_AVAILABLE message if it is time to
    virtual void Tick( uint32_t time_in_millis );

    /// Handle any incoming ADPDU. Return true if handled
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );

    /// Formulate the ADPDU and send it
    void SendADP();

    jdksavdecc_eui64 const &GetEntityID() const { return m_entity_id; }
    jdksavdecc_eui64 const &GetEntityModelID() const { return m_entity_model_id; }
    uint32_t GetEntityCapabilities() const { return m_entity_capabilities; }
    uint32_t GetControllerCapabilities() const { return m_controller_capabilities; }
    uint16_t GetValidTimeInSeconds() const { return m_valid_time_in_seconds; }
    uint32_t GetAvailableIndex() const { return m_available_index; }

protected:
    NetIO &m_net;
    jdksavdecc_eui64 m_entity_id;
    jdksavdecc_eui64 m_entity_model_id;
    uint32_t m_entity_capabilities;
    uint32_t m_controller_capabilities;
    uint16_t m_valid_time_in_seconds;
    uint32_t m_available_index;
    uint32_t m_next_send_time_millis;
};

}
