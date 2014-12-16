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
#include "JDKSAvdeccMCU_App.hpp"

namespace JDKSAvdeccMCU
{

AppMessage::AppMessage()
{
    jdksavdecc_fullappdu_init( &m_appdu );
    jdksavdecc_appdu_set_nop( &m_appdu.base );
}

AppMessage::AppMessage( const AppMessage &other ) : m_appdu( other.m_appdu )
{
    m_appdu.base.payload = m_appdu.payload_buffer;
}

const AppMessage &AppMessage::operator=( const AppMessage &other )
{
    m_appdu = other.m_appdu;
    m_appdu.base.payload = m_appdu.payload_buffer;
    return *this;
}

void AppMessage::setNOP() { jdksavdecc_appdu_set_nop( &m_appdu.base ); }

void AppMessage::setEntityIdRequest(
    const JDKSAvdeccMCU::Eui48 &apc_primary_mac,
    const JDKSAvdeccMCU::Eui64 &requested_entity_id )
{
    jdksavdecc_appdu_set_entity_id_request(
        &m_appdu.base, apc_primary_mac, requested_entity_id );
}

void AppMessage::setEntityIdResponse(
    const JDKSAvdeccMCU::Eui48 &apc_primary_mac,
    const JDKSAvdeccMCU::Eui64 &requested_entity_id )
{
    jdksavdecc_appdu_set_entity_id_response(
        &m_appdu.base, apc_primary_mac, requested_entity_id );
}

void AppMessage::setLinkUp( const JDKSAvdeccMCU::Eui48 &network_port_mac )
{
    jdksavdecc_appdu_set_link_up( &m_appdu.base, network_port_mac );
}

void AppMessage::setLinkDown( const JDKSAvdeccMCU::Eui48 &network_port_mac )
{
    jdksavdecc_appdu_set_link_down( &m_appdu.base, network_port_mac );
}

void AppMessage::setAvdeccFromAps( const JDKSAvdeccMCU::Frame &frame )
{
    jdksavdecc_appdu_set_avdecc_from_aps( &m_appdu.base,
                                          frame.getSA(),
                                          frame.getPayloadLength(),
                                          frame.getPayload() );
}

void AppMessage::setAvdeccFromApc( const JDKSAvdeccMCU::Frame &frame )
{
    jdksavdecc_appdu_set_avdecc_from_apc( &m_appdu.base,
                                          frame.getSA(),
                                          frame.getPayloadLength(),
                                          frame.getPayload() );
}

void AppMessage::setVendor( const JDKSAvdeccMCU::Eui48 &vendor_message_type,
                            const JDKSAvdeccMCU::FixedBuffer &payload )
{
    jdksavdecc_appdu_set_vendor( &m_appdu.base,
                                 vendor_message_type,
                                 payload.getLength(),
                                 payload.getBuf() );
}

AppMessageParser::AppMessageParser()
    : m_octets_left_in_payload( 0 ), m_error_count( 0 )
{
}

AppMessage *AppMessageParser::parse( uint8_t octet )
{
    AppMessage *msg = 0;

    // Is there room in the header buffer?
    if ( m_header_buffer.canPut() )
    {
        // yes, try parse the header
        msg = parseHeader( octet );
    }
    else
    {
        // is there octets we are expecting for the payload?
        if ( m_octets_left_in_payload )
        {
            // yes, parse the payload
            msg = parsePayload( octet );
        }
    }

    return msg;
}

AppMessage *AppMessageParser::parseHeader( uint8_t octet )
{
    AppMessage *msg = 0;

    // store the header octet
    m_header_buffer.putOctet( octet );

    // is the header buffer full now?
    if ( m_header_buffer.isFull() )
    {
        // yes, try parse the header

        jdksavdecc_appdu *p = &m_current_message.m_appdu.base;

        p->version
            = m_header_buffer.getOctet( JDKSAVDECC_APPDU_OFFSET_VERSION );

        p->message_type
            = m_header_buffer.getOctet( JDKSAVDECC_APPDU_OFFSET_MESSAGE_TYPE );

        p->payload_length = m_header_buffer.getDoublet(
            JDKSAVDECC_APPDU_OFFSET_PAYLOAD_LENGTH );

        p->address
            = m_header_buffer.getEUI48( JDKSAVDECC_APPDU_OFFSET_ADDRESS );

        p->reserved
            = m_header_buffer.getDoublet( JDKSAVDECC_APPDU_OFFSET_RESERVED );

        // and validate the header
        msg = validateHeader();
    }

    return msg;
}

AppMessage *AppMessageParser::validateHeader()
{
    AppMessage *msg = 0;
    jdksavdecc_appdu *p = &m_current_message.m_appdu.base;

    // Is the version field recognized?
    if ( p->version == JDKSAVDECC_APPDU_VERSION )
    {
        switch ( p->message_type )
        {
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_NOP:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.1
            if ( p->payload_length == 0 && p->reserved == 0
                 && Eui48( p->address ).isZero() )
            {
                // Yes, we parsed the entire NOP message now.
                msg = &m_current_message;
                m_header_buffer.clear();
                m_octets_left_in_payload = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_ENTITY_ID_REQUEST:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.2
            if ( p->payload_length == 8 && p->reserved == 0 )
            {
                // We have 8 more payload bytes to go
                m_octets_left_in_payload = p->payload_length;
                // use p->payload_length as payload octet counter
                p->payload_length = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_ENTITY_ID_RESPONSE:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.3
            if ( p->payload_length == 8 && p->reserved == 0 )
            {
                // We have 8 more payload bytes to go
                m_octets_left_in_payload = p->payload_length;
                // use p->payload_length as payload octet counter
                p->payload_length = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_LINK_UP:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.4
            if ( p->payload_length == 0 && p->reserved == 0 )
            {
                // Yes, we parsed the entire LINK_UP message now.
                msg = &m_current_message;
                m_header_buffer.clear();
                m_octets_left_in_payload = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_LINK_DOWN:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.5
            if ( p->payload_length == 0 && p->reserved == 0 )
            {
                // Yes, we parsed the entire LINK_DOWN message now.
                msg = &m_current_message;
                m_header_buffer.clear();
                m_octets_left_in_payload = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_AVDECC_FROM_APS:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.6
            if ( p->payload_length <= JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH
                 && p->reserved == 0 )
            {
                // we have some payload to read now
                m_octets_left_in_payload = p->payload_length;
                // use p->payload_length as payload octet counter
                p->payload_length = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_AVDECC_FROM_APC:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.7
            if ( p->payload_length <= JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH
                 && p->reserved == 0 )
            {
                // we have some payload to read now
                m_octets_left_in_payload = p->payload_length;
                // use p->payload_length as payload octet counter
                p->payload_length = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        case JDKSAVDECC_APPDU_MESSAGE_TYPE_VENDOR:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.8
            if ( p->payload_length <= JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH
                 && p->reserved == 0 )
            {
                // we have some payload to read now
                m_octets_left_in_payload = p->payload_length;
                // use p->payload_length as payload octet counter
                p->payload_length = 0;
            }
            else
            {
                m_error_count++;
            }
            break;
        default:
            // Unrecognized message type
            m_error_count++;
            break;
        }
    }
    else
    {
        // We don't know this version
        m_error_count++;
    }
    return msg;
}

AppMessage *AppMessageParser::parsePayload( uint8_t octet )
{
    AppMessage *msg = 0;
    jdksavdecc_appdu *p = &m_current_message.m_appdu.base;

    // append the octet to the payload buffer
    p->payload[p->payload_length++] = octet;
    --m_octets_left_in_payload;

    // are we done?
    if ( m_octets_left_in_payload == 0 )
    {
        // yes, return the fully formed message
        msg = &m_current_message;

        // and clear our state to be ready for the next one
        m_header_buffer.clear();
    }
    return msg;
}
}
