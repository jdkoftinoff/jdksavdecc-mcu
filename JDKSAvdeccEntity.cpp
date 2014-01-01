
#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccEntity.h"

namespace JDKSAvdecc {

Entity::Entity( ADPManager &adp_manager )
    : m_adp_manager( adp_manager )
    , m_last_sent_command_time(0)
    , m_last_sent_command_type(JDKSAVDECC_AEM_COMMAND_EXPANSION) {
    memset(&m_last_sent_command_target_entity_id.value,0,sizeof(m_last_sent_command_target_entity_id.value) );
}

void Entity::Tick( uint32_t time_in_millis ) {
    m_cur_time = time_in_millis;
    uint16_t cmd = m_last_sent_command_type;
    if( cmd != JDKSAVDECC_AEM_COMMAND_EXPANSION &&
            WasTimeOutHit( time_in_millis, m_last_sent_command_time, JDKSAVDECC_AEM_TIMEOUT_IN_MS ) ) {
        m_last_sent_command_type=JDKSAVDECC_AEM_COMMAND_EXPANSION; // clear knowledge of sent command
        CommandTimedOut(m_last_sent_command_target_entity_id,cmd,m_outgoing_sequence_id);
    }
}

void Entity::CommandTimedOut( jdksavdecc_eui64 const &target_entity_id,
                              uint16_t command_type,
                              uint16_t sequence_id) {
//    avr_debug_log("TimeOut:", sequence_id );
    memset( &m_last_sent_command_target_entity_id,0,sizeof(m_last_sent_command_target_entity_id));
}

bool Entity::ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
    bool r=false;
    m_cur_time = time_in_millis;

    // we already know the message is AVTP ethertype and is either directly
    // targetting my MAC address or is a multicast message

    // DA (6), SA (6), Ethertype (2) brings us to subtype byte at 14
    ssize_t pos=JDKSAVDECC_FRAME_HEADER_LEN;

    {
        // Try see if it is an AEM message
        jdksavdecc_aecpdu_aem aem;
        if( ParseAEM(&aem,buf,pos,len)) {
            if( IsAEMForTarget(aem,m_adp_manager.GetEntityID())) {
                uint8_t response_code=ReceivedAEMCommand(aem,buf,pos,len);
                r=true;
            }
        }
    }


    if( !r ) {
        // Try see if it is an Address Access message
        jdksavdecc_aecp_aa aa;
        if( ParseAA(&aa,buf,pos,len)) {
            // Yes, is it a command to read/write data?
            if( IsAAForTarget(aa,m_adp_manager.GetEntityID())) {
                uint8_t response_code=ReceivedAACommand(aa,buf,pos,len);
                r=true;
                // TODO: fill in response code in PDU and send reply
            }
        }
    }

    return r;
}

uint8_t Entity::ReceivedAEMCommand(  jdksavdecc_aecpdu_aem const &aem, uint8_t *buf, uint16_t pos, uint16_t len ) {
    uint16_t actual_command_type = aem.command_type&0x7fff;
    uint8_t response_status = JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
    bool command_is_set_something=false;

    switch(actual_command_type) {
    case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
        response_status=ReceiveAcquireEntityCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
        response_status=ReceiveLockEntityCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
        response_status=ReceiveControllerAvailableCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
        response_status=ReceiveEntityAvailableCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
        response_status=ReceiveReadDescriptorCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION:
        response_status=ReceiveSetConfigurationCommand(buf,pos,len);
        command_is_set_something=true;
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION:
        response_status=ReceiveGetConfigurationCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_NAME:
        response_status=ReceiveSetNameCommand(buf,pos,len);
        command_is_set_something=true;
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_NAME:
        response_status=ReceiveGetNameCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
        response_status=ReceiveSetControlCommand(buf,pos,len);
        command_is_set_something=true;
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_CONTROL:
        response_status=ReceiveGetControlCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION:
        response_status=ReceiveRegisterUnsolicitedNotificationCommand(buf,pos,len);
        break;
    case JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION:
        response_status=ReceiveDeRegisterUnsolicitedNotificationCommand(buf,pos,len);
        break;
    }

    // Send the response to either just the requesting controller or it and all registered controllers
    buf[JDKSAVDECC_FRAME_HEADER_LEN + 2 ] = (buf[JDKSAVDECC_FRAME_HEADER_LEN + 2 ]&0x7) + (response_status<<3);

    SendResponses(
                false,
                command_is_set_something && response_status==JDKSAVDECC_AECP_STATUS_SUCCESS,
                buf,
                len);

    return response_status;
}



