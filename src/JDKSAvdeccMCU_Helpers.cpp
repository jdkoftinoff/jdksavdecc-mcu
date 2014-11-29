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
#include "JDKSAvdeccMCU_Helpers.hpp"

namespace JDKSAvdeccMCU
{

bool parseAEM( jdksavdecc_aecpdu_aem *aem, Frame const &rx )
{
    bool r = false;
    // Validate subtype is AECP
    if ( rx.getOctet( JDKSAVDECC_FRAME_HEADER_LEN )
         == JDKSAVDECC_1722A_SUBTYPE_AECP )
    {
        // Yes, read the aem header
        memset( aem, 0, sizeof( *aem ) );
        if ( jdksavdecc_aecpdu_aem_read(
                 aem, rx.getBuf(), JDKSAVDECC_FRAME_HEADER_LEN, rx.getLength() )
             > 0 )
        {
            // make sure it is version 0 and an AEM_COMMAND or AEM_RESPONSE
            if ( aem->aecpdu_header.header.version == 0
                 && ( aem->aecpdu_header.header.message_type
                      == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND
                      || aem->aecpdu_header.header.message_type
                         == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE ) )
            {
                r = true;
            }
        }
    }
    return r;
}

bool isAEMForTarget( jdksavdecc_aecpdu_aem const &aem,
                     Eui64 const &expected_target_entity_id )
{
    bool r = false;
    // Is it an AEM_COMMAND?
    if ( aem.aecpdu_header.header.message_type
         == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND )
    {
        // Yes
        // Is it for me?
        if ( expected_target_entity_id
             == aem.aecpdu_header.header.target_entity_id )
        {
            r = true;
        }
    }
    return r;
}

bool isAEMForController( jdksavdecc_aecpdu_aem const &aem,
                         Eui64 const &expected_controller_entity_id )
{
    bool r = false;
    // Is it an AEM_RESPONSE?
    if ( aem.aecpdu_header.header.message_type
         == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE )
    {
        // Yes
        // Is it for me?
        if ( expected_controller_entity_id
             == aem.aecpdu_header.controller_entity_id )
        {
            r = true;
        }
    }
    return r;
}

inline void setAEMReply( uint8_t status_code, uint16_t new_length, Frame &pdu )
{
    // offset 1: sv=0, version=0, control_data = AEM_RESPONSE
    pdu.setOctet( JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE,
                  JDKSAVDECC_FRAME_HEADER_LEN + 1 );
    // new control data length is new_length minus frame beginning position pos
    // and the common control header.
    // See IEEE 1722-2011 Clause 5.3.3
    uint16_t control_data_length = new_length - JDKSAVDECC_FRAME_HEADER_LEN
                                   - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN;
    // offset 2: status = status code, top 3 bits of new control_data_length
    pdu.setOctet( ( status_code << 3 ) + ( ( control_data_length >> 8 ) & 0x7 ),
                  JDKSAVDECC_FRAME_HEADER_LEN + 2 );
    // offset 3: bottom 8 bits of new control_data_length

    pdu.setOctet( uint8_t( control_data_length & 0xff ),
                  JDKSAVDECC_FRAME_HEADER_LEN + 3 );
}

bool parseAA( jdksavdecc_aecp_aa *aa, Frame const &pdu )
{
    bool r = false;
    // Validate subtype is AA
    if ( pdu.getOctet( JDKSAVDECC_FRAME_HEADER_LEN )
         == JDKSAVDECC_1722A_SUBTYPE_AECP )
    {
        // Yes, read the aa header

        if ( jdksavdecc_aecp_aa_read( aa,
                                      pdu.getBuf(),
                                      JDKSAVDECC_FRAME_HEADER_LEN,
                                      pdu.getLength() ) > 0 )
        {
            // make sure it is version 0 and an AA_COMMAND or AA_RESPONSE
            if ( aa->aecpdu_header.header.version == 0
                 && ( aa->aecpdu_header.header.message_type
                      == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND
                      || aa->aecpdu_header.header.message_type
                         == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE ) )
            {
                r = true;
            }
        }
    }
    return r;
}

bool isAAForTarget( jdksavdecc_aecp_aa const &aa,
                    Eui64 const &expected_target_entity_id )
{
    bool r = false;

    // Is it an AA_COMMAND?
    if ( aa.aecpdu_header.header.message_type
         == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND )
    {
        // Yes
        // Is it for me?
        if ( expected_target_entity_id
             == aa.aecpdu_header.header.target_entity_id )
        {
            r = true;
        }
    }
    return r;
}

bool isAAForController( jdksavdecc_aecp_aa const &aa,
                        Eui64 const &expected_controller_entity_id )
{
    bool r = false;

    // Is it an AA_RESPONSE?
    if ( aa.aecpdu_header.header.message_type
         == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE )
    {
        // Yes
        // Is it for me?
        if ( expected_controller_entity_id == aa.controller_entity_id )
        {
            r = true;
        }
    }
    return r;
}

void setAAReply( uint8_t status_code,
                 uint16_t new_length,
                 uint8_t *buf,
                 uint16_t pos,
                 uint16_t len )
{
    if ( len > pos + 2 )
    {
        // offset 1: sv=0, version=0, control_data = ADDRESS_ACCESS_RESPONSE
        buf[pos + 1] = JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE;
        // new control data length is new_length minus frame beginning position
        // pos and the common control header.
        // See IEEE 1722-2011 Clause 5.3.3
        uint16_t control_data_length = new_length - pos
                                       - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN;
        // offset 2: status = status code, top 3 bits of new control_data_length
        buf[pos + 2] = ( status_code << 3 )
                       + ( ( control_data_length >> 8 ) & 0x7 );
        // offset 3: bottom 8 bits of new control_data_length
        buf[pos + 3] = ( control_data_length & 0xff );
    }
}

bool parseACMP( jdksavdecc_acmpdu *acmpdu, Frame const &pdu )
{
    bool r = false;
    // Validate subtype is ACMP
    if ( pdu.getOctet( JDKSAVDECC_FRAME_HEADER_LEN )
         == JDKSAVDECC_1722A_SUBTYPE_ACMP )
    {
        // Yes, read the acmp message

        if ( jdksavdecc_acmpdu_read( acmpdu,
                                     pdu.getBuf(),
                                     JDKSAVDECC_FRAME_HEADER_LEN,
                                     pdu.getLength() ) > 0 )
        {
            // make sure it is version 0 and an AA_COMMAND or AA_RESPONSE
            if ( acmpdu->header.version == 0
                 && acmpdu->header.control_data_length
                    >= ( JDKSAVDECC_ACMPDU_LEN
                         - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN ) )
            {
                r = true;
            }
        }
    }
    return r;
}

bool isACMPInvolvingTarget( jdksavdecc_acmpdu const &acmpdu,
                            Eui64 const &entity_id )
{
    bool r = false;

    if ( ( acmpdu.controller_entity_id == entity_id )
         || ( acmpdu.talker_entity_id == entity_id )
         || ( acmpdu.listener_entity_id == entity_id ) )
    {
        r = true;
    }

    return r;
}
}
