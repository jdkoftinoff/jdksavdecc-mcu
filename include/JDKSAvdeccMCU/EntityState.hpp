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
#include "JDKSAvdeccMCU/RawSocket.hpp"
#include "JDKSAvdeccMCU/Handler.hpp"
#include "JDKSAvdeccMCU/Helpers.hpp"
#include "JDKSAvdeccMCU/Frame.hpp"

namespace JDKSAvdeccMCU
{
class Entity;

class EntityState : public Handler
{
  public:
    EntityState() {}
    virtual ~EntityState();

    /// Run periodic state machines
    virtual void tick( jdksavdecc_timestamp_in_milliseconds time_in_millis ) override;

    virtual bool receivedPDU( RawSocket *incoming_socket, Frame &frame ) override;

    /// The pdu contains a valid Lock Entity command.
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveLockEntityCommand( Frame &pdu );

    /// The pdu contains a valid Read Descriptor Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveReadDescriptorCommand( Frame &pdu,
                                                  uint16_t configuration_index,
                                                  uint16_t descriptor_type,
                                                  uint16_t descriptor_index );

    /// The pdu contains a valid Set Configuration Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveSetConfigurationCommand( Frame &pdu );

    /// The pdu contains a valid Get Configuration Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveGetConfigurationCommand( Frame &pdu );

    /// The pdu contains a valid Set Name Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        receiveSetNameCommand( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_type, uint16_t descriptor_index );

    /// The pdu contains a valid Get Name Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        receiveGetNameCommand( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_type, uint16_t descriptor_index );

    /// The pdu contains a valid Set Control Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveSetControlCommand( Frame &pdu, uint16_t descriptor_index );

    /// The pdu contains a valid Get Control Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveGetControlCommand( Frame &pdu, uint16_t descriptor_index );

    uint8_t fillDescriptorEntity( Frame &pdu,
                                  Eui64 const &entity_id,
                                  Eui64 const &entity_model_id,
                                  uint32_t entity_capabilities,
                                  uint16_t talker_stream_sources,
                                  uint16_t talker_capabilities,
                                  uint16_t listener_stream_sinks,
                                  uint16_t listener_capabilities,
                                  uint32_t controller_capabilities,
                                  uint32_t available_index,
                                  const char *entity_name,
                                  uint16_t vendor_name_string,
                                  uint16_t model_name_string,
                                  const char *firmware_version,
                                  const char *group_name,
                                  const char *serial_number,
                                  uint16_t configurations_count,
                                  uint16_t current_configuration )
    {
        pdu.setLength( JDKSAVDECC_FRAME_HEADER_LEN + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_COMMAND_OFFSET_DESCRIPTOR_TYPE );

        // descriptor_type
        pdu.putDoublet( JDKSAVDECC_DESCRIPTOR_ENTITY );

        // descriptor_index
        pdu.putDoublet( 0 );

        pdu.putEUI64( entity_id );
        pdu.putEUI64( entity_model_id );
        pdu.putQuadlet( entity_capabilities );
        pdu.putDoublet( talker_stream_sources );
        pdu.putDoublet( talker_capabilities );
        pdu.putDoublet( listener_stream_sinks );
        pdu.putDoublet( listener_capabilities );
        pdu.putQuadlet( controller_capabilities );
        pdu.putQuadlet( available_index );

        // association_id = 0
        pdu.putEUI64();
        pdu.putAvdeccString( entity_name );
        pdu.putDoublet( vendor_name_string );
        pdu.putDoublet( model_name_string );
        pdu.putAvdeccString( firmware_version );
        pdu.putAvdeccString( group_name );
        pdu.putAvdeccString( serial_number );
        pdu.putDoublet( configurations_count );
        pdu.putDoublet( current_configuration );

        return pdu.isFull() ? JDKSAVDECC_AEM_STATUS_ENTITY_MISBEHAVING : JDKSAVDECC_AEM_STATUS_SUCCESS;
    }

    /// The pdu contains a valid Read Entity Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorEntity( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index );

    /// The pdu contains a valid Read AVB_INTERFACE descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorAvbInterface( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index );

    /// The pdu contains a valid Read Configuration Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorConfiguration( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index );

    /// The pdu contains a valid Read Control Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorControl( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index );

    /// The pdu contains a valid Read Locale Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorLocale( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index );

    /// The pdu contains a valid Read Strings Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorStrings( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index );

    /// The pdu contains a valid Read Memory Object Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorMemoryObject( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index );

    /// The pdu contains a valid Read Address Access TLV command
    /// Fill in the response in place in the pdu and return an AECP AA status
    /// code
    virtual uint8_t receiveAARead( uint32_t virtual_base_address, uint16_t length, uint8_t *response );

    /// The pdu contains a valid Write Address Access TLV command
    /// Fill in the response in place in the pdu and return an AECP AA status
    /// code
    virtual uint8_t receiveAAWrite( uint32_t virtual_base_address, uint16_t length, uint8_t const *request );

    /// The pdu contains a valid Execute Address Access TLV command
    /// Fill in the response in place in the pdu and return an AECP AA status
    /// code
    virtual uint8_t receiveAAExecute( uint32_t virtual_base_address, uint16_t length, uint8_t const *request );
};
}
