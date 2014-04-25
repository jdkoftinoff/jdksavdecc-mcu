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


#include "JDKSAvdeccWorld.hpp"

#include "JDKSAvdeccControllerEntity.hpp"

namespace JDKSAvdecc {

bool ControllerEntity::ReceivedPDU( jdksavdecc_timestamp_in_milliseconds time_in_millis, uint8_t *buf, uint16_t len ) {
    bool r=false;
    bool done=false;

    // we already know the message is AVTP ethertype and is either directly
    // targetting my MAC address or is a multicast message
    FrameBase pdu(time_in_millis,buf,len);
    {
        // Try see if it is an AEM message
        jdksavdecc_aecpdu_aem aem;
        if( ParseAEM(&aem,pdu)) {
            if( IsAEMForTarget(aem,m_adp_manager.GetEntityID())) {
                ReceivedAEMCommand(aem,pdu);
                r=true;
            } else if( IsAEMForController(aem,m_adp_manager.GetEntityID()) ) {
                r=ReceivedAEMResponse(aem,pdu);
            }
            done=true;
        }
    }


    if( !done ) {
        // Try see if it is an Address Access message
        jdksavdecc_aecp_aa aa;
        memset(&aa,0,sizeof(aa));
        if( ParseAA(&aa,pdu)) {
            // Yes, is it a command to read/write data?
            if( IsAAForTarget(aa,m_adp_manager.GetEntityID())) {
                ReceivedAACommand(aa,pdu);
                r=true;
                // TODO: fill in response code in PDU and send reply
            }
        }
    }

    return r;
}


bool ControllerEntity::ReceivedAEMResponse(
            jdksavdecc_aecpdu_aem const &aem,
            FrameBase &pdu ) {
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
                if( aem.aecpdu_header.sequence_id == m_outgoing_sequence_id ) {
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
            r=ReceiveAcquireEntityResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
            r=ReceiveLockEntityResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
            r=ReceiveControllerAvailableResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
            r=ReceiveEntityAvailableResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
            r=ReceiveReadDescriptorResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION:
            r=ReceiveSetConfigurationResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION:
            r=ReceiveGetConfigurationResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_NAME:
            r=ReceiveSetNameResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_NAME:
            r=ReceiveGetNameResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
            r=ReceiveSetControlResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONTROL:
            r=ReceiveGetControlResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION:
            r=ReceiveRegisterUnsolicitedNotificationResponse(aem,pdu);
            break;
        case JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION:
            r=ReceiveDeRegisterUnsolicitedNotificationResponse(aem,pdu);
            break;
        }
    }

    return r;
}

bool ControllerEntity::ReceivedAAResponse(
        jdksavdecc_aecp_aa const &aa,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveAcquireEntityResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveLockEntityResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveEntityAvailableResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveReadDescriptorResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveSetConfigurationResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveGetConfigurationResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}


bool ControllerEntity::ReceiveSetNameResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveGetNameResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}


bool ControllerEntity::ReceiveSetControlResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveGetControlResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
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
        FrameBase &pdu ) {
    return false;
}

bool ControllerEntity::ReceiveDeRegisterUnsolicitedNotificationResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    return false;
}


}

