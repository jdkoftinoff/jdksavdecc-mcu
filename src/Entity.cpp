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

#include "JDKSAvdeccMCU/World.hpp"

#include "JDKSAvdeccMCU/Entity.hpp"
#include "JDKSAvdeccMCU/EntityState.hpp"

namespace JDKSAvdeccMCU
{

Entity::Entity( ADPManager &adp_manager,
                RegisteredControllers *registered_controllers,
                EntityState *entity_state,
                ACMPControllerGroupHandlerBase *acmp_controller_group_handler,
                ACMPTalkerGroupHandlerBase *acmp_talker_group_handler,
                ACMPListenerGroupHandlerBase *acmp_listener_group_handler )
    : m_adp_manager( adp_manager )
    , m_outgoing_sequence_id( 0 )
    , m_acquire_in_progress_time( 0 )
    , m_locked_time( 0 )
    , m_registered_controllers( registered_controllers )
    , m_last_sent_command_time( 0 )
    , m_last_sent_command_type( JDKSAVDECC_AEM_COMMAND_EXPANSION )
    , m_entity_state( entity_state )
    , m_acmp_controller_group_handler( acmp_controller_group_handler )
    , m_acmp_talker_group_handler( acmp_talker_group_handler )
    , m_acmp_listener_group_handler( acmp_listener_group_handler )
{
    // clear info on sent command state
    m_last_sent_command_target_entity_id.clear();
    // clear info on acquired state
    m_acquired_by_controller_entity_id.clear();
    m_acquire_in_progress_by_controller_entity_id.clear();
    m_locked_by_controller_entity_id.clear();
    m_acquired_by_controller_mac_address.clear();
}

void Entity::tick( jdksavdecc_timestamp_in_milliseconds time_in_millis )
{
    uint16_t cmd = m_last_sent_command_type;
    // If we are locked, then time out the lock
    if ( isSet( m_locked_by_controller_entity_id ) )
    {
        if ( wasTimeOutHit( time_in_millis, m_locked_time, JDKSAVDECC_AEM_LOCK_TIMEOUT_MS ) )
        {
            m_locked_by_controller_entity_id.clear();
        }
    }

    // TODO: if acquire is in progress more than 250 ms then
    // expire that one, update the new owner entity id and send
    // an acquire entity response message to the new owner

    // TODO: Send acquire in progress every 120 ms while we are
    // m_acquire_in_progress_by_controller_entity_id

    // Check to see if we had a command in flight that timed out
    if ( cmd != JDKSAVDECC_AEM_COMMAND_EXPANSION
         && wasTimeOutHit( time_in_millis, m_last_sent_command_time, JDKSAVDECC_AEM_TIMEOUT_IN_MS ) )
    {
        m_last_sent_command_type = JDKSAVDECC_AEM_COMMAND_EXPANSION; // clear knowledge of sent
                                                                     // command

        // Was the command a CONTROLLER_AVAILABLE? if so, handle it here
        if ( cmd == JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE )
        {
            // Yes, this means that the old controller goes away and the new one
            // is approved
            m_acquired_by_controller_entity_id = m_acquire_in_progress_by_controller_entity_id;
            // Also clear any lock that may have been there
            m_locked_by_controller_entity_id = Eui64();
            // TODO: Formulate and send reply to the new controller
        }
        else
        {
            // Notify entity info about the timed out command
            commandTimedOut( m_last_sent_command_target_entity_id, cmd, m_outgoing_sequence_id );
        }
    }

    // Run periodic state machine events for ACMP Controller
    if ( m_acmp_controller_group_handler )
    {
        m_acmp_controller_group_handler->tick( time_in_millis );
    }
    // Run periodic state machine events for ACMP Talker
    if ( m_acmp_talker_group_handler )
    {
        m_acmp_talker_group_handler->tick( time_in_millis );
    }
    // Run periodic state machine events for ACMP Listener
    if ( m_acmp_listener_group_handler )
    {
        m_acmp_listener_group_handler->tick( time_in_millis );
    }
}

void Entity::commandTimedOut( Eui64 const &target_entity_id, uint16_t command_type, uint16_t sequence_id )
{
    (void)target_entity_id;
    (void)command_type;
    (void)sequence_id;
    //    avr_debug_log("TimeOut:", sequence_id );
    m_last_sent_command_target_entity_id.clear();
}

bool Entity::receivedPDU( RawSocket *incoming_socket, Frame &frame )
{
    // default status code to not implemented, this value is common to all sub
    // protocols
    uint8_t status_code = JDKSAVDECC_AEM_STATUS_NOT_IMPLEMENTED;

    bool r = false;
    // we already know the message is AVTP ethertype and is either directly
    // targetting my MAC address or is a multicast message

    // Try see if it is an AEM message
    {
        jdksavdecc_aecpdu_aem aem;
        if ( parseAEM( &aem, frame ) )
        {
            if ( isAEMForTarget( aem, getEntityID() ) )
            {
                status_code = receivedAEMCommand( incoming_socket, aem, frame );
                r = true;
            }
        }
    }

    if ( !r )
    {
        // Try see if it is an Address Access message
        jdksavdecc_aecp_aa aa;
        memset( &aa, 0, sizeof( aa ) );
        if ( parseAA( &aa, frame ) )
        {
            // Yes, is it a command to read/write data?
            if ( isAAForTarget( aa, getEntityID() ) )
            {
                status_code = receivedAACommand( incoming_socket, aa, frame );
                r = true;
            }
        }
    }

    if ( !r )
    {
        // Try see if it is an ACMP message
        jdksavdecc_acmpdu acmpdu;
        memset( &acmpdu, 0, sizeof( acmpdu ) );
        if ( parseACMP( &acmpdu, frame ) )
        {
            if ( isACMPInvolvingTarget( acmpdu, getEntityID() ) )
            {
                status_code = receivedACMPMessage( incoming_socket, acmpdu, frame );
                r = true;
            }
        }
    }

    (void)status_code;

    return r;
}

uint8_t Entity::receivedAEMCommand( RawSocket *incoming_socket, jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    // The low 15 bits of command_type is the command. High bit is the 'u' bit.
    uint16_t actual_command_type = aem.command_type & 0x7fff;
    // Assume command is not implemented
    uint8_t response_status = JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;

    // commands that change state will set command_is_set_something to true
    bool command_is_set_something = false;

    switch ( actual_command_type )
    {
    case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
        response_status = receiveAcquireEntityCommand( aem, pdu );
        break;
    case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
        response_status = receiveLockEntityCommand( aem, pdu );
        break;
    case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
        response_status = receiveControllerAvailableCommand( aem, pdu );
        break;
    case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
        response_status = receiveEntityAvailableCommand( aem, pdu );
        break;
    case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
        if ( m_entity_state )
        {
            uint16_t configuration_index
                = jdksavdecc_aem_command_read_descriptor_get_configuration_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );
            uint16_t descriptor_type
                = jdksavdecc_aem_command_read_descriptor_get_descriptor_type( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );
            uint16_t descriptor_index
                = jdksavdecc_aem_command_read_descriptor_get_descriptor_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );

            response_status
                = m_entity_state->receiveReadDescriptorCommand( pdu, configuration_index, descriptor_type, descriptor_index );
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION:
        command_is_set_something = true;
        response_status = validatePermissions( aem );
        if ( response_status == JDKSAVDECC_AEM_STATUS_SUCCESS && m_entity_state )
        {
            response_status = m_entity_state->receiveSetConfigurationCommand( pdu );
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION:
        if ( m_entity_state )
        {
            response_status = m_entity_state->receiveGetConfigurationCommand( pdu );
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_NAME:
        command_is_set_something = true;
        response_status = validatePermissions( aem );
        if ( response_status == JDKSAVDECC_AEM_STATUS_SUCCESS && m_entity_state )
        {
            uint16_t configuration_index
                = jdksavdecc_aem_command_set_name_get_configuration_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );
            uint16_t descriptor_type
                = jdksavdecc_aem_command_set_name_get_descriptor_type( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );
            uint16_t descriptor_index
                = jdksavdecc_aem_command_set_name_get_descriptor_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );

            response_status = m_entity_state->receiveSetNameCommand( pdu, configuration_index, descriptor_type, descriptor_index );
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_NAME:
        if ( m_entity_state )
        {
            uint16_t configuration_index
                = jdksavdecc_aem_command_get_name_get_configuration_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );
            uint16_t descriptor_type
                = jdksavdecc_aem_command_get_name_get_descriptor_type( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );
            uint16_t descriptor_index
                = jdksavdecc_aem_command_get_name_get_descriptor_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );

            response_status = m_entity_state->receiveGetNameCommand( pdu, configuration_index, descriptor_type, descriptor_index );
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
        command_is_set_something = true;
        response_status = validatePermissions( aem );
        if ( response_status == JDKSAVDECC_AEM_STATUS_SUCCESS && m_entity_state )
        {
            uint16_t descriptor_index
                = jdksavdecc_aem_command_set_control_get_descriptor_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );

            response_status = m_entity_state->receiveSetControlCommand( pdu, descriptor_index );
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_GET_CONTROL:
        if ( m_entity_state )
        {
            uint16_t descriptor_index
                = jdksavdecc_aem_command_get_control_get_descriptor_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN );

            response_status = m_entity_state->receiveGetControlCommand( pdu, descriptor_index );
        }
        break;
    case JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION:
        response_status = receiveRegisterUnsolicitedNotificationCommand( aem, pdu );
        break;
    case JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION:
        response_status = receiveDeRegisterUnsolicitedNotificationCommand( aem, pdu );
        break;
    }

    // fill in the new response status
    pdu.setOctet( ( pdu.getOctet( JDKSAVDECC_FRAME_HEADER_LEN + 2 ) & 0x7 ) + ( response_status << 3 ),
                  JDKSAVDECC_FRAME_HEADER_LEN + 2 );

    // Send the response to either just the requesting controller or it and all
    // registered controllers
    sendResponses( false, command_is_set_something && response_status == JDKSAVDECC_AECP_STATUS_SUCCESS, response_status, pdu );

    return response_status;
}