uint8_t Entity::ReceivedAACommand(  jdksavdecc_aecp_aa const &aa, uint8_t *buf, uint16_t pos, uint16_t len ) {
    // Yes go through the TLV's and dispatch the read/writes and respond
    uint8_t *p = &buf[pos+JDKSAVDECC_AECPDU_AA_LEN];
    uint8_t aa_status=JDKSAVDECC_AECP_AA_STATUS_NOT_IMPLEMENTED;
    for( uint16_t i=0; i<aa.tlv_count; ++i ) {
        // See 9.2.1.3.3
        uint8_t tlv_mode = (p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_MODE_LENGTH]>>4)&0xf;
        uint16_t tlv_length = (((uint16_t)(p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_MODE_LENGTH]&0xf))<<4)
                + p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_MODE_LENGTH+1];
        // require top 32 bits of address to be zero
        if( p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER]==0
                && p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER+1]==0
                && p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER+2]==0
                && p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER+3]==0) {
            uint32_t tlv_address = jdksavdecc_uint32_get( buf, JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_LOWER );

            switch( tlv_mode ) {
            case JDKSAVDECC_AECP_AA_MODE_READ:
                aa_status=ReceiveAARead(tlv_address,tlv_length,p+JDKSAVDECC_AECPDU_AA_TLV_LEN);
                break;
            case JDKSAVDECC_AECP_AA_MODE_WRITE:
                aa_status=ReceiveAAWrite(tlv_address,tlv_length,p+JDKSAVDECC_AECPDU_AA_TLV_LEN);
                break;
            case JDKSAVDECC_AECP_AA_MODE_EXECUTE:
                aa_status=ReceiveAAExecute(tlv_address,tlv_length,p+JDKSAVDECC_AECPDU_AA_TLV_LEN);
                break;
            }
        }
        p=p+JDKSAVDECC_AECPDU_AA_TLV_LEN+tlv_length;
        if( aa_status!=JDKSAVDECC_AECP_AA_STATUS_SUCCESS ) {
            break;
        }
    }
    // Send the response to either just the requesting controller or it and all registered controllers
    buf[JDKSAVDECC_FRAME_HEADER_LEN + 2 ] = (buf[JDKSAVDECC_FRAME_HEADER_LEN + 2 ]&0x7) + (aa_status<<3);

    // Only send responses to the requesting controller
    SendResponses(false,false,buf,pos);

    return aa_status;
}

void Entity::SendResponses( bool internally_generated,
                            bool send_to_registered_controllers,
                            uint8_t *buf,
                            uint16_t len,
                            uint8_t const *additional_data1,
                            uint16_t additional_data_length1,
                            uint8_t const *additional_data2,
                            uint16_t additional_data_length2 ) {

    // buf already contains the requestor's mac address and controller. Capture the controller entity id
    jdksavdecc_eui64 original_controller_id;
    jdksavdecc_eui64_init(&original_controller_id);

    if( !internally_generated ) {
        // The message is not internally generated, it is a response to a real request
        // extract the actually requested controller entity id from the frame.
        jdksavdecc_eui64_get(buf,14+JDKSAVDECC_AECPDU_COMMON_OFFSET_CONTROLLER_ENTITY_ID);
        // Send the buf to the original
        net->SendReplyRawNet(buf,len);
    } else {
        // Read my source MAC address manually
        jdksavdecc_eui48_set(net->GetMACAddress(),buf,6);
    }

    if( send_to_registered_controllers ) {
        // Mark the message as an unsolicited response
        jdksavdecc_aecpdu_aem_set_command_type(
                    jdksavdecc_aecpdu_aem_get_command_type(buf,14) | 0x8000,
                    buf, 14 );

        // Now go through all subscribed entities
        for( uint8_t i=0; i<JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS; ++i ) {
            // We only care about controllers with entity_id != 0

            if( !jdksavdecc_eui64_is_zero( m_registered_controllers_entity_id[i] ) ) {
                // and don't send the original requesting controller a double response
                if( jdksavdecc_eui64_compare( &original_controller_id, &m_registered_controllers_entity_id[i] )!=0 ) {
                    // Set the controller_entity_id in the frame
                    jdksavdecc_eui64_set(m_registered_controllers_entity_id[i],buf,14+JDKSAVDECC_AECPDU_COMMON_OFFSET_CONTROLLER_ENTITY_ID);
                    // Set the destination mac address
                    jdksavdecc_eui48_set(m_registered_controllers_mac_address[i],buf,0);
                    // Send the frame to that controller
                    net->SendRawNet(buf,len);
                }
            }
        }
    }
}

