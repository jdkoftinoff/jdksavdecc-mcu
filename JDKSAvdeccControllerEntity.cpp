
#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccControllerEntity.h"

namespace JDKSAvdecc {

bool ControllerEntity::ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
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
            } else if( IsAEMForController(aem,m_adp_manager.GetEntityID()) ) {
                r=ReceivedAEMResponse(aem,buf,pos,len);
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


bool ControllerEntity::ReceivedAEMResponse(  jdksavdecc_aecpdu_aem const &aem, uint8_t *buf, uint16_t pos, uint16_t len ) {
    bool r=false;
    // figure out if it is unsolicited from the high bit of command_type
    uint16_t actual_command_type = aem.command_type&0x7fff;
    bool unsolicited=(aem.command_type>>15)&1;

    // only bother with the response if it is either unsolicited,
    // or is solicited and matches the last request we did send
    bool interesting = unsolicited;

    if( !unsolicited ) {
        // First, is it from the entity we sent the command to?
        if( jdksavdecc_eui64_compare(
                    &m_last_sent_command_target_entity_id, &aem.aecpdu_header.header.target_entity_id )==0 ) {
            // yes, does the command type match?
            if( actual_command_type == m_last_sent_command_type ) {
                // yes, does the sequence ID match?
                if( aem.sequence_id == m_outgoing_sequence_id ) {
                    // Yes, then we are interested in this message
                    interesting = true;
                    // forget about the sent state by clearing the last send command target entity id
                    jdksavdecc_eui64_init(&m_last_sent_command_target_entity_id);
                }
            }
        }
    }

    // If this message is interesting to us then dispatch it
    if( interesting ) {
        switch(actual_command_type) {
        case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
            r=ReceiveAcquireEntityResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
            r=ReceiveLockEntityResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
            r=ReceiveControllerAvailableResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
            r=ReceiveEntityAvailableResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
            r=ReceiveReadDescriptorResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION:
            r=ReceiveSetConfigurationResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION:
            r=ReceiveGetConfigurationResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_NAME:
            r=ReceiveSetNameResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_NAME:
            r=ReceiveGetNameResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
            r=ReceiveSetControlResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONTROL:
            r=ReceiveGetControlResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION:
            r=ReceiveRegisterUnsolicitedNotificationResponse(buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION:
            r=ReceiveDeRegisterUnsolicitedNotificationResponse(buf,pos,len);
            break;
        }
    }

    return r;
}

bool ControllerEntity::ReceivedAAResponse(  jdksavdecc_aecp_aa const &aa, uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveAcquireEntityResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveLockEntityResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveEntityAvailableResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveReadDescriptorResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveSetConfigurationResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveGetConfigurationResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}


bool ControllerEntity::ReceiveSetNameResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveGetNameResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}


bool ControllerEntity::ReceiveSetControlResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveGetControlResponse( uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

uint8_t ControllerEntity::ReceiveControlValue(
        jdksavdecc_eui64 const &target_entity_id,
        uint16_t target_descriptor_index,
        uint8_t const *control_value,
        uint16_t control_value_len
        ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


bool ControllerEntity::ReceiveRegisterUnsolicitedNotificationResponse(  uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveDeRegisterUnsolicitedNotificationResponse(  uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}


}

