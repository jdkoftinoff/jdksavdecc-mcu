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
#include "JDKSAvdeccMCU_RawSocket.hpp"
#include "JDKSAvdeccMCU_Frame.hpp"
#include "JDKSAvdeccMCU_Handler.hpp"
#include "JDKSAvdeccMCU_ControlValueHolder.hpp"
#include "JDKSAvdeccMCU_Helpers.hpp"

namespace JDKSAvdeccMCU
{

template <size_t MaxDescriptors>
class ControlReceiver : public Handler
{
  public:
    /// Construct the ControlReceiver object
    ControlReceiver( RawSocket &net,
                     jdksavdecc_eui64 const &entity_id,
                     uint16_t descriptor_index_offset = 0 )
        : m_net( net )
        , m_entity_id( entity_id )
        , m_descriptor_index_offset( descriptor_index_offset )
        , m_num_descriptors( 0 )
        , m_last_sequence_id( 0xffff )
    {
    }

    /// Register the ControlValueHolder to relate to the next descriptor_index
    /// in line
    void addDescriptor( ControlValueHolder *v )
    {
        m_values[m_num_descriptors++] = v;
    }

    virtual bool receivedGetControlCommand( jdksavdecc_aecpdu_aem const &aem,
                                            Frame &pdu )
    {
        bool r = false;
        // yes, get the descriptor index
        uint16_t descriptor_index
            = jdksavdecc_aem_command_set_control_get_descriptor_index(
                pdu.getBuf(), pdu.getPosition() );
        (void)aem;
        // is it a descriptor index that we care about?
        if ( ( descriptor_index >= m_descriptor_index_offset )
             && ( descriptor_index
                  < ( m_descriptor_index_offset + m_num_descriptors ) ) )
        {
            // yes, it is in range, extract the data value

            // TODO Add GetValue method in ControlValueHolder.
            //            m_values[descriptor_index-m_descriptor_index_offset]->SetValue(
            //                                                                           &buf[pos+JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_OFFSET_VALUES]
            //                                                                           );
            // TODO: Set control_data_length appropriately.

            r = true;
        }
        return r;
    }

    virtual bool receivedSetControlCommand( jdksavdecc_aecpdu_aem const &aem,
                                            Frame &pdu )
    {
        bool r = false;
        // yes, get the descriptor index
        uint16_t descriptor_index
            = jdksavdecc_aem_command_set_control_get_descriptor_index(
                pdu.getBuf(), pdu.getPosition() );
        (void)aem;
        // is it a descriptor index that we care about?
        if ( ( descriptor_index >= m_descriptor_index_offset )
             && ( descriptor_index
                  < ( m_descriptor_index_offset + m_num_descriptors ) ) )
        {
            // yes, it is in range, extract the data value

            m_values[descriptor_index - m_descriptor_index_offset]->setValue(
                pdu.getBuf() + pdu.getPosition()
                + JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_OFFSET_VALUES );

            r = true;
        }
        return r;
    }

    virtual bool
        receivedReadDescriptorCommand( jdksavdecc_aecpdu_aem const &aem,
                                       Frame &pdu )
    {
        bool r = false;
        (void)aem;
        (void)pdu;
        return r;
    }

    /// Handle incoming PDU
    virtual bool
        receivedPDU( jdksavdecc_timestamp_in_milliseconds time_in_millis,
                     uint8_t *buf,
                     uint16_t len )
    {
        bool r = false;
        // we already know the message is AVTP ethertype and is either directly
        // targetting my MAC address or is a multicast message

        Frame pdu( time_in_millis, buf, len );

        // Try see if it is an AEM message
        jdksavdecc_aecpdu_aem aem;
        if ( parseAEM( &aem, pdu ) )
        {
            // Yes, Is it a command for me?
            if ( isAEMForTarget( aem, m_entity_id ) )
            {
                // Yes. Is the sequence_id ok?
                if ( aem.aecpdu_header.sequence_id != m_last_sequence_id )
                {
                    // Yes, it is different.
                    m_last_sequence_id = aem.aecpdu_header.sequence_id;
                    if ( aem.command_type
                         == JDKSAVDECC_AEM_COMMAND_GET_CONTROL )
                    {
                        // Handle GET_CONTROL commands
                        r = receivedGetControlCommand( aem, pdu );
                    }
                    else if ( aem.command_type
                              == JDKSAVDECC_AEM_COMMAND_SET_CONTROL )
                    {
                        // Handle SET_CONTROL commands
                        r = receivedSetControlCommand( aem, pdu );
                    }
                    else if ( aem.command_type
                              == JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR )
                    {
                        // Handle READ_DESCRIPTOR commands
                        r = receivedReadDescriptorCommand( aem, pdu );
                    }
                }
            }
        }
        if ( r )
        {
            // TODO: change message type to AEM_RESPONSE and send it back to
            // sender
        }
        return r;
    }

    /// Get the enitity ID that we are handling
    jdksavdecc_eui64 const &getEntityID() const { return m_entity_id; }

  protected:
    RawSocket &m_net;
    jdksavdecc_eui64 const &m_entity_id;
    uint16_t m_descriptor_index_offset;
    uint16_t m_num_descriptors;
    ControlValueHolder *m_values[MaxDescriptors];
    uint16_t m_last_sequence_id;
};
}