uint8_t Entity::validatePermissions( jdksavdecc_aecpdu_aem const &aem )
{
    uint8_t response_status = JDKSAVDECC_AEM_STATUS_SUCCESS;
    // Check to see that it matches the current owner, if any
    // First, check if there is an owner (acquired)
    bool has_owner;
    has_owner = ( isSet( m_acquired_by_controller_entity_id ) != 0 );

    // if we have an owner and it isn't the controller that sent us the request
    // then fail
    if ( has_owner )
    {
        if ( m_acquired_by_controller_entity_id != aem.aecpdu_header.controller_entity_id )
        {
            // not our controller.
            response_status = JDKSAVDECC_AEM_STATUS_ENTITY_ACQUIRED;
        }
        else
        {
            // it is our controller so we are good.
            response_status = JDKSAVDECC_AEM_STATUS_SUCCESS;
        }
    }
    else
    {
        // We don't have an owner, so check to see if we are locked
        if ( isSet( m_locked_by_controller_entity_id ) )
        {
            // Yes, we are locked. Are we locked by the controller that sent us
            // the message?
            if ( m_locked_by_controller_entity_id != aem.aecpdu_header.controller_entity_id )
            {
                // not our controller
                response_status = JDKSAVDECC_AEM_STATUS_ENTITY_LOCKED;
            }
            else
            {
                // it is our controller so we are good.
                response_status = JDKSAVDECC_AEM_STATUS_SUCCESS;
            }
        }
    }
    return response_status;
}

