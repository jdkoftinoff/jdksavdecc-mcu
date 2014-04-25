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

#include "JDKSAvdeccEntity.hpp"

namespace JDKSAvdecc {

Entity::Entity( ADPManager &adp_manager )
    : m_adp_manager( adp_manager )
    , m_outgoing_sequence_id(0)
    , m_acquire_in_progress_time(0)
    , m_locked_time(0)
    , m_last_sent_command_time(0)
    , m_last_sent_command_type(JDKSAVDECC_AEM_COMMAND_EXPANSION) {
    // clear info on sent command state
    jdksavdecc_eui64_init(&m_last_sent_command_target_entity_id);
    // clear info on acquired state
    jdksavdecc_eui64_init(&m_acquired_by_controller_entity_id);
    jdksavdecc_eui64_init(&m_acquire_in_progress_by_controller_entity_id);
    jdksavdecc_eui64_init(&m_locked_by_controller_entity_id);
    jdksavdecc_eui48_init(&m_acquired_by_controller_mac_address);
    for( uint8_t i=0; i<JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS; ++i ) {
        jdksavdecc_eui64_init(&m_registered_controllers_entity_id[i]);
        jdksavdecc_eui48_init(&m_registered_controllers_mac_address[i]);
    }
}

void Entity::Tick( jdksavdecc_timestamp_in_milliseconds time_in_millis ) {
    uint16_t cmd = m_last_sent_command_type;

    // If we are locked, then time out the lock
    if( jdksavdecc_eui64_is_set(m_locked_by_controller_entity_id) ) {
        if( WasTimeOutHit(time_in_millis,m_locked_time,JDKSAVDECC_AEM_LOCK_TIMEOUT_MS) ) {
            jdksavdecc_eui64_init(&m_locked_by_controller_entity_id);
        }
    }

    // Check to see if we had a command in flight that timed out
    if( cmd != JDKSAVDECC_AEM_COMMAND_EXPANSION &&
            WasTimeOutHit( time_in_millis, m_last_sent_command_time, JDKSAVDECC_AEM_TIMEOUT_IN_MS ) ) {
        m_last_sent_command_type=JDKSAVDECC_AEM_COMMAND_EXPANSION; // clear knowledge of sent command

        // Was the command a CONTROLLER_AVAILABLE? if so, handle it here
        if( cmd == JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE ) {
            // Yes, this means that the old controller goes away and the new one is approved
            m_acquired_by_controller_entity_id = m_acquire_in_progress_by_controller_entity_id;
            // Also clear any lock that may have been there
            jdksavdecc_eui64_init(&m_locked_by_controller_entity_id);
            // TODO: Formulate and send reply to the new controller
        } else {
            // Notify entity info about the timed out command
            CommandTimedOut(m_last_sent_command_target_entity_id,cmd,m_outgoing_sequence_id);
        }
    }
}

void Entity::CommandTimedOut(
        jdksavdecc_eui64 const &target_entity_id,
        uint16_t command_type,
        uint16_t sequence_id) {
    //    avr_debug_log("TimeOut:", sequence_id );
    memset( &m_last_sent_command_target_entity_id,0,sizeof(m_last_sent_command_target_entity_id));
}

bool Entity::ReceivedPDU(
        jdksavdecc_timestamp_in_milliseconds time_in_millis,
        uint8_t *buf,
        uint16_t len ) {
    bool r=false;
    FrameBase pdu(time_in_millis,buf,len);
    // we already know the message is AVTP ethertype and is either directly
    // targetting my MAC address or is a multicast message

    {
        // Try see if it is an AEM message
        jdksavdecc_aecpdu_aem aem;
        if( ParseAEM(&aem,pdu)) {
            if( IsAEMForTarget(aem,m_adp_manager.GetEntityID())) {
                ReceivedAEMCommand(aem,pdu);
                r=true;
            }
        }
    }


    if( !r ) {
        // Try see if it is an Address Access message
        jdksavdecc_aecp_aa aa;
        memset(&aa,0,sizeof(aa));   
        if( ParseAA(&aa,pdu)) {
            // Yes, is it a command to read/write data?
            if( IsAAForTarget(aa,m_adp_manager.GetEntityID())) {
                ReceivedAACommand(aa,pdu);
                r=true;
            }
        }
    }

    return r;
}

uint8_t Entity::ReceivedAEMCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu ) {
    // The low 15 bits of command_type is the command. High bit is the 'u' bit.
    uint16_t actual_command_type = aem.command_type&0x7fff;
    // Assume command is not implemented
    uint8_t response_status = JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;

    // commands that change state will set command_is_set_something to true
    bool command_is_set_something=false;

    switch(actual_command_type) {
    case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
        response_status=ReceiveAcquireEntityCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
        response_status=ReceiveLockEntityCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
        response_status=ReceiveControllerAvailableCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
        response_status=ReceiveEntityAvailableCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
        response_status=ReceiveReadDescriptorCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION:
        command_is_set_something=true;
        response_status = ValidatePermissions(aem);
        if( response_status==JDKSAVDECC_AEM_STATUS_SUCCESS ) {
            response_status=ReceiveSetConfigurationCommand(aem,pdu);
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION:
        response_status=ReceiveGetConfigurationCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_NAME:
        command_is_set_something=true;
        response_status = ValidatePermissions(aem);
        if( response_status==JDKSAVDECC_AEM_STATUS_SUCCESS ) {
            response_status=ReceiveSetNameCommand(aem,pdu);
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_NAME:
        response_status=ReceiveGetNameCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
        command_is_set_something=true;
        response_status = ValidatePermissions(aem);
        if( response_status==JDKSAVDECC_AEM_STATUS_SUCCESS ) {
            response_status=ReceiveSetControlCommand(aem,pdu);
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_CONTROL:
        response_status=ReceiveGetControlCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION:
        response_status=ReceiveRegisterUnsolicitedNotificationCommand(aem,pdu);
        break;
    case JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION:
        response_status=ReceiveDeRegisterUnsolicitedNotificationCommand(aem,pdu);
        break;
    }

    // fill in the new response status
    pdu.GetBuf()[JDKSAVDECC_FRAME_HEADER_LEN + 2 ] = (pdu.GetBuf()[JDKSAVDECC_FRAME_HEADER_LEN + 2 ]&0x7) + (response_status<<3);

    // Send the response to either just the requesting controller or it and all registered controllers
    SendResponses(
                false,
                command_is_set_something && response_status==JDKSAVDECC_AECP_STATUS_SUCCESS,
                pdu.GetBuf(),
                pdu.GetPos());

    return response_status;
}

uint8_t Entity::ValidatePermissions( jdksavdecc_aecpdu_aem const &aem ) {
    uint8_t response_status = JDKSAVDECC_AEM_STATUS_SUCCESS;
    // Check to see that it matches the current owner, if any
    // First, check if there is an owner (acquired)
    bool has_owner;
    has_owner = jdksavdecc_eui64_is_set(m_acquired_by_controller_entity_id);

    // if we have an owner and it isn't the controller that sent us the request then fail
    if( has_owner ) {
        if( jdksavdecc_eui64_compare(&m_acquired_by_controller_entity_id,&aem.aecpdu_header.controller_entity_id)!=0 ) {
            // not our controller.
            response_status=JDKSAVDECC_AEM_STATUS_ENTITY_ACQUIRED;
        } else {
            // it is our controller so we are good.
            response_status=JDKSAVDECC_AEM_STATUS_SUCCESS;
        }
    } else {
        // We don't have an owner, so check to see if we are locked
        if( jdksavdecc_eui64_is_set(m_locked_by_controller_entity_id) ) {
            // Yes, we are locked. Are we locked by the controller that sent us the message?
            if( jdksavdecc_eui64_compare(&m_locked_by_controller_entity_id,&aem.aecpdu_header.controller_entity_id)!=0 ) {
                // not our controller
                response_status=JDKSAVDECC_AEM_STATUS_ENTITY_LOCKED;
            } else {
                // it is our controller so we are good.
                response_status=JDKSAVDECC_AEM_STATUS_SUCCESS;
            }
        }
    }
    return response_status;
}



uint8_t Entity::ReceivedAACommand(
        jdksavdecc_aecp_aa const &aa,
        FrameBase &pdu) {
    // Yes go through the TLV's and dispatch the read/writes and respond
    uint8_t *p = pdu.GetBuf()+JDKSAVDECC_FRAME_HEADER_LEN+JDKSAVDECC_AECPDU_AA_LEN;
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
            uint32_t tlv_address = jdksavdecc_uint32_get( p, JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_LOWER );

            switch( tlv_mode ) {
            case JDKSAVDECC_AECP_AA_MODE_READ:
                aa_status=ReceiveAARead(aa,tlv_address,tlv_length,p+JDKSAVDECC_AECPDU_AA_TLV_LEN);
                break;
            case JDKSAVDECC_AECP_AA_MODE_WRITE:
                aa_status=ReceiveAAWrite(aa,tlv_address,tlv_length,p+JDKSAVDECC_AECPDU_AA_TLV_LEN);
                break;
            case JDKSAVDECC_AECP_AA_MODE_EXECUTE:
                aa_status=ReceiveAAExecute(aa,tlv_address,tlv_length,p+JDKSAVDECC_AECPDU_AA_TLV_LEN);
                break;
            }
        }
        p=p+JDKSAVDECC_AECPDU_AA_TLV_LEN+tlv_length;
        if( aa_status!=JDKSAVDECC_AECP_AA_STATUS_SUCCESS ) {
            break;
        }
    }
    // Send the response to either just the requesting controller or it and all registered controllers
    pdu.GetBuf()[JDKSAVDECC_FRAME_HEADER_LEN + 2 ] = (pdu.GetBuf()[JDKSAVDECC_FRAME_HEADER_LEN + 2 ]&0x7) + (aa_status<<3);

    // Only send responses to the requesting controller
    SendResponses(false,false,pdu.GetBuf(),pdu.GetPos());

    return aa_status;
}

void Entity::SendResponses(
        bool internally_generated,
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
            // We only care about controllers with entity_id != FF:FF:FF:FF:FF:FF:FF:FF

            if( jdksavdecc_eui64_is_set( m_registered_controllers_entity_id[i] ) ) {
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

void Entity::SendCommand(
        jdksavdecc_eui64 const &target_entity_id,
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
            pdu(0,target_mac_address,net->GetMACAddress(),JDKSAVDECC_AVTP_ETHERTYPE);

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
    net->SendRawNet( pdu.GetBuf(), pdu.GetPos(),
                     additional_data1, additional_data_length1,
                     additional_data2, additional_data_length2);

    if( track_for_ack ) {
        // Keep track of when we sent this message and who we sent it to so we can
        // manage time outs
        m_last_sent_command_time = GetTimeInMs();
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
            pdu(0,net->GetMACAddress(),net->GetMACAddress(),JDKSAVDECC_AVTP_ETHERTYPE);

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
                  (uint8_t*)pdu.GetBuf(),pdu.GetPos(),
                  additional_data1, additional_data_length1,
                  additional_data2, additional_data_length2);

}

uint8_t Entity::ReceiveAcquireEntityCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {

    uint8_t status = JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;

    bool controller_id_matches_current_owner;
    controller_id_matches_current_owner = jdksavdecc_eui64_compare(&m_acquired_by_controller_entity_id,&aem.aecpdu_header.controller_entity_id);

    bool has_current_owner;
    has_current_owner  = jdksavdecc_eui64_is_set(m_acquired_by_controller_entity_id);

    // First, make sure this is entity level:
    if( jdksavdecc_aem_command_acquire_entity_get_descriptor_index(pdu.GetBuf(),pdu.GetPos())==0 &&
           jdksavdecc_aem_command_acquire_entity_get_descriptor_type(pdu.GetBuf(),pdu.GetPos())==JDKSAVDECC_DESCRIPTOR_ENTITY ) {

        // is it a release or an acquire?
        if( jdksavdecc_aem_command_acquire_entity_get_aem_acquire_flags(pdu.GetBuf(),pdu.GetPos()) & 0x80000000 ) {
            // This is a request to release.  A release only works if the
            // requesting controller is the current owner, or there is no current owner
            if( (has_current_owner && controller_id_matches_current_owner) || !has_current_owner ) {
                // clear current acquire state
                jdksavdecc_eui64_init(&m_acquired_by_controller_entity_id);
                status = JDKSAVDECC_AEM_STATUS_SUCCESS;
            } else {
                // The controller can't release someone else's ownership.
                status = JDKSAVDECC_AEM_STATUS_ENTITY_ACQUIRED;
            }
        } else {
            // This is a request to acquire. Are we already acquired by the same controller or are we not yet acquired?
            if( !has_current_owner || (has_current_owner && controller_id_matches_current_owner)  ) {
                // Yes, success.
                status = JDKSAVDECC_AEM_STATUS_SUCCESS;
            } else {
                // Okay, someone else owns us. Send a CONTROLLER_AVAILABLE message to the controller that
                // currently owns us to make sure he is still around.
                SendControllerAvailable(m_acquired_by_controller_entity_id,m_acquired_by_controller_mac_address);
                // Keep track of when this happened
                m_acquire_in_progress_time = GetTimeInMs();
                // Return IN_PROGRESS, the real response will be coming either when the owning controller responds, or
                // if it times out.
                status=JDKSAVDECC_AEM_STATUS_IN_PROGRESS;
            }
        }
    }
    else {
        // We only support acquiring at the entity level
        status=JDKSAVDECC_AEM_STATUS_BAD_ARGUMENTS;
    }

    // TODO: Fill in response fields
    return status;
}


uint8_t Entity::ReceiveLockEntityCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveEntityAvailableCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveControllerAvailableCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

bool Entity::ReceiveControllerAvailableResponse(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    // TODO: Send a failed message to the controller that was attempting to acquire me
    // cancel any acquire in progress
    jdksavdecc_eui64_init(&m_acquire_in_progress_by_controller_entity_id);
    return false;
}

uint8_t Entity::ReceiveReadDescriptorCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveSetConfigurationCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveGetConfigurationCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveSetNameCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveGetNameCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveSetControlCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveGetControlCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveRegisterUnsolicitedNotificationCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveDeRegisterUnsolicitedNotificationCommand(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReadDescriptorEntity(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorConfiguration(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorControl(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorLocale(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorStrings(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReadDescriptorMemory(
        jdksavdecc_aecpdu_aem const &aem,
        FrameBase &pdu) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}


uint8_t Entity::ReceiveAARead(
        jdksavdecc_aecp_aa const &aa,
        uint32_t virtual_base_address,
        uint16_t length,
        uint8_t *response ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveAAWrite(
        jdksavdecc_aecp_aa const &aa,
        uint32_t virtual_base_address,
        uint16_t length,
        uint8_t const *request ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::ReceiveAAExecute(
        jdksavdecc_aecp_aa const &aa,
        uint32_t virtual_base_address,
        uint16_t length,
        uint8_t const *request ) {
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

}