void Entity::SendCommand( jdksavdecc_eui64 const &target_entity_id,
                          jdksavdecc_eui48 const &target_mac_address,
                          uint16_t aem_command_type,
                          bool track_for_ack,
                          uint8_t const *additional_data1,
                          uint16_t additional_data_length1,
                          uint8_t const *additional_data2,
                          uint16_t additional_data_length2 ) {

    // Make a temp pdu buffer just long enough to contain:
    // ethernet frame DA,SA,Ethertype, AVTP Common Control Header, AVDECC AEM Common Format.
    Frame<JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AECPDU_AEM_LEN>
            pdu(target_mac_address,net->GetMACAddress(),JDKSAVDECC_AVTP_ETHERTYPE);

    // control_data_length field is N + value_length
    uint16_t control_data_length = JDKSAVDECC_AECPDU_AEM_LEN -
            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + additional_data_length1 + additional_data_length2;

    // AECPDU common control header
    pdu.PutOctet( 0x80 + JDKSAVDECC_SUBTYPE_AECP); // cd=1, subtype=0x7b (AECP)
    pdu.PutOctet( 0x00 + JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );  // sv=0, version=0, message_type = AEM_COMMAND
    pdu.PutOctet( ((JDKSAVDECC_AEM_STATUS_SUCCESS) << 3) + ((control_data_length>>8)&0x7) ); // Send success code. top 3 bits of control_data_length
    pdu.PutOctet( control_data_length & 0xff ); // lower 8 bits of control_data_length

    pdu.PutEUI64( target_entity_id ); // entity_id of the device we are setting
    pdu.PutEUI64( m_adp_manager.GetEntityID() ); // controller_id

    // increment outoging sequence id before sending it, so when we receive a response we know what sequence id to expect
    // there are no inflight messages here
    ++m_outgoing_sequence_id;
    pdu.PutDoublet( m_outgoing_sequence_id );
    pdu.PutDoublet( aem_command_type );

    // Send the header appended to any additional data
    net->SendRawNet( pdu.GetBuf(), pdu.GetLength(),
                     additional_data1, additional_data_length1,
                     additional_data2, additional_data_length2);

    if( track_for_ack ) {
        // Keep track of when we sent this message and who we sent it to so we can
        // manage time outs
        m_last_sent_command_time = m_cur_time;
        m_last_sent_command_type = aem_command_type;
        m_last_sent_command_target_entity_id = target_entity_id;
    }
}

void Entity::SendUnsolicitedResponses(
                          uint16_t aem_command_type,
                          uint8_t const *additional_data1,
                          uint16_t additional_data_length1,
                          uint8_t const *additional_data2,
                          uint16_t additional_data_length2 ) {

    // Make a temp pdu buffer just long enough to contain:
    // ethernet frame DA,SA,Ethertype, AVTP Common Control Header, AVDECC AEM Common Format.
    Frame<JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AECPDU_AEM_LEN>
            pdu(net->GetMACAddress(),net->GetMACAddress(),JDKSAVDECC_AVTP_ETHERTYPE);

    // control_data_length field is N + value_length
    uint16_t control_data_length = JDKSAVDECC_AECPDU_AEM_LEN -
            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + additional_data_length1 + additional_data_length2;

    // AECPDU common control header
    pdu.PutOctet( 0x80 + JDKSAVDECC_SUBTYPE_AECP); // cd=1, subtype=0x7b (AECP)
    pdu.PutOctet( 0x00 + JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );  // sv=0, version=0, message_type = AEM_COMMAND
    pdu.PutOctet( ((JDKSAVDECC_AEM_STATUS_SUCCESS) << 3) + ((control_data_length>>8)&0x7) ); // Send success code. top 3 bits of control_data_length
    pdu.PutOctet( control_data_length & 0xff ); // lower 8 bits of control_data_length

    pdu.PutEUI64( m_adp_manager.GetEntityID() ); // entity_id of the device we are setting (placeholder)
    pdu.PutEUI64( m_adp_manager.GetEntityID() ); // controller_id

    // increment outoging sequence id before sending it, so when we receive a response we know what sequence id to expect
    // there are no inflight messages here
    ++m_outgoing_sequence_id;
    pdu.PutDoublet( m_outgoing_sequence_id );
    pdu.PutDoublet( aem_command_type );

    SendResponses(true,true,
                  (uint8_t*)pdu.GetBuf(),pdu.GetLength(),
                  additional_data1, additional_data_length1,
                  additional_data2, additional_data_length2);

}

uint8_t Entity::ReceiveAcquireEntityCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveLockEntityCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveEntityAvailableCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveControllerAvailableCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

bool Entity::ReceiveControllerAvailableResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

uint8_t Entity::ReceiveReadDescriptorCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveSetConfigurationCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveGetConfigurationCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveSetNameCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveGetNameCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveSetControlCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveGetControlCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveRegisterUnsolicitedNotificationCommand(  uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveDeRegisterUnsolicitedNotificationCommand( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReadDescriptorEntity( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorConfiguration( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorControl( uint8_t *buf, uint16_t pos, uint16_t len )  {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorLocale( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorString( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorMemory( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveAARead( uint32_t virtual_base_address, uint16_t length, uint8_t *response ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveAAWrite( uint32_t virtual_base_address, uint16_t length, uint8_t const *request ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveAAExecute( uint32_t virtual_base_address, uint16_t length, uint8_t const *request ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

}

