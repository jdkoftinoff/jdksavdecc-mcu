#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"

namespace JDKSAvdecc {

class ADPManager {
public:
    ADPManager(NetIO &net,
               jdksavdecc_eui64 const &entity_id,
               jdksavdecc_eui64 const &entity_model_id )
               : m_net( net )
               , m_mac_address( mac_address )
               , m_entity_id( entity_id )
               , m_entity_model_id( entity_model_id ) {}

    virtual void SendADP();

protected:
    NetIO &m_net;
    jdksavdecc_eui64 m_acquired_by_controller_entity_id;
    uint32_t m_available_index;
    uint8_t m_raw_packet[640];
    uint16_t m_raw_packet_len;
};

}
