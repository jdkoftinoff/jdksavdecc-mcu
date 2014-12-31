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
#include "JDKSAvdeccMCU_FixedBuffer.hpp"
#include "JDKSAvdeccMCU_Frame.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief The AppMessage AVDECC Proxy Protocol Message
///
/// See IEEE Std 1722.1-2013 Annex C.4 and Annex C.5
///
struct AppMessage
{
    ///
    /// \brief The MessageType enum
    ///
    /// See IEEE Std 1722.1-2013 Annex C.4.2
    ///
    enum MessageType
    {
        NOP = 0,
        ENTITY_ID_REQUEST = 1,
        ENTITY_ID_RESPONSE = 2,
        LINK_UP = 3,
        LINK_DOWN = 4,
        AVDECC_FROM_APS = 5,
        AVDECC_FROM_APC = 6,
        VENDOR = 0xff
    };

    ///
    /// \brief AppMessage Constructor
    ///
    /// Creates an AppMessage containing a NOP
    ///
    AppMessage()
    {
        jdksavdecc_fullappdu_init( &m_appdu );
        setNOP();
    }

    virtual ~AppMessage() {}

    ///
    /// \brief AppMessage Copy Constructor
    /// \param other the AppMessage to copy
    ///
    AppMessage( const AppMessage &other ) : m_appdu( other.m_appdu )
    {
        m_appdu.base.payload = m_appdu.payload_buffer;
    }

    ///
    /// \brief operator =
    /// Assignment operator
    /// \param other the AppMessage to copy
    /// \return *this
    ///
    const AppMessage &operator=( const AppMessage &other )
    {
        m_appdu = other.m_appdu;
        m_appdu.base.payload = m_appdu.payload_buffer;
        return *this;
    }

    ///
    /// \brief clear
    ///
    /// Alias for setNOP
    ///
    void clear() { setNOP(); }

    ///
    /// \brief setNOP
    ///
    /// Set the message type to NOP
    /// See IEEE Std 1722.1-2013 Annex C.5.1.1
    ///
    void setNOP() { jdksavdecc_appdu_set_nop( &m_appdu.base ); }

    ///
    /// \brief setEntityIdRequest
    ///
    /// Set the message type to ENTITY_ID_REQUEST
    /// See IEEE Std 1722.1-2013 Annex C.5.1.2
    ///
    /// \param apc_primary_mac
    /// \param requested_entity_id
    ///
    void setEntityIdRequest( const Eui48 &apc_primary_mac,
                             const Eui64 &requested_entity_id )
    {
        jdksavdecc_appdu_set_entity_id_request(
            &m_appdu.base, apc_primary_mac, requested_entity_id );
    }

    ///
    /// \brief setEntityIdResponse
    ///
    /// Set the message type to ENTITY_ID_RESPONSE
    /// See IEEE Std 1722.1-2013 Annex C.5.1.3
    ///
    /// \param apc_primary_mac
    /// \param requested_entity_id
    ///
    void setEntityIdResponse( const Eui48 &apc_primary_mac,
                              const Eui64 &requested_entity_id )
    {
        jdksavdecc_appdu_set_entity_id_response(
            &m_appdu.base, apc_primary_mac, requested_entity_id );
    }

    ///
    /// \brief setLinkUp
    ///
    /// Set the message type to LINK_UP
    /// See IEEE Std 1722.1-2013 Annex C.5.1.4
    ///
    /// \param network_port_mac
    ///
    void setLinkUp( const Eui48 &network_port_mac )
    {
        jdksavdecc_appdu_set_link_up( &m_appdu.base, network_port_mac );
    }

    ///
    /// \brief setLinkDown
    ///
    /// Set the message type to LINK_DOWN
    /// See IEEE Std 1722.1-2013 Annex C.5.1.5
    ///
    /// \param network_port_mac
    ///
    void setLinkDown( const Eui48 &network_port_mac )
    {
        jdksavdecc_appdu_set_link_down( &m_appdu.base, network_port_mac );
    }

    ///
    /// \brief setAvdeccFromAps
    ///
    /// Set the message type to AVDECC_FROM_APS
    /// See IEEE Std 1722.1-2013 Annex C.5.1.6
    ///
    /// \param frame The AVDECC message from APS
    /// to be encapsulated into APPDU
    ///
    void setAvdeccFromAps( const Frame &frame )
    {
        jdksavdecc_appdu_set_avdecc_from_aps( &m_appdu.base,
                                              frame.getSA(),
                                              frame.getPayloadLength(),
                                              frame.getPayload() );
    }

    ///
    /// \brief setAvdeccFromApc
    ///
    /// Set the message type to AVDECC_FROM_APC
    /// See IEEE Std 1722.1-2013 Annex C.5.1.7
    ///
    /// \param frame The AVDECC message from APC
    /// to be encapsulated into APPDU
    ///
    void setAvdeccFromApc( const Frame &frame )
    {
        jdksavdecc_appdu_set_avdecc_from_apc( &m_appdu.base,
                                              frame.getSA(),
                                              frame.getPayloadLength(),
                                              frame.getPayload() );
    }

    ///
    /// \brief setVendor
    ///
    /// Set the message type to VENDOR
    /// See IEEE Std 1722.1-2013 Annex C.5.1.8
    ///
    /// \param vendor_message_type Eui48 code
    /// \param payload the vendor specific message data
    ///
    void setVendor( const Eui48 &vendor_message_type,
                    const FixedBuffer &payload )
    {
        jdksavdecc_appdu_set_vendor( &m_appdu.base,
                                     vendor_message_type,
                                     payload.getLength(),
                                     payload.getBuf() );
    }

    uint8_t const *getPayload() const { return m_appdu.payload_buffer; }

    uint8_t *getPayload() { return m_appdu.payload_buffer; }

    Eui48 getAddress() const { return Eui48( m_appdu.base.address ); }

    uint16_t getPayloadLength() const { return m_appdu.base.payload_length; }

    uint8_t getVersion() const { return m_appdu.base.version; }

    MessageType getMessageType() const
    {
        return MessageType( m_appdu.base.message_type );
    }

    Eui64 getEntityIdRequestEntityId() const
    {
        Eui64 r;
        if ( getMessageType() == ENTITY_ID_REQUEST && getPayloadLength() == 8 )
        {
            r = Eui64( m_appdu.payload_buffer );
        }
        return r;
    }

    Eui64 getEntityIdResponseEntityId() const
    {
        Eui64 r;
        if ( getMessageType() == ENTITY_ID_RESPONSE && getPayloadLength() == 8 )
        {
            r = Eui64( m_appdu.payload_buffer );
        }
        return r;
    }

    bool store( FixedBuffer *dest ) const
    {
        ssize_t r;
        dest->clear();
        r = jdksavdecc_appdu_write(
            &m_appdu.base, dest->getBuf(), 0, dest->getMaxLength() );
        if ( r > 0 )
        {
            dest->setLength( uint16_t( r ) );
        }
        return r > 0;
    }

    ///
    /// \brief m_appdu
    /// The parsed header and additional payload storage
    ///
    jdksavdecc_fullappdu m_appdu;
};
}