uint8_t Entity::receivedAACommand( RawSocket *incoming_socket, jdksavdecc_aecp_aa const &aa, Frame &pdu )
{
    // Yes go through the TLV's and dispatch the read/writes and respond
    uint8_t *p = pdu.getBuf() + JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AECPDU_AA_LEN;
    uint8_t aa_status = JDKSAVDECC_AECP_AA_STATUS_NOT_IMPLEMENTED;
    for ( uint16_t i = 0; i < aa.tlv_count; ++i )
    {
        // See 9.2.1.3.3
        uint8_t tlv_mode = ( p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_MODE_LENGTH] >> 4 ) & 0xf;

        uint16_t tlv_length = ( ( ( uint16_t )( p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_MODE_LENGTH] & 0xf ) ) << 4 )
                              + p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_MODE_LENGTH + 1];

        // require top 32 bits of address to be zero
        if ( p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER] == 0 && p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER + 1] == 0
             && p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER + 2] == 0
             && p[JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_UPPER + 3] == 0 )
        {
            uint32_t tlv_address = jdksavdecc_uint32_get( p, JDKSAVDECC_AECPDU_AA_TLV_OFFSET_ADDRESS_LOWER );

            switch ( tlv_mode )
            {
            case JDKSAVDECC_AECP_AA_MODE_READ:
                if ( m_entity_state )
                {
                    aa_status = m_entity_state->receiveAARead( tlv_address, tlv_length, p + JDKSAVDECC_AECPDU_AA_TLV_LEN );
                }
                break;
            case JDKSAVDECC_AECP_AA_MODE_WRITE:
                if ( m_entity_state )
                {
                    aa_status = m_entity_state->receiveAAWrite( tlv_address, tlv_length, p + JDKSAVDECC_AECPDU_AA_TLV_LEN );
                }
                break;
            case JDKSAVDECC_AECP_AA_MODE_EXECUTE:
                if ( m_entity_state )
                {
                    aa_status = m_entity_state->receiveAAExecute( tlv_address, tlv_length, p + JDKSAVDECC_AECPDU_AA_TLV_LEN );
                }
                break;
            }
        }
        p = p + JDKSAVDECC_AECPDU_AA_TLV_LEN + tlv_length;
        if ( aa_status != JDKSAVDECC_AECP_AA_STATUS_SUCCESS )
        {
            break;
        }
    }
    // Send the response to either just the requesting controller or it and all
    // registered controllers
    pdu.setOctet( ( pdu.getOctet( JDKSAVDECC_FRAME_HEADER_LEN + 2 ) & 0x7 ) + ( aa_status << 3 ), JDKSAVDECC_FRAME_HEADER_LEN + 2 );

    // Only send responses to the requesting controller
    sendResponses( false, false, aa_status, pdu );

    return aa_status;
}

