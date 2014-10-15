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

#include "JDKSAvdeccMCU_EntityState.hpp"
#include "JDKSAvdeccMCU_Entity.hpp"

namespace JDKSAvdeccMCU
{

EntityState::~EntityState() {}

void EntityState::tick() {}

bool EntityState::receivedPDU( Frame &frame )
{
    (void)frame;
    return false;
}

uint8_t EntityState::receiveLockEntityCommand( const jdksavdecc_aecpdu_aem &aem,
                                               Frame &pdu )
{
    (void)aem;
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t
    EntityState::receiveReadDescriptorCommand( const jdksavdecc_aecpdu_aem &aem,
                                               Frame &pdu,
                                               uint16_t configuration_index,
                                               uint16_t descriptor_type,
                                               uint16_t descriptor_index )
{
    uint8_t status = JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;

    switch ( descriptor_type )
    {
    case JDKSAVDECC_DESCRIPTOR_ENTITY:
        status = readDescriptorEntity(
            aem, pdu, configuration_index, descriptor_type, descriptor_index );
        break;
    }
    return status;
}

uint8_t EntityState::receiveSetConfigurationCommand(
    const jdksavdecc_aecpdu_aem &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveGetConfigurationCommand(
    const jdksavdecc_aecpdu_aem &aem, Frame &pdu )
{
    (void)aem;
    (void)pdu;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveSetNameCommand( const jdksavdecc_aecpdu_aem &aem,
                                            Frame &pdu,
                                            uint16_t configuration_index,
                                            uint16_t descriptor_type,
                                            uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveGetNameCommand( const jdksavdecc_aecpdu_aem &aem,
                                            Frame &pdu,
                                            uint16_t configuration_index,
                                            uint16_t descriptor_type,
                                            uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveSetControlCommand( const jdksavdecc_aecpdu_aem &aem,
                                               Frame &pdu,
                                               uint16_t descriptor_type,
                                               uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveGetControlCommand( const jdksavdecc_aecpdu_aem &aem,
                                               Frame &pdu,
                                               uint16_t descriptor_type,
                                               uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::readDescriptorEntity( const jdksavdecc_aecpdu_aem &aem,
                                           Frame &pdu,
                                           uint16_t configuration_index,
                                           uint16_t descriptor_type,
                                           uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t
    EntityState::readDescriptorConfiguration( const jdksavdecc_aecpdu_aem &aem,
                                              Frame &pdu,
                                              uint16_t configuration_index,
                                              uint16_t descriptor_type,
                                              uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::readDescriptorControl( const jdksavdecc_aecpdu_aem &aem,
                                            Frame &pdu,
                                            uint16_t configuration_index,
                                            uint16_t descriptor_type,
                                            uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::readDescriptorLocale( const jdksavdecc_aecpdu_aem &aem,
                                           Frame &pdu,
                                           uint16_t configuration_index,
                                           uint16_t descriptor_type,
                                           uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::readDescriptorStrings( const jdksavdecc_aecpdu_aem &aem,
                                            Frame &pdu,
                                            uint16_t configuration_index,
                                            uint16_t descriptor_type,
                                            uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::readDescriptorMemory( const jdksavdecc_aecpdu_aem &aem,
                                           Frame &pdu,
                                           uint16_t configuration_index,
                                           uint16_t descriptor_type,
                                           uint16_t descriptor_index )
{
    (void)aem;
    (void)pdu;
    (void)configuration_index;
    (void)descriptor_type;
    (void)descriptor_index;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveAARead( const jdksavdecc_aecp_aa &aa,
                                    uint32_t virtual_base_address,
                                    uint16_t length,
                                    uint8_t *response )
{
    (void)aa;
    (void)virtual_base_address;
    (void)length;
    (void)response;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveAAWrite( const jdksavdecc_aecp_aa &aa,
                                     uint32_t virtual_base_address,
                                     uint16_t length,
                                     const uint8_t *request )
{
    (void)aa;
    (void)virtual_base_address;
    (void)length;
    (void)request;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}

uint8_t EntityState::receiveAAExecute( const jdksavdecc_aecp_aa &aa,
                                       uint32_t virtual_base_address,
                                       uint16_t length,
                                       const uint8_t *request )
{
    (void)aa;
    (void)virtual_base_address;
    (void)length;
    (void)request;

    return JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED;
}
}
