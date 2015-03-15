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

#include "JDKSAvdeccMCU/EntityState.hpp"
#include "JDKSAvdeccMCU/Entity.hpp"

namespace JDKSAvdeccMCU
{

EntityState::~EntityState() {}

void EntityState::tick( jdksavdecc_timestamp_in_milliseconds time_in_millis ) { (void)time_in_millis; }

bool EntityState::receivedPDU( Frame &frame )
{
    (void)frame;
    return false;
}

uint8_t EntityState::receiveLockEntityCommand( Frame &pdu )
{
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveReadDescriptorCommand( Frame &pdu,
                                                   uint16_t configuration_index,
                                                   uint16_t descriptor_type,
                                                   uint16_t descriptor_index )
{
    uint8_t status = JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;

    switch ( descriptor_type )
    {
    case JDKSAVDECC_DESCRIPTOR_ENTITY:
        status = readDescriptorEntity( pdu, configuration_index, descriptor_index );
        break;
    case JDKSAVDECC_DESCRIPTOR_CONFIGURATION:
        status = readDescriptorConfiguration( pdu, configuration_index, descriptor_index );
        break;
    case JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE:
        status = readDescriptorAvbInterface( pdu, configuration_index, descriptor_index );
        break;
    case JDKSAVDECC_DESCRIPTOR_CONTROL:
        status = readDescriptorControl( pdu, configuration_index, descriptor_index );
        break;
    case JDKSAVDECC_DESCRIPTOR_LOCALE:
        status = readDescriptorLocale( pdu, configuration_index, descriptor_index );
        break;
    case JDKSAVDECC_DESCRIPTOR_STRINGS:
        status = readDescriptorStrings( pdu, configuration_index, descriptor_index );
        break;
    case JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT:
        status = readDescriptorMemoryObject( pdu, configuration_index, descriptor_index );
        break;
    }
    return status;
}

uint8_t EntityState::receiveSetConfigurationCommand( Frame &pdu )
{
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveGetConfigurationCommand( Frame &pdu )
{
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveSetNameCommand( Frame &pdu,
                                            uint16_t configuration_index,
                                            uint16_t descriptor_type,
                                            uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveGetNameCommand( Frame &pdu,
                                            uint16_t configuration_index,
                                            uint16_t descriptor_type,
                                            uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveSetControlCommand( Frame &pdu, uint16_t descriptor_index )
{
    (void)pdu;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveGetControlCommand( Frame &pdu, uint16_t descriptor_index )
{
    (void)pdu;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::readDescriptorEntity( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_index;

    return JDKSAVDECC_AEM_STATUS_NO_SUCH_DESCRIPTOR;
}

uint8_t EntityState::readDescriptorAvbInterface( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_index;

    return JDKSAVDECC_AEM_STATUS_NO_SUCH_DESCRIPTOR;
}

uint8_t EntityState::readDescriptorConfiguration( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_index;

    return JDKSAVDECC_AEM_STATUS_NO_SUCH_DESCRIPTOR;
}

uint8_t EntityState::readDescriptorControl( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_index;

    return JDKSAVDECC_AEM_STATUS_NO_SUCH_DESCRIPTOR;
}

uint8_t EntityState::readDescriptorLocale( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_index;

    return JDKSAVDECC_AEM_STATUS_NO_SUCH_DESCRIPTOR;
}

uint8_t EntityState::readDescriptorStrings( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_index;

    return JDKSAVDECC_AEM_STATUS_NO_SUCH_DESCRIPTOR;
}

uint8_t EntityState::readDescriptorMemoryObject( Frame &pdu, uint16_t configuration_index, uint16_t descriptor_index )
{
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_index;

    return JDKSAVDECC_AEM_STATUS_NO_SUCH_DESCRIPTOR;
}

uint8_t EntityState::receiveAARead( uint32_t virtual_base_address, uint16_t length, uint8_t *response )
{
    (void)virtual_base_address;
    (void)length;
    (void)response;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveAAWrite( uint32_t virtual_base_address, uint16_t length, const uint8_t *request )
{
    (void)virtual_base_address;
    (void)length;
    (void)request;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveAAExecute( uint32_t virtual_base_address, uint16_t length, const uint8_t *request )
{
    (void)virtual_base_address;
    (void)length;
    (void)request;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}
}