uint8_t Entity::receivedACMPMessage( RawSocket *incoming_socket, const jdksavdecc_acmpdu &acmpdu, Frame &pdu )
{
    uint8_t status = JDKSAVDECC_ACMP_STATUS_NOT_SUPPORTED;

    if ( m_acmp_controller_group_handler && acmpdu.controller_entity_id == getEntityID() )
    {
        if ( acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_TX_RESPONSE
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE )
        {
            status = m_acmp_controller_group_handler->receivedACMPDU( incoming_socket, acmpdu, pdu );
        }
    }
    if ( m_acmp_talker_group_handler && acmpdu.talker_entity_id == getEntityID() )
    {
        if ( acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_TX_COMMAND
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_COMMAND
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_COMMAND )
        {
            status = m_acmp_talker_group_handler->receivedACMPDU( incoming_socket, acmpdu, pdu );
        }
    }
    if ( m_acmp_listener_group_handler && acmpdu.listener_entity_id == getEntityID() )
    {
        if ( acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_COMMAND
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_COMMAND
             || acmpdu.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_COMMAND )
        {
            status = m_acmp_listener_group_handler->receivedACMPDU( incoming_socket, acmpdu, pdu );
        }
    }

    return status;
}

void Entity::sendResponses( bool internally_generated,
                            bool send_to_registered_controllers,
                            uint8_t aecp_status_code,
                            Frame &pdu,
                            uint8_t const *additional_data1,
                            uint16_t additional_data_length1,
                            uint8_t const *additional_data2,
                            uint16_t additional_data_length2 )
{
    // buf already contains the requestor's mac address and controller. Capture
    // the controller entity id
    Eui64 original_controller_id;

    original_controller_id = Eui64();

    pdu.setOctet( ( ( pdu.getOctet( 2 ) & 0xf8 ) | ( aecp_status_code << 3 ) ), 2 );

    if ( !internally_generated )
    {
        // The message is not internally generated, it is a response to a real
        // request.

        // Send the buf to the original
        getRawSocket().sendReplyFrame( pdu, additional_data1, additional_data_length1, additional_data2, additional_data_length2 );
    }
    else
    {
        pdu.setSA( getRawSocket().getMACAddress() );
    }

    if ( send_to_registered_controllers )
    {
        // Mark the message as an unsolicited response
        jdksavdecc_aecpdu_aem_set_command_type( jdksavdecc_aecpdu_aem_get_command_type( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN )
                                                | 0x8000,
                                                pdu.getBuf(),
                                                JDKSAVDECC_FRAME_HEADER_LEN );

        // Now go through all subscribed entities
        for ( uint16_t i = 0; i < m_registered_controllers->getControllerCount(); ++i )
        {
            RegisteredController const *controller = m_registered_controllers->getController( i );

            // don't send the original requesting controller a double
            // response
            // and skip sending a second message to the acquiring controller even it if also registered explicitely
            if ( original_controller_id != controller->m_entity_id
                 && controller->m_entity_id != m_acquired_by_controller_entity_id )
            {
                // Set the controller_entity_id in the frame

                controller->m_entity_id.store( pdu.getBuf(),
                                               JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AECPDU_COMMON_OFFSET_CONTROLLER_ENTITY_ID );

                // Set the destination mac address
                pdu.setDA( controller->m_mac_address );

                // Send the frame to that controller
                getRawSocket().sendFrame(
                    pdu, additional_data1, additional_data_length1, additional_data2, additional_data_length2 );
            }
        }

        // If the message is internally generated, also send it to the controller that acquired the entity
        if ( internally_generated && m_acquired_by_controller_entity_id.isSet() )
        {
            m_acquired_by_controller_entity_id.store(
                pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AECPDU_COMMON_OFFSET_CONTROLLER_ENTITY_ID );
            pdu.setDA( m_acquired_by_controller_mac_address );

            // Send the frame to that controller
            getRawSocket().sendFrame( pdu, additional_data1, additional_data_length1, additional_data2, additional_data_length2 );
        }
    }
}

