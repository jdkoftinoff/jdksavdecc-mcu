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

#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/Frame.hpp"

namespace JDKSAvdeccMCU
{

inline bool wasTimeOutHit( jdksavdecc_timestamp_in_milliseconds cur_time,
                           jdksavdecc_timestamp_in_milliseconds last_time_done,
                           jdksavdecc_timestamp_in_milliseconds timeout )
{
    jdksavdecc_timestamp_in_milliseconds difftime = cur_time - last_time_done;
    return difftime > timeout;
}

///
/// \brief parseAEM Helper function to parse AECP AEM message
/// \param aem pointer to AECPDU AEM structure to fill in
/// \param frame reference to the Frame to parse
/// \return true if the message is an AECPDU AEM message
///
bool parseAEM( jdksavdecc_aecpdu_aem *aem, Frame const &rx );

///
/// \brief isAEMForTarget Test to see if AECPDU AEM message is a for target
/// \param aem AECPDU AEM structure to read
/// \param expected_target_entity_id Eui64 to test for
/// \return true if the AEM message is for the entity
///
bool isAEMForTarget( jdksavdecc_aecpdu_aem const &aem,
                     Eui64 const &expected_target_entity_id );

///
/// \brief isAEMForController Test if AEM message is response for a controller
/// \param aem AECPDU AEM structure to read
/// \param expected_controller_entity_id Controller Entity ID to test for
/// \return true if the message is AEM response for the specified controller
/// entity id
///
bool isAEMForController( jdksavdecc_aecpdu_aem const &aem,
                         Eui64 const &expected_controller_entity_id );

///
/// \brief setAEMReply Convert AEM command packet to an AEM response
///
/// Twiddle the header bytes to convert this message from AEM_COMMAND to
/// AEM_RESPONSE in place with the new status code, and the new
/// control_data_length to match the new_length
///
/// \param status_code The new AECPDU AEM status code to use
/// \param new_length The new length to use
/// \param buf The pointer to the base of the packet buffer
/// \param pos The position of the packet payload
/// \param len The length of the entire packet
///
void setAEMReply( uint8_t status_code,
                  uint16_t new_length,
                  uint8_t *buf,
                  uint16_t pos,
                  uint16_t len );

///
/// \brief parseAA Helper function to parse AECP AA message
/// \param aa Pointer to aecp aa structure to fill in
/// \param frame reference to the Frame to parse
/// \return true if the message is an AECPDU AA message
///
bool parseAA( jdksavdecc_aecp_aa *aa, Frame const &pdu );

/// Test to see if AECPDU AA message is a command for the specified target
/// entity
bool isAAForTarget( jdksavdecc_aecp_aa const &aa,
                    Eui64 const &expected_target_entity_id );

/// Test to see if AECPDU AA message is a response for the specified controller
/// entity
bool isAAForController( jdksavdecc_aecp_aa const &aa,
                        Eui64 const &expected_controller_entity_id );

/// Twiddle the header bytes to convert this message from ADDRESS_ACCESS_COMMAND
/// to ADDRESS_ACCESS_RESPONSE in place with the new status code, and the new
/// control_data_length to match the new_length
void
    setAAReply( uint16_t new_length, uint8_t *buf, uint16_t pos, uint16_t len );

///
/// \brief formAEMSetControl Formulate an AEM SET_CONTROL style message with
/// payload
/// \param frame Pointer to the Frame to clear and fill in
/// \param dest_mac Destination MAC address
/// \param src_mac Source MAC Address
/// \param my_controller_entity_id Controller Entity ID
/// \param destination_entity_id Destination Entity ID
/// \param sequence_id Sequence ID
/// \param unsolicited Unsolicited flag
/// \param descriptor_index Control Descriptor Index
/// \param value_data Pointer to control data
/// \param value_data_length length of control data
/// \return true if the formed message fit in the frame
///
inline bool formAEMSetControl( Frame *frame,
                               Eui48 const &dest_mac,
                               Eui48 const &src_mac,
                               Eui64 const &my_controller_entity_id,
                               Eui64 const &destination_entity_id,
                               uint16_t sequence_id,
                               bool unsolicited,
                               uint16_t descriptor_index,
                               uint8_t const *value_data,
                               uint16_t value_data_length )
{
    bool r = false;
    frame->setDA( dest_mac );
    frame->setSA( src_mac );
    frame->setEtherType( JDKSAVDECC_AVTP_ETHERTYPE );
    frame->setLength( JDKSAVDECC_FRAME_HEADER_LEN );
    frame->putOctet( JDKSAVDECC_1722A_SUBTYPE_AECP );
    frame->putOctet( JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );
    frame->putDoublet( ( JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN
                         + value_data_length
                         - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN ) & 0x7ff );
    frame->putEUI64( destination_entity_id );
    frame->putEUI64( my_controller_entity_id );
    frame->putDoublet( sequence_id );
    frame->putDoublet( JDKSAVDECC_AEM_COMMAND_SET_CONTROL
                       + ( unsolicited ? 0x8000 : 0 ) );
    frame->putDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
    frame->putDoublet( descriptor_index );
    r = frame->canPut( value_data_length );
    frame->putBuf( value_data, value_data_length );
    return r;
}

///
/// \brief formAEMGetControl Formulate an AEM GET_CONTROL style message with
/// payload
/// \param frame Pointer to the Frame to clear and fill in
/// \param dest_mac Destination MAC address
/// \param src_mac Source MAC Address
/// \param my_controller_entity_id Controller Entity ID
/// \param destination_entity_id Destination Entity ID
/// \param sequence_id Sequence ID
/// \param descriptor_index Control Descriptor Index
/// \return true if the formed message fit in the frame
///
inline bool formAEMGetControl( Frame *frame,
                               Eui48 const &dest_mac,
                               Eui48 const &src_mac,
                               Eui64 const &my_controller_entity_id,
                               Eui64 const &destination_entity_id,
                               uint16_t sequence_id,
                               uint16_t descriptor_index )
{
    bool r = false;
    frame->setDA( dest_mac );
    frame->setSA( src_mac );
    frame->setEtherType( JDKSAVDECC_AVTP_ETHERTYPE );
    frame->setLength( JDKSAVDECC_FRAME_HEADER_LEN );
    frame->putOctet( JDKSAVDECC_1722A_SUBTYPE_AECP );
    frame->putOctet( JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );
    frame->putDoublet( ( JDKSAVDECC_AEM_COMMAND_GET_CONTROL_COMMAND_LEN
                         - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN ) & 0x7ff );
    frame->putEUI64( destination_entity_id );
    frame->putEUI64( my_controller_entity_id );
    frame->putDoublet( sequence_id );
    frame->putDoublet( JDKSAVDECC_AEM_COMMAND_GET_CONTROL );
    frame->putDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
    r = frame->canPut( 2 );
    frame->putDoublet( descriptor_index );
    return r;
}

///
/// \brief formAEMSetControlResponse Formulate an AEM SET_CONTROL response style
/// message with payload
/// \param frame Pointer to the Frame to clear and fill in
/// \param dest_mac Destination MAC address
/// \param src_mac Source MAC Address
/// \param controller_entity_id Controller Entity ID
/// \param destination_entity_id Destination Entity ID
/// \param sequence_id Sequence ID
/// \param unsolicited Unsolicited flag
/// \param descriptor_index Control Descriptor Index
/// \param value_data Pointer to control data
/// \param value_data_length length of control data
/// \return true if the formed message fit in the frame
///
inline bool formAEMSetControlResponse( Frame *frame,
                                       Eui48 const &dest_mac,
                                       Eui48 const &src_mac,
                                       Eui64 const &controller_entity_id,
                                       Eui64 const &destination_entity_id,
                                       uint16_t sequence_id,
                                       bool unsolicited,
                                       uint16_t descriptor_index,
                                       uint8_t const *value_data,
                                       uint16_t value_data_length )
{
    bool r = false;
    frame->setDA( dest_mac );
    frame->setSA( src_mac );
    frame->setEtherType( JDKSAVDECC_AVTP_ETHERTYPE );
    frame->setLength( JDKSAVDECC_FRAME_HEADER_LEN );
    frame->putOctet( JDKSAVDECC_1722A_SUBTYPE_AECP );
    frame->putOctet( JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE );
    frame->putDoublet( ( JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN
                         + value_data_length
                         - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN ) & 0x7ff );
    frame->putEUI64( destination_entity_id );
    frame->putEUI64( controller_entity_id );
    frame->putDoublet( sequence_id );
    frame->putDoublet( JDKSAVDECC_AEM_COMMAND_SET_CONTROL
                       + ( unsolicited ? 0x8000 : 0 ) );
    frame->putDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
    frame->putDoublet( descriptor_index );
    r = frame->canPut( value_data_length );
    frame->putBuf( value_data, value_data_length );
    return r;
}

///
/// \brief formAEMGetControlResponse Formulate an AEM GET_CONTROL response style
/// message with payload
/// \param frame Pointer to the Frame to clear and fill in
/// \param dest_mac Destination MAC address
/// \param src_mac Source MAC Address
/// \param controller_entity_id Controller Entity ID
/// \param destination_entity_id Destination Entity ID
/// \param sequence_id Sequence ID
/// \param unsolicited Unsolicited flag
/// \param descriptor_index Control Descriptor Index
/// \param value_data Pointer to control data
/// \param value_data_length length of control data
/// \return true if the formed message fit in the frame
///
inline bool formAEMGetControlResponse( Frame *frame,
                                       Eui48 const &dest_mac,
                                       Eui48 const &src_mac,
                                       Eui64 const &controller_entity_id,
                                       Eui64 const &destination_entity_id,
                                       uint16_t sequence_id,
                                       bool unsolicited,
                                       uint16_t descriptor_index,
                                       uint8_t const *value_data,
                                       uint16_t value_data_length )
{
    bool r = false;
    frame->setDA( dest_mac );
    frame->setSA( src_mac );
    frame->setEtherType( JDKSAVDECC_AVTP_ETHERTYPE );
    frame->setLength( JDKSAVDECC_FRAME_HEADER_LEN );
    frame->putOctet( JDKSAVDECC_1722A_SUBTYPE_AECP );
    frame->putOctet( JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE );
    frame->putDoublet( ( JDKSAVDECC_AEM_COMMAND_GET_CONTROL_COMMAND_LEN
                         + value_data_length
                         - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN ) & 0x7ff );
    frame->putEUI64( destination_entity_id );
    frame->putEUI64( controller_entity_id );
    frame->putDoublet( sequence_id );
    frame->putDoublet( JDKSAVDECC_AEM_COMMAND_GET_CONTROL
                       + ( unsolicited ? 0x8000 : 0 ) );
    frame->putDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
    frame->putDoublet( descriptor_index );
    r = frame->canPut( value_data_length );
    frame->putBuf( value_data, value_data_length );
    return r;
}

///
/// \brief setAEMReply
/// \param status_code
/// \param new_length
/// \param pdu
///
void setAEMReply( uint8_t status_code, uint16_t new_length, Frame &pdu );

///
/// \brief setAAReply
/// \param status_code
/// \param new_length
/// \param buf
/// \param pos
/// \param len
///
void setAAReply( uint8_t status_code,
                 uint16_t new_length,
                 uint8_t *buf,
                 uint16_t pos,
                 uint16_t len );

///
/// \brief parseACMP
/// \param acmpdu
/// \param pdu
/// \return
///
bool parseACMP( jdksavdecc_acmpdu *acmpdu, Frame const &pdu );

///
/// \brief isACMPInvolvingTarget
/// \param acmpdu
/// \param entity_id
/// \return
///
bool isACMPInvolvingTarget( jdksavdecc_acmpdu const &acmpdu,
                            Eui64 const &entity_id );
}
