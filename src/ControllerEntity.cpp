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

#include "JDKSAvdeccMCU/ControllerEntity.hpp"

namespace JDKSAvdeccMCU
{

bool ControllerEntity::receivedPDU( RawSocket *incoming_socket, Frame &frame )
{
    bool r = false;
    bool done = false;

    // we already know the message is AVTP ethertype and is either directly
    // targetting my MAC address or is a multicast message
    // Try see if it is an AEM message
    jdksavdecc_aecpdu_aem aem;
    if ( parseAEM( &aem, frame ) )
    {
        if ( isAEMForTarget( aem, getEntityID() ) )
        {
            receivedAEMCommand( aem, frame );
            r = true;
        }
        else if ( isAEMForController( aem, getEntityID() ) )
        {
            r = receivedAEMResponse( aem, frame );
        }
        done = true;
    }

    if ( !done )
    {
        // Try see if it is an Address Access message
        jdksavdecc_aecp_aa aa;
        memset( &aa, 0, sizeof( aa ) );
        if ( parseAA( &aa, frame ) )
        {
            // Yes, is it a command to read/write data?
            if ( isAAForTarget( aa, getEntityID() ) )
            {
                receivedAACommand( aa, frame );
                r = true;
                // TODO: fill in response code in PDU and send reply
            }
        }
    }

    return r;
}

bool ControllerEntity::receivedAEMResponse( jdksavdecc_aecpdu_aem const &aem,
                                            Frame &pdu )
{
    bool r = false;
    // figure out if it is unsolicited from the high bit of command_type
    uint16_t actual_command_type = aem.command_type & 0x7fff;
    bool unsolicited = ( aem.command_type >> 15 ) & 1;

    // only bother with the response if it is either unsolicited,
    // or is solicited and matches the last request we did send
    bool interesting = unsolicited;

    if ( !unsolicited )
    {
        // First, is it from the entity we sent the command to?
        if ( m_last_sent_command_target_entity_id
             == aem.aecpdu_header.header.target_entity_id )
        {
            // yes, does the command type match?
            if ( actual_command_type == m_last_sent_command_type )
            {
                // yes, does the sequence ID match?
                if ( aem.aecpdu_header.sequence_id == m_outgoing_sequence_id )
                {
                    // Yes, then we are interested in this message
                    interesting = true;
                    // forget about the sent state by clearing the last send
                    // command target entity id
                    m_last_sent_command_target_entity_id = Eui64();
                }
            }
        }
    }

    // If this message is interesting to us then dispatch it
    if ( interesting )
    {
        switch ( actual_command_type )
        {
        case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
            r = receiveAcquireEntityResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
            r = receiveLockEntityResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
            r = receiveControllerAvailableResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
            r = receiveEntityAvailableResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
            r = receiveReadDescriptorResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION:
            r = receiveSetConfigurationResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION:
            r = receiveGetConfigurationResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_NAME:
            r = receiveSetNameResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_NAME:
            r = receiveGetNameResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
            r = receiveSetControlResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_GET_CONTROL:
            r = receiveGetControlResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION:
            r = receiveRegisterUnsolicitedNotificationResponse( aem, pdu );
            break;
        case JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION:
            r = receiveDeRegisterUnsolicitedNotificationResponse( aem, pdu );
            break;
        }
    }

    return r;
}

void ControllerEntity::sendAcquireEntity(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint32_t flags)
{
    FixedBufferWithSize<16> additional1;

    // offset 12 in Figure 7.34, and Table 7.127 for flags
    additional1.putQuadlet( flags );

    // offset 16 in Figure 7.34
    additional1.putEUI64( getEntityID() );

    // offset 24 in Figure 7.34
    additional1.putDoublet( JDKSAVDECC_DESCRIPTOR_ENTITY );

    // offset 26 in Figure 7.34
    additional1.putDoublet( 0 );

    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY,
                 true,
                 additional1.getBuf(),
                 additional1.getLength() );
}

bool ControllerEntity::receivedAAResponse( jdksavdecc_aecp_aa const &aa,
                                           Frame &pdu )
{
    (void)aa;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveAcquireEntityResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendLockEntity(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint32_t flags)
{
    FixedBufferWithSize<16> additional1;

    // offset 12 in Figure 7.35, and Table 7.128 for flags
    additional1.putQuadlet( flags );

    // offset 16 in Figure 7.35
    additional1.putEUI64( getEntityID() );

    // offset 24 in Figure 7.35
    additional1.putDoublet( JDKSAVDECC_DESCRIPTOR_ENTITY );

    // offset 26 in Figure 7.35
    additional1.putDoublet( 0 );

    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY,
                 true,
                 additional1.getBuf(),
                 additional1.getLength() );
}

bool ControllerEntity::receiveLockEntityResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendEntityAvailable(const Eui64 &target_entity_id, const Eui48 &target_mac_address)
{
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE );
}