void Entity::sendCommand( Eui64 const &target_entity_id,
                          Eui48 const &target_mac_address,
                          uint16_t aem_command_type,
                          bool track_for_ack,
                          uint8_t const *additional_data1,
                          uint16_t additional_data_length1,
                          uint8_t const *additional_data2,
                          uint16_t additional_data_length2 )
{

    // Make a temp pdu buffer just long enough to contain:
    // ethernet frame DA,SA,Ethertype, AVTP Common Control Header, AVDECC AEM
    // Common Format.
    FrameWithSize<JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AECPDU_AEM_LEN> pdu(
        0, target_mac_address, getRawSocket().getMACAddress(), JDKSAVDECC_AVTP_ETHERTYPE );

    // control_data_length field is N + value_length
    uint16_t control_data_length = JDKSAVDECC_AECPDU_AEM_LEN - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + additional_data_length1
                                   + additional_data_length2;

    // AECPDU common control header
    pdu.putOctet( JDKSAVDECC_1722A_SUBTYPE_AECP );                   // cd=1, subtype=0x7b (AECP)
    pdu.putOctet( 0x00 + JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND ); // sv=0, version=0,
                                                                     // message_type =
                                                                     // AEM_COMMAND
    pdu.putOctet( ( ( JDKSAVDECC_AEM_STATUS_SUCCESS ) << 3 )
                  + ( ( control_data_length >> 8 ) & 0x7 ) ); // Send success code. top 3 bits of control_data_length
    pdu.putOctet( control_data_length & 0xff );               // lower 8 bits of control_data_length

    pdu.putEUI64( target_entity_id ); // entity_id of the device we are setting
    pdu.putEUI64( getEntityID() );    // controller_id

    // increment outoging sequence id before sending it, so when we receive a
    // response we know what sequence id to expect
    // there are no inflight messages here
    ++m_outgoing_sequence_id;
    pdu.putDoublet( m_outgoing_sequence_id );
    pdu.putDoublet( aem_command_type );

    // Send the header appended to any additional data
    getRawSocket().sendFrame( pdu, additional_data1, additional_data_length1, additional_data2, additional_data_length2 );

    if ( track_for_ack )
    {
        // Keep track of when we sent this message and who we sent it to so we
        // can
        // manage time outs
        m_last_sent_command_time = getRawSocket().getTimeInMilliseconds();
        m_last_sent_command_type = aem_command_type;
        m_last_sent_command_target_entity_id = target_entity_id;
    }
}

