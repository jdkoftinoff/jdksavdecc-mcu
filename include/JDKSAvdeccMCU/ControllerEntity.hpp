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
#include "JDKSAvdeccMCU/ADPManager.hpp"
#include "JDKSAvdeccMCU/Entity.hpp"
#include "JDKSAvdeccMCU/EntityState.hpp"

namespace JDKSAvdeccMCU
{

class ControllerEntity : public Entity
{
  public:
    ControllerEntity( ADPManager &adp_manager, RegisteredControllers *registered_controllers, EntityState *entity_state )
        : Entity( adp_manager, registered_controllers, entity_state )
    {
    }

    /// Handle incoming commands and responses
    virtual bool receivedPDU( Frame &frame ) override;

    virtual bool receivedAAResponse( jdksavdecc_aecp_aa const &aa, Frame &pdu );

    bool receivedAEMResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a ACQUIRE_ENTITY command to a target entity
    void sendAcquireEntity( Eui64 const &target_entity_id, Eui48 const &target_mac_address, uint32_t flags );

    virtual bool receiveAcquireEntityResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a LOCK_ENTITY command to a target entity
    void sendLockEntity( Eui64 const &target_entity_id, Eui48 const &target_mac_address, uint32_t flags );

    virtual bool receiveLockEntityResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send an ENTITY_AVAILABLE command to a target entity
    void sendEntityAvailable( Eui64 const &target_entity_id, Eui48 const &target_mac_address );

    virtual bool receiveEntityAvailableResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a READ_DESCRIPTOR command to a target entity
    void sendReadDescriptor( Eui64 const &target_entity_id,
                             Eui48 const &target_mac_address,
                             uint16_t configuration_index,
                             uint16_t descriptor_type,
                             uint16_t descriptor_index );

    virtual bool receiveReadDescriptorResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a SET_CONFIGURATION command to a target entity
    void sendSetConfiguration( Eui64 const &target_entity_id, Eui48 const &target_mac_address, uint16_t configuration_index );

    virtual bool receiveSetConfigurationResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a GET_CONFIGURATION command to a target entity
    void sendGetConfiguration( Eui64 const &target_entity_id, Eui48 const &target_mac_address );

    virtual bool receiveGetConfigurationResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a SET_NAME command to a target entity
    void sendSetName( Eui64 const &target_entity_id,
                      Eui48 const &target_mac_address,
                      uint16_t target_descriptor_type,
                      uint16_t target_descriptor_index,
                      uint16_t name_index,
                      uint16_t configuration_index,
                      jdksavdecc_string const &name );

    virtual bool receiveSetNameResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a GET_NAME command to a target entity
    void sendGetName( Eui64 const &target_entity_id,
                      Eui48 const &target_mac_address,
                      uint16_t target_descriptor_type,
                      uint16_t target_descriptor_index,
                      uint16_t name_index,
                      uint16_t configuration_index );

    virtual bool receiveGetNameResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a SET_CONTROL command to a target entity
    void sendSetControl( Eui64 const &target_entity_id,
                         Eui48 const &target_mac_address,
                         uint16_t target_descriptor_index,
                         uint8_t *control_value,
                         uint16_t control_value_len,
                         bool track_for_ack );

    virtual bool receiveSetControlResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send an REGISTER_UNSOLICITED_NOTIFICATION command to a
    // target entity
    void sendRegisterUnsolicitedNotification( Eui64 const &target_entity_id, Eui48 const &target_mac_address );

    virtual bool receiveRegisterUnsolicitedNotificationResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Formulate and send a DEREGISTER_UNSOLICITED_NOTIFICATION command to a
    // target entity
    void sendDeRegisterUnsolicitedNotification( Eui64 const &target_entity_id, Eui48 const &target_mac_address );

    virtual bool receiveDeRegisterUnsolicitedNotificationResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    // Notification that a new control value was received from the target
    // entity, either because of a solicited or unsolicited SET_CONTROL response
    // or a GET_CONTROL response
    virtual uint8_t receiveControlValue( Eui64 const &target_entity_id,
                                         uint16_t target_descriptor_index,
                                         uint8_t const *control_value,
                                         uint16_t control_value_len );

    // Formulate and send a GET_CONTROL command to a target entity
    void sendGetControl( Eui64 const &target_entity_id, Eui48 const &target_mac_address, uint16_t target_descriptor_index );

    virtual bool receiveGetControlResponse( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

  protected:
};
}
