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
#include "JDKSAvdeccMCU_ADPManager.hpp"
#include "JDKSAvdeccMCU_Helpers.hpp"

namespace JDKSAvdeccMCU
{

ADPManager::ADPManager( RawSocket &net,
                        Eui64 const &entity_id,
                        Eui64 const &entity_model_id,
                        uint32_t entity_capabilities,
                        uint32_t controller_capabilities,
                        uint16_t valid_time_in_seconds,
                        uint16_t talker_stream_sources,
                        uint16_t talker_capabilities,
                        uint16_t listener_stream_sinks,
                        uint16_t listener_capabilities )
    : m_net( net )
    , m_entity_id( entity_id )
    , m_entity_model_id( entity_model_id )
    , m_entity_capabilities( entity_capabilities )
    , m_controller_capabilities( controller_capabilities )
    , m_valid_time_in_seconds( valid_time_in_seconds )
    , m_talker_stream_sources( talker_stream_sources )
    , m_talker_capabilities( talker_capabilities )
    , m_listener_stream_sinks( listener_stream_sinks )
    , m_listener_capabilities( listener_capabilities )
    , m_available_index( 0 )
    , m_last_send_time_in_millis( 0 )
    , m_trigger_send_time( 0 )
    , m_trigger_send( false )
    , m_gptp_grandmaster_id()
{
}

void ADPManager::tick( jdksavdecc_timestamp_in_milliseconds time_in_millis )
{
    // figure out if it is a scheduled time to send
    bool timeouthit
        = wasTimeOutHit( time_in_millis,
                         m_last_send_time_in_millis,
                         ( m_valid_time_in_seconds * ( 1000 / 4 ) ) );

    // figure out if we were triggered to send
    bool triggered
        = m_trigger_send
          && wasTimeOutHit( time_in_millis, m_trigger_send_time, 1000 );

    if ( triggered || timeouthit )
    {
        m_trigger_send = false;
        sendADP();
        m_last_send_time_in_millis = time_in_millis;
    }
}

void ADPManager::sendADP()
{
    Eui48 adp_multicast_addr = JDKSAVDECC_MULTICAST_ADP_ACMP_MAC;

    // DA, SA, EtherType, ADPDU = 82 bytes
    FrameWithSize<82> adp( 0,
                           adp_multicast_addr,
                           m_net.getMACAddress(),
                           JDKSAVDECC_AVTP_ETHERTYPE );

    // avtpdu common control header
    // cd=1, subtype=0x7a (ADP)
    adp.putOctet( 0x80 + JDKSAVDECC_SUBTYPE_ADP );

    // sv=0, version=0, message_type = ENTITY_AVAILABLE
    adp.putOctet( 0x00 + JDKSAVDECC_ADP_MESSAGE_TYPE_ENTITY_AVAILABLE );

    // valid_time is in 2 second steps. top 3 bits of control_data_length is 0
    adp.putOctet( ( m_valid_time_in_seconds / 2 ) << 3 );

    // control_data_length field is 56 - See 1722.1 Clause 6.2.1.7
    adp.putOctet( JDKSAVDECC_ADPDU_LEN - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN );

    adp.putEUI64( m_entity_id );
    adp.putEUI64( m_entity_model_id );
    adp.putQuadlet( m_entity_capabilities );

    adp.putDoublet( m_talker_stream_sources );
    adp.putDoublet( m_talker_capabilities );

    adp.putDoublet( m_listener_stream_sinks );
    adp.putDoublet( m_listener_capabilities );

    adp.putQuadlet( m_controller_capabilities );

    adp.putQuadlet( m_available_index );

    adp.putEUI64( m_gptp_grandmaster_id );

    // gptp_domain_number reserved0, identify_control_index, interface_index
    // association_id, reserved1
    // 20 octets total, all 0
    adp.putZeros( 20 );

    m_net.sendFrame( adp );
    m_available_index++;
}

void ADPManager::triggerSend()
{
    jdksavdecc_timestamp_in_milliseconds t = m_net.getTimeInMilliseconds();

    // were we already triggered to send?
    if ( m_trigger_send )
    {
        // Yes, Do nothing, wait for it.
    }
    else
    {
        // No, schedule a send in one second
        m_trigger_send = true;
        m_trigger_send_time = t;
    }
}

void ADPManager::setGPTPGrandMasterID( const Eui64 &new_gm )
{
    if ( Eui64_compare( m_gptp_grandmaster_id, new_gm ) != 0 )
    {
        m_gptp_grandmaster_id = new_gm;
        triggerSend();
    }
}

bool ADPManager::receivedPDU( Frame &frame )
{
    bool r = false;
    uint8_t *p = frame.getBuf();
    jdksavdecc_adpdu_common_control_header header;
    if ( jdksavdecc_adpdu_common_control_header_read(
             &header, p, JDKSAVDECC_FRAME_HEADER_LEN, frame.getLength() ) > 0 )
    {
        r = true;
        if ( header.message_type
             == JDKSAVDECC_ADP_MESSAGE_TYPE_ENTITY_DISCOVER )
        {
            if ( Eui64_compare( header.entity_id, m_entity_id )
                 == 0 || Eui64_is_unset( header.entity_id )
                 || Eui64_is_zero( header.entity_id ) )
            {
                m_last_send_time_in_millis
                    -= ( m_valid_time_in_seconds * ( 1000 / 4 ) );
            }
        }
        else if ( header.message_type
                  == JDKSAVDECC_ADP_MESSAGE_TYPE_ENTITY_AVAILABLE )
        {
            receivedEntityAvailable( header, frame );
        }
        else if ( header.message_type
                  == JDKSAVDECC_ADP_MESSAGE_TYPE_ENTITY_DEPARTING )
        {
            receivedEntityDeparting( header, frame );
        }
    }
    return r;
}
}