void Entity::sendUnsolicitedResponses( uint16_t aem_command_type,
                                       uint8_t const *additional_data1,
                                       uint16_t additional_data_length1,
                                       uint8_t const *additional_data2,
                                       uint16_t additional_data_length2 )
{

    // Make a temp pdu buffer just long enough to contain:
    // ethernet frame DA,SA,Ethertype, AVTP Common Control Header, AVDECC AEM
    // Common Format.
    FrameWithSize<JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AECPDU_AEM_LEN> pdu(
        0, getRawSocket().getMACAddress(), getRawSocket().getMACAddress(), JDKSAVDECC_AVTP_ETHERTYPE );

    // control_data_length field is N + value_length
    uint16_t control_data_length = JDKSAVDECC_AECPDU_AEM_LEN - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + additional_data_length1
                                   + additional_data_length2;

    // AECPDU common control header

    // cd=1, subtype=0x7b (AECP)
    pdu.putOctet( JDKSAVDECC_1722A_SUBTYPE_AECP );

    // sv=0, version=0, message_type = AEM_COMMAND
    pdu.putOctet( 0x00 + JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );

    // Send success code. top 3 bits of control_data_length
    pdu.putOctet( ( ( JDKSAVDECC_AEM_STATUS_SUCCESS ) << 3 ) + ( ( control_data_length >> 8 ) & 0x7 ) );

    // lower 8 bits of control_data_length
    pdu.putOctet( control_data_length & 0xff );

    // entity_id of the device we are setting (placeholder)
    pdu.putEUI64( getEntityID() );

    // controller_id
    pdu.putEUI64( getEntityID() );

    // increment outoging sequence id before sending it, so when we receive a
    // response we know what sequence id to expect
    // there are no inflight messages here
    ++m_outgoing_sequence_id;

    pdu.putDoublet( m_outgoing_sequence_id );
    pdu.putDoublet( aem_command_type );

    sendResponses( true,
                   true,
                   JDKSAVDECC_AECP_STATUS_SUCCESS,
                   pdu,
                   additional_data1,
                   additional_data_length1,
                   additional_data2,
                   additional_data_length2 );
}

