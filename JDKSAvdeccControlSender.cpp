#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccControlSender.h"

namespace JDKSAvdecc {

ControlSender::ControlSender(
                 jdksavdecc_eui64 const &entity_id,
                 jdksavdecc_eui64 const &target_entity_id,
                 jdksavdecc_eui48 const &target_mac_address,
                 uint16_t &sequence_id,
                 uint16_t target_descriptor_index,
                 uint16_t value_length,
                 uint32_t update_rate_in_millis
                 )
: m_entity_id( entity_id )
, m_target_entity_id( target_entity_id )
, m_target_mac_address( target_mac_address )
, m_sequence_id( sequence_id )
, m_target_descriptor_index( target_descriptor_index )
, m_value_length(value_length)
, m_update_rate_in_millis(update_rate_in_millis)
, m_next_send_time_millis(0) {
    m_value[0] = 0;
    m_value[1] = 0;
    m_value[2] = 0;
    m_value[3] = 0;
}

void ControlSender::SetValueOctet( uint8_t val ) {
    if( val!=m_value[0] ) {
        m_next_send_time_millis = 0;
        m_value[0] = val;
    }
}

/// Set a doublet value. If it actually changed, then force Tick to send ASAP
void ControlSender::SetValueDoublet( uint16_t val ) {
    if( (((val>>8)&0xff) !=m_value[0])
       || (((val>>0)&0xff)!=m_value[1]) ) {
        m_next_send_time_millis = 0;
        m_value[0] = (val>>8)&0xff;
        m_value[1] = (val>>0)&0xff;
    }
}

/// Set a quadlet value. If it actually changed, then force Tick to send ASAP
void ControlSender::SetValueQuadlet( uint32_t val ) {
    if( (((val>>24)&0xff) !=m_value[0])
       || (((val>>16)&0xff)!=m_value[1])
       || (((val>>8)&0xff) !=m_value[2])
       || (((val>>0)&0xff)!=m_value[3]) ) {

        m_next_send_time_millis = 0;
        m_value[0] = (val>>24)&0xff;
        m_value[1] = (val>>16)&0xff;
        m_value[2] = (val>>8)&0xff;
        m_value[3] = (val>>0)&0xff;
    }
}

void ControlSender::Tick( uint32_t time_in_millis ) {
    if( m_next_send_time_millis < time_in_millis ) {
        SendSetControl();
        m_next_send_time_millis = time_in_millis + m_update_rate_in_millis;
    }
}

void ControlSender::SendSetControl() {
    Frame<128> pdu(m_target_mac_address,net->GetMACAddress(),JDKSAVDECC_AVTP_ETHERTYPE);  // DA, SA, EtherType, ADPDU

    // AECPDU common control header
    pdu.PutOctet( 0x80 + JDKSAVDECC_SUBTYPE_AECP); // cd=1, subtype=0x7b (AECP)
    pdu.PutOctet( 0x00 + JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );  // sv=0, version=0, message_type = AEM_COMMAND
    pdu.PutOctet( ((JDKSAVDECC_AEM_STATUS_SUCCESS) << 3) + 0 ); // Send success code. top 3 bits of control_data_length is 0
    pdu.PutOctet( JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + m_value_length ); // control_data_length field is N + value_length

    pdu.PutEUI64( m_target_entity_id ); // entity_id of the device we are setting
    pdu.PutEUI64( m_entity_id ); // controller_id

    pdu.PutDoublet( m_sequence_id );
    pdu.PutDoublet( JDKSAVDECC_AEM_COMMAND_SET_CONTROL );
    pdu.PutDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
    pdu.PutDoublet( m_target_descriptor_index );
    pdu.PutBuf( m_value, m_value_length );

    net->SendRawNet( pdu.GetBuf(), pdu.GetLength() );
    m_sequence_id++;
}

bool ControlSender::ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
    return false;
}


}
