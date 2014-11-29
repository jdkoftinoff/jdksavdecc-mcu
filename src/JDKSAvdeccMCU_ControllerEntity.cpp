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

#include "JDKSAvdeccMCU_World.hpp"

#include "JDKSAvdeccMCU_ControllerEntity.hpp"

namespace JDKSAvdeccMCU
{

bool ControllerEntity::receivedPDU( Frame &frame )
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

bool ControllerEntity::receiveLockEntityResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveEntityAvailableResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveReadDescriptorResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveSetConfigurationResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveGetConfigurationResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveSetNameResponse( jdksavdecc_aecpdu_aem const &aem,
                                               Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveGetNameResponse( jdksavdecc_aecpdu_aem const &aem,
                                               Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveSetControlResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
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

bool ControllerEntity::receiveRegisterUnsolicitedNotificationResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}

bool ControllerEntity::receiveDeRegisterUnsolicitedNotificationResponse(
    jdksavdecc_aecpdu_aem const &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;
    return false;
}
}