uint8_t Entity::receiveAcquireEntityCommand( jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{

    uint8_t status = JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;

    bool controller_id_matches_current_owner;
    controller_id_matches_current_owner = ( m_acquired_by_controller_entity_id == aem.aecpdu_header.controller_entity_id );

    bool has_current_owner;
    has_current_owner = ( isSet( m_acquired_by_controller_entity_id ) != 0 );

    // First, make sure this is entity level:
    if ( jdksavdecc_aem_command_acquire_entity_get_descriptor_index( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN ) == 0
         && jdksavdecc_aem_command_acquire_entity_get_descriptor_type( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN )
            == JDKSAVDECC_DESCRIPTOR_ENTITY )
    {

        // is it a release or an acquire?
        if ( jdksavdecc_aem_command_acquire_entity_get_aem_acquire_flags( pdu.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN ) & 0x80000000 )
        {
            // This is a request to release.  A release only works if the
            // requesting controller is the current owner, or there is no
            // current owner

            if ( ( has_current_owner && controller_id_matches_current_owner ) || !has_current_owner )
            {
                // clear current acquire state
                m_acquired_by_controller_entity_id = Eui64();
                status = JDKSAVDECC_AEM_STATUS_SUCCESS;
            }
            else
            {
                // The controller can't release someone else's ownership.
                status = JDKSAVDECC_AEM_STATUS_ENTITY_ACQUIRED;
            }
        }
        else
        {
            // This is a request to acquire. Are we already acquired by the same
            // controller or are we not yet acquired?

            if ( ( has_current_owner && controller_id_matches_current_owner ) || ( !has_current_owner ) )
            {
                // Yes, success.
                status = JDKSAVDECC_AEM_STATUS_SUCCESS;
            }
            else
            {
                // Are we already in progress of acquiring from a second
                // controller?
                if ( isSet( m_acquire_in_progress_by_controller_entity_id ) )
                {
                    // yes, we we are already waiting for a dispute between 2
                    // controllers.
                    // return that we are acquired
                    status = JDKSAVDECC_AEM_STATUS_ENTITY_ACQUIRED;
                }
                else
                {
                    // Okay, someone else owns us, and we are not currently in
                    // progress of validating that controllers available status.
                    // Send a CONTROLLER_AVAILABLE message to the controller
                    // that currently owns us to make sure he is still around.

                    sendControllerAvailable( m_acquired_by_controller_entity_id, m_acquired_by_controller_mac_address );

                    // Keep track of when this happened
                    m_acquire_in_progress_time = getRawSocket().getTimeInMilliseconds();

                    // Return IN_PROGRESS, the real response will be coming
                    // either
                    // when the owning controller responds, or
                    // if it times out.
                    status = JDKSAVDECC_AEM_STATUS_IN_PROGRESS;
                }
            }
        }
    }
    else
    {
        // We only support acquiring at the entity level
        status = JDKSAVDECC_AEM_STATUS_BAD_ARGUMENTS;
    }

    // TODO: Fill in response fields
    return status;
}

uint8_t Entity::receiveLockEntityCommand( jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t Entity::receiveEntityAvailableCommand( jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_SUCCESS;
}

uint8_t Entity::receiveControllerAvailableCommand( jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_SUCCESS;
}

bool Entity::receiveControllerAvailableResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;

    // TODO: Send a failed message to the controller that was attempting to
    // acquire me
    // cancel any acquire in progress
    m_acquire_in_progress_by_controller_entity_id = Eui64();
    return false;
}

uint8_t Entity::receiveRegisterUnsolicitedNotificationCommand( jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    uint8_t status = JDKSAVDECC_AEM_STATUS_NO_RESOURCES;
    bool registered = m_registered_controllers->addController( aem.aecpdu_header.controller_entity_id, pdu.getSA() );
    if ( registered )
    {
        status = JDKSAVDECC_AECP_STATUS_SUCCESS;
    }
    return status;
}

uint8_t Entity::receiveDeRegisterUnsolicitedNotificationCommand( jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    uint8_t status = JDKSAVDECC_AECP_STATUS_SUCCESS;

    m_registered_controllers->removeController( aem.aecpdu_header.controller_entity_id );

    return status;
}
}
