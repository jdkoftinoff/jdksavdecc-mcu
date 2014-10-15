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
#include "JDKSAvdeccMCU_Handler.hpp"
#include "JDKSAvdeccMCU_Helpers.hpp"
#include "JDKSAvdeccMCU_Frame.hpp"

namespace JDKSAvdeccMCU
{
class Entity;

class EntityState : public Handler
{
  public:
    EntityState() {}
    virtual ~EntityState();

    /// Run periodic state machines
    virtual void tick();

    virtual bool receivedPDU( Frame &frame );

    /// The pdu contains a valid Lock Entity command.
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveLockEntityCommand( jdksavdecc_aecpdu_aem const &aem,
                                              Frame &pdu );

    /// The pdu contains a valid Read Descriptor Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        receiveReadDescriptorCommand( jdksavdecc_aecpdu_aem const &aem,
                                      Frame &pdu,
                                      uint16_t configuration_index,
                                      uint16_t descriptor_type,
                                      uint16_t descriptor_index );

    /// The pdu contains a valid Set Configuration Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        receiveSetConfigurationCommand( jdksavdecc_aecpdu_aem const &aem,
                                        Frame &pdu );

    /// The pdu contains a valid Get Configuration Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        receiveGetConfigurationCommand( jdksavdecc_aecpdu_aem const &aem,
                                        Frame &pdu );

    /// The pdu contains a valid Set Name Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveSetNameCommand( jdksavdecc_aecpdu_aem const &aem,
                                           Frame &pdu,
                                           uint16_t configuration_index,
                                           uint16_t descriptor_type,
                                           uint16_t descriptor_index );

    /// The pdu contains a valid Get Name Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveGetNameCommand( jdksavdecc_aecpdu_aem const &aem,
                                           Frame &pdu,
                                           uint16_t configuration_index,
                                           uint16_t descriptor_type,
                                           uint16_t descriptor_index );

    /// The pdu contains a valid Set Control Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveSetControlCommand( jdksavdecc_aecpdu_aem const &aem,
                                              Frame &pdu,
                                              uint16_t descriptor_index );

    /// The pdu contains a valid Get Control Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveGetControlCommand( jdksavdecc_aecpdu_aem const &aem,
                                              Frame &pdu,
                                              uint16_t descriptor_index );

    /// The pdu contains a valid Read Entity Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorEntity( jdksavdecc_aecpdu_aem const &aem,
                                          Frame &pdu,
                                          uint16_t configuration_index,
                                          uint16_t descriptor_index );

    /// The pdu contains a valid Read AVB_INTERFACE descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        readDescriptorAvbInterface( const jdksavdecc_aecpdu_aem &aem,
                                    Frame &pdu,
                                    uint16_t configuration_index,
                                    uint16_t descriptor_index );

    /// The pdu contains a valid Read Configuration Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        readDescriptorConfiguration( jdksavdecc_aecpdu_aem const &aem,
                                     Frame &pdu,
                                     uint16_t configuration_index,
                                     uint16_t descriptor_index );

    /// The pdu contains a valid Read Control Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorControl( jdksavdecc_aecpdu_aem const &aem,
                                           Frame &pdu,
                                           uint16_t configuration_index,
                                           uint16_t descriptor_index );

    /// The pdu contains a valid Read Locale Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorLocale( jdksavdecc_aecpdu_aem const &aem,
                                          Frame &pdu,
                                          uint16_t configuration_index,
                                          uint16_t descriptor_index );

    /// The pdu contains a valid Read Strings Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t readDescriptorStrings( jdksavdecc_aecpdu_aem const &aem,
                                           Frame &pdu,
                                           uint16_t configuration_index,
                                           uint16_t descriptor_index );

    /// The pdu contains a valid Read Memory Object Descriptor command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        readDescriptorMemoryObject( jdksavdecc_aecpdu_aem const &aem,
                                    Frame &pdu,
                                    uint16_t configuration_index,
                                    uint16_t descriptor_index );

    /// The pdu contains a valid Read Address Access TLV command
    /// Fill in the response in place in the pdu and return an AECP AA status
    /// code
    virtual uint8_t receiveAARead( jdksavdecc_aecp_aa const &aa,
                                   uint32_t virtual_base_address,
                                   uint16_t length,
                                   uint8_t *response );

    /// The pdu contains a valid Write Address Access TLV command
    /// Fill in the response in place in the pdu and return an AECP AA status
    /// code
    virtual uint8_t receiveAAWrite( jdksavdecc_aecp_aa const &aa,
                                    uint32_t virtual_base_address,
                                    uint16_t length,
                                    uint8_t const *request );

    /// The pdu contains a valid Execute Address Access TLV command
    /// Fill in the response in place in the pdu and return an AECP AA status
    /// code
    virtual uint8_t receiveAAExecute( jdksavdecc_aecp_aa const &aa,
                                      uint32_t virtual_base_address,
                                      uint16_t length,
                                      uint8_t const *request );
};
}
