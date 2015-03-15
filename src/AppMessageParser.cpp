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
#include "JDKSAvdeccMCU/AppMessageParser.hpp"

namespace JDKSAvdeccMCU
{

int AppMessageParser::dispatchMsg( const AppMessage &msg )
{
    int r = 0;
    switch ( msg.m_appdu.base.message_type )
    {
    case AppMessage::NOP:
        m_handler.onAppNop( msg );
        break;
    case AppMessage::ENTITY_ID_REQUEST:
        m_handler.onAppEntityIdRequest( msg );
        break;
    case AppMessage::ENTITY_ID_RESPONSE:
        m_handler.onAppEntityIdResponse( msg );
        break;
    case AppMessage::LINK_UP:
        m_handler.onAppLinkUp( msg );
        break;
    case AppMessage::LINK_DOWN:
        m_handler.onAppLinkDown( msg );
        break;
    case AppMessage::AVDECC_FROM_APS:
        m_handler.onAppAvdeccFromAps( msg );
        break;
    case AppMessage::AVDECC_FROM_APC:
        m_handler.onAppAvdeccFromApc( msg );
        break;
    case AppMessage::VENDOR:
        m_handler.onAppVendor( msg );
        break;
    default:
        m_handler.onAppUnknown( msg );
        break;
    }
    return r;
}

int AppMessageParser::parse( uint8_t octet )
{
    int r = 0;
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

    if ( msg )
    {
        dispatchMsg( *msg );
        r = 0;
    }

    if ( m_error_count > 0 )
    {
        r = -1;
    }

    return r;
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

        p->version = m_header_buffer.getOctet( JDKSAVDECC_APPDU_OFFSET_VERSION );

        p->message_type = m_header_buffer.getOctet( JDKSAVDECC_APPDU_OFFSET_MESSAGE_TYPE );

        p->payload_length = m_header_buffer.getDoublet( JDKSAVDECC_APPDU_OFFSET_PAYLOAD_LENGTH );

        p->address = m_header_buffer.getEUI48( JDKSAVDECC_APPDU_OFFSET_ADDRESS );

        p->reserved = m_header_buffer.getDoublet( JDKSAVDECC_APPDU_OFFSET_RESERVED );

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
        case AppMessage::NOP:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.1
            if ( p->payload_length == 0 && p->reserved == 0 && Eui48( p->address ).isZero() )
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
        case AppMessage::ENTITY_ID_REQUEST:
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
        case AppMessage::ENTITY_ID_RESPONSE:
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
        case AppMessage::LINK_UP:
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
        case AppMessage::LINK_DOWN:
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
        case AppMessage::AVDECC_FROM_APS:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.6
            if ( p->payload_length <= JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH && p->reserved == 0 )
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
        case AppMessage::AVDECC_FROM_APC:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.7
            if ( p->payload_length <= JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH && p->reserved == 0 )
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
        case AppMessage::VENDOR:
            /// See IEEE Std 1722.1-2013 Annex C.5.1.8
            if ( p->payload_length <= JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH && p->reserved == 0 )
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
