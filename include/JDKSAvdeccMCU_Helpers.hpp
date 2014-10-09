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
#pragma once

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_Frame.hpp"

namespace JDKSAvdeccMCU
{

inline bool wasTimeOutHit( jdksavdecc_timestamp_in_milliseconds cur_time,
                           jdksavdecc_timestamp_in_milliseconds last_time_done,
                           jdksavdecc_timestamp_in_milliseconds timeout )
{
    jdksavdecc_timestamp_in_milliseconds difftime = cur_time - last_time_done;
    return difftime > timeout;
}

/// Helper function to parse AECP AEM message
bool parseAEM( jdksavdecc_aecpdu_aem *aem, uint8_t const *buf, uint16_t pos, uint16_t len );

/// Test to see if AECPDU AEM message is a command for the specified target entity
bool isAEMForTarget( jdksavdecc_aecpdu_aem const &aem, jdksavdecc_eui64 const &expected_target_entity_id );

/// Test to see if AECPDU AEM message is a response for the specified controller entity
bool isAEMForController( jdksavdecc_aecpdu_aem const &aem, jdksavdecc_eui64 const &expected_controller_entity_id );

/// Twiddle the header bytes to convert this message from AEM_COMMAND to AEM_RESPONSE in place
/// with the new status code, and the new control_data_length to match the new_length
void setAEMReply( uint8_t status_code, uint16_t new_length, uint8_t *buf, uint16_t pos, uint16_t len );

/// Helper function to parse AECP AA message
bool parseAA( jdksavdecc_aecp_aa *aa, uint8_t const *buf, uint16_t pos, uint16_t len );

/// Test to see if AECPDU AA message is a command for the specified target entity
bool isAAForTarget( jdksavdecc_aecp_aa const &aa, jdksavdecc_eui64 const &expected_target_entity_id );

/// Test to see if AECPDU AA message is a response for the specified controller entity
bool isAAForController( jdksavdecc_aecp_aa const &aa, jdksavdecc_eui64 const &expected_controller_entity_id );

/// Twiddle the header bytes to convert this message from ADDRESS_ACCESS_COMMAND to ADDRESS_ACCESS_RESPONSE in place
/// with the new status code, and the new control_data_length to match the new_length
void setAAReply( uint16_t new_length, uint8_t *buf, uint16_t pos, uint16_t len );

/// Formulate an AEM SET_CONTROL style message with payload
void formAEMSetControl( jdksavdecc_eui48 const &dest_mac,
                        jdksavdecc_eui48 const &src_mac,
                        jdksavdecc_eui64 const &my_controller_entity_id,
                        jdksavdecc_eui64 const &destination_entity_id,
                        bool unsolicited,
                        uint16_t descriptor_index,
                        uint8_t const *value_data,
                        uint16_t value_data_length,
                        uint8_t *buf );

inline bool parseAEM( jdksavdecc_aecpdu_aem *aem, FrameBase const &rx )
{
    bool r = false;
    // Validate subtype is AECP
    if ( jdksavdecc_uint8_get( rx.getBuf(), rx.getPos() ) == ( 0x80 + JDKSAVDECC_SUBTYPE_AECP ) )
    {
        // Yes, read the aem header
        memset( aem, 0, sizeof( *aem ) );
        if ( jdksavdecc_aecpdu_aem_read( aem, rx.getBuf(), rx.getPos(), rx.getLen() ) > 0 )
        {
            // make sure it is version 0 and an AEM_COMMAND or AEM_RESPONSE
            if ( aem->aecpdu_header.header.version == 0
                 && ( aem->aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND
                      || aem->aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE ) )
            {
                r = true;
            }
        }
    }
    return r;
}

inline bool isAEMForTarget( jdksavdecc_aecpdu_aem const &aem, jdksavdecc_eui64 const &expected_target_entity_id )
{
    bool r = false;
    // Is it an AEM_COMMAND?
    if ( aem.aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND )
    {
        // Yes
        // Is it for me?
        if ( jdksavdecc_eui64_compare( &expected_target_entity_id, &aem.aecpdu_header.header.target_entity_id ) == 0 )
        {
            r = true;
        }
    }
    return r;
}

inline bool isAEMForController( jdksavdecc_aecpdu_aem const &aem, jdksavdecc_eui64 const &expected_controller_entity_id )
{
    bool r = false;
    // Is it an AEM_RESPONSE?
    if ( aem.aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE )
    {
        // Yes
        // Is it for me?
        if ( jdksavdecc_eui64_compare( &expected_controller_entity_id, &aem.aecpdu_header.controller_entity_id ) == 0 )
        {
            r = true;
        }
    }
    return r;
}

inline void setAEMReply( uint8_t status_code, uint16_t new_length, FrameBase &pdu )
{
    // offset 1: sv=0, version=0, control_data = AEM_RESPONSE
    pdu.getBuf()[pdu.getPos() + 1] = JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE;
    // new control data length is new_length minus frame beginning position pos and the common control header.
    // See IEEE 1722-2011 Clause 5.3.3
    uint16_t control_data_length = new_length - pdu.getPos() - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN;
    // offset 2: status = status code, top 3 bits of new control_data_length
    pdu.getBuf()[pdu.getPos() + 2] = ( status_code << 3 ) + ( ( control_data_length >> 8 ) & 0x7 );
    // offset 3: bottom 8 bits of new control_data_length
    // TODO: new control_data_length
}

inline bool parseAA( jdksavdecc_aecp_aa *aa, FrameBase const &pdu )
{
    bool r = false;
    // Validate subtype is AA
    if ( jdksavdecc_uint8_get( pdu.getBuf(), pdu.getPos() ) == ( 0x80 + JDKSAVDECC_SUBTYPE_AECP ) )
    {
        // Yes, read the aa header

        if ( jdksavdecc_aecp_aa_read( aa, pdu.getBuf(), pdu.getPos(), pdu.getLen() ) > 0 )
        {
            // make sure it is version 0 and an AA_COMMAND or AA_RESPONSE
            if ( aa->aecpdu_header.header.version == 0
                 && ( aa->aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND
                      || aa->aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE ) )
            {
                r = true;
            }
        }
    }
    return r;
}

inline bool isAAForTarget( jdksavdecc_aecp_aa const &aa, jdksavdecc_eui64 const &expected_target_entity_id )
{
    bool r = false;

    // Is it an AA_COMMAND?
    if ( aa.aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND )
    {
        // Yes
        // Is it for me?
        if ( jdksavdecc_eui64_compare( &expected_target_entity_id, &aa.aecpdu_header.header.target_entity_id ) == 0 )
        {
            r = true;
        }
    }
    return r;
}

inline bool isAAForController( jdksavdecc_aecp_aa const &aa, jdksavdecc_eui64 const &expected_controller_entity_id )
{
    bool r = false;

    // Is it an AA_RESPONSE?
    if ( aa.aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE )
    {
        // Yes
        // Is it for me?
        if ( jdksavdecc_eui64_compare( &expected_controller_entity_id, &aa.controller_entity_id ) == 0 )
        {
            r = true;
        }
    }
    return r;
}

inline void setAAReply( uint8_t status_code, uint16_t new_length, uint8_t *buf, uint16_t pos, uint16_t len )
{
    if ( len > pos + 2 )
    {
        // offset 1: sv=0, version=0, control_data = ADDRESS_ACCESS_RESPONSE
        buf[pos + 1] = JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE;
        // new control data length is new_length minus frame beginning position pos and the common control header.
        // See IEEE 1722-2011 Clause 5.3.3
        uint16_t control_data_length = new_length - pos - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN;
        // offset 2: status = status code, top 3 bits of new control_data_length
        buf[pos + 2] = ( status_code << 3 ) + ( ( control_data_length >> 8 ) & 0x7 );
        // offset 3: bottom 8 bits of new control_data_length
        buf[pos + 3] = ( control_data_length & 0xff );
    }
}
}
