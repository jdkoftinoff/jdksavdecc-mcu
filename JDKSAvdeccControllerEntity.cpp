/*
  Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/


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
            r=ReceiveAcquireEntityResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
            r=ReceiveLockEntityResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
            r=ReceiveControllerAvailableResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
            r=ReceiveEntityAvailableResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
            r=ReceiveReadDescriptorResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION:
            r=ReceiveSetConfigurationResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION:
            r=ReceiveGetConfigurationResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_NAME:
            r=ReceiveSetNameResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_NAME:
            r=ReceiveGetNameResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
            r=ReceiveSetControlResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONTROL:
            r=ReceiveGetControlResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION:
            r=ReceiveRegisterUnsolicitedNotificationResponse(aem,buf,pos,len);
            break;
        case JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION:
            r=ReceiveDeRegisterUnsolicitedNotificationResponse(aem,buf,pos,len);
            break;
        }
    }

    return r;
}

bool ControllerEntity::ReceivedAAResponse( jdksavdecc_aecp_aa const &aa, uint8_t *buf, uint16_t pos, uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveAcquireEntityResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveLockEntityResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveEntityAvailableResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveReadDescriptorResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveSetConfigurationResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveGetConfigurationResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}


bool ControllerEntity::ReceiveSetNameResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveGetNameResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}


bool ControllerEntity::ReceiveSetControlResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveGetControlResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
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


bool ControllerEntity::ReceiveRegisterUnsolicitedNotificationResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}

bool ControllerEntity::ReceiveDeRegisterUnsolicitedNotificationResponse(
        jdksavdecc_aecpdu_aem const &aem,
        uint8_t *buf,
        uint16_t pos,
        uint16_t len ) {
    return false;
}


}