bool ControllerEntity::receiveEntityAvailableResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendReadDescriptor(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint16_t configuration_index, uint16_t descriptor_type, uint16_t descriptor_index)
{
    uint8_t additional1[8];
    jdksavdecc_uint16_set(
                configuration_index, additional1, 0 );  // offset 12 in Figure 7.36
    jdksavdecc_uint16_set( 0, additional1, 2 ); // offset 14 in Figure 7.36
    jdksavdecc_uint16_set(
                descriptor_type, additional1, 4 ); // offset 16 in Figure 7.36
    jdksavdecc_uint16_set(
                descriptor_index, additional1, 6 ); // offset 20 in Figure 7.35
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR,
                 true,
                 additional1,
                 sizeof( additional1 ) );
}

bool ControllerEntity::receiveReadDescriptorResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendSetConfiguration(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint16_t configuration_index)
{
    uint8_t additional1[4];
    jdksavdecc_uint16_set( 0, additional1, 0 );
    jdksavdecc_uint16_set( configuration_index, additional1, 2 );
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION,
                 true,
                 additional1,
                 sizeof( additional1 ) );
}

bool ControllerEntity::receiveSetConfigurationResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendGetConfiguration(const Eui64 &target_entity_id, const Eui48 &target_mac_address)
{
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION );
}

bool ControllerEntity::receiveGetConfigurationResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendSetName(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint16_t target_descriptor_type, uint16_t target_descriptor_index, uint16_t name_index, uint16_t configuration_index, const jdksavdecc_string &name)
{
    uint8_t additional1[8];
    jdksavdecc_uint16_set( target_descriptor_type, additional1, 0 );
    jdksavdecc_uint16_set( target_descriptor_index, additional1, 2 );
    jdksavdecc_uint16_set( name_index, additional1, 4 );
    jdksavdecc_uint16_set( configuration_index, additional1, 6 );
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_SET_NAME,
                 true,
                 additional1,
                 sizeof( additional1 ),
                 name.value,
                 sizeof( name.value ) );
}

bool ControllerEntity::receiveSetNameResponse( jdksavdecc_aecpdu_aem const &aem,
                                               Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendGetName(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint16_t target_descriptor_type, uint16_t target_descriptor_index, uint16_t name_index, uint16_t configuration_index)
{
    uint8_t additional1[8];
    jdksavdecc_uint16_set( target_descriptor_type, additional1, 0 );
    jdksavdecc_uint16_set( target_descriptor_index, additional1, 2 );
    jdksavdecc_uint16_set( name_index, additional1, 4 );
    jdksavdecc_uint16_set( configuration_index, additional1, 6 );
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_GET_NAME );
}

bool ControllerEntity::receiveGetNameResponse( jdksavdecc_aecpdu_aem const &aem,
                                               Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendSetControl(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint16_t target_descriptor_index, uint8_t *control_value, uint16_t control_value_len, bool track_for_ack)
{
    uint8_t additional1[4];
    jdksavdecc_uint16_set( JDKSAVDECC_DESCRIPTOR_CONTROL, additional1, 0 );
    jdksavdecc_uint16_set( target_descriptor_index, additional1, 2 );
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_SET_CONTROL,
                 track_for_ack,
                 additional1,
                 sizeof( additional1 ),
                 control_value,
                 control_value_len );
}

bool ControllerEntity::receiveSetControlResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendRegisterUnsolicitedNotification(const Eui64 &target_entity_id, const Eui48 &target_mac_address)
{
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION,
                 0,
                 0 );
}

bool ControllerEntity::receiveGetControlResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

uint8_t ControllerEntity::receiveControlValue( Eui64 const &target_entity_id,
                                               uint16_t target_descriptor_index,
                                               uint8_t const *control_value,
                                               uint16_t control_value_len )
{
    (void)target_entity_id;
    (void)target_descriptor_index;
    (void)control_value;
    (void)control_value_len;
    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

void ControllerEntity::sendGetControl(const Eui64 &target_entity_id, const Eui48 &target_mac_address, uint16_t target_descriptor_index)
{
    uint8_t additional1[4];
    jdksavdecc_uint16_set( JDKSAVDECC_DESCRIPTOR_CONTROL, additional1, 0 );
    jdksavdecc_uint16_set( target_descriptor_index, additional1, 2 );
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_GET_CONTROL,
                 true,
                 additional1,
                 sizeof( additional1 ) );
}

bool ControllerEntity::receiveRegisterUnsolicitedNotificationResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

void ControllerEntity::sendDeRegisterUnsolicitedNotification(const Eui64 &target_entity_id, const Eui48 &target_mac_address)
{
    sendCommand( target_entity_id,
                 target_mac_address,
                 JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION,
                 0,
                 0 );
}

bool ControllerEntity::receiveDeRegisterUnsolicitedNotificationResponse(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}
}
