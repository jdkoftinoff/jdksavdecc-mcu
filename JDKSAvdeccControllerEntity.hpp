#pragma once
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


#include "JDKSAvdeccWorld.hpp"
#include "JDKSAvdeccNetIO.hpp"
#include "JDKSAvdeccHandler.hpp"
#include "JDKSAvdeccHelpers.hpp"
#include "JDKSAvdeccFrame.hpp"
#include "JDKSAvdeccADPManager.hpp"
#include "JDKSAvdeccEntity.hpp"

namespace JDKSAvdecc {


class ControllerEntity : public Entity {
public:
    ControllerEntity( ADPManager &adp_manager )
        : Entity(adp_manager) {}

    /// Handle incoming commands and responses
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );


    virtual bool ReceivedAAResponse(
            jdksavdecc_aecp_aa const &aa,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    bool ReceivedAEMResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );


    // Formulate and send a ACQUIRE_ENTITY command to a target entity
    void SendAcquireEntity(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint32_t flags ) {
        uint8_t additional1[16];
        jdksavdecc_uint32_set(flags,additional1,0); // offset 12 in Figure 7.34, and Table 7.127 for flags
        jdksavdecc_eui64_set(m_adp_manager.GetEntityID(),additional1,4); // offset 16 in Figure 7.34
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_ENTITY,additional1,12); // offset 24 in Figure 7.34
        jdksavdecc_uint16_set(0,additional1,14); // offset 26 in Figure 7.34
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY,true,additional1,sizeof(additional1));
    }

    virtual bool ReceiveAcquireEntityResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Formulate and send a LOCK_ENTITY command to a target entity
    void SendLockEntity(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint32_t flags
            ) {
        uint8_t additional1[16];
        jdksavdecc_uint32_set(flags,additional1,0); // offset 12 in Figure 7.35, and Table 7.128 for flags
        jdksavdecc_eui64_set(m_adp_manager.GetEntityID(),additional1,4); // offset 16 in Figure 7.35
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_ENTITY,additional1,12); // offset 24 in Figure 7.35
        jdksavdecc_uint16_set(0,additional1,14); // offset 26 in Figure 7.35
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY,true,additional1,sizeof(additional1));
    }

    virtual bool ReceiveLockEntityResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Formulate and send an ENTITY_AVAILABLE command to a target entity
    void SendEntityAvailable(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address) {
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE);
    }

    virtual bool ReceiveEntityAvailableResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Formulate and send a READ_DESCRIPTOR command to a target entity
    void SendReadDescriptor(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint16_t configuration_index,
            uint16_t descriptor_type,
            uint16_t descriptor_index
            ) {
        uint8_t additional1[8];
        jdksavdecc_uint16_set(configuration_index,additional1,0); // offset 12 in Figure 7.36
        jdksavdecc_uint16_set(0,additional1,2); // offset 14 in Figure 7.36
        jdksavdecc_uint16_set(descriptor_type,additional1,4); // offset 16 in Figure 7.36
        jdksavdecc_uint16_set(descriptor_index,additional1,6); // offset 20 in Figure 7.35
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR,true,additional1,sizeof(additional1));
    }


    virtual bool ReceiveReadDescriptorResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a SET_CONFIGURATION command to a target entity
    void SendSetConfiguration(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint16_t configuration_index ) {
        uint8_t additional1[4];
        jdksavdecc_uint16_set(0,additional1,0);
        jdksavdecc_uint16_set(configuration_index,additional1,2);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION,
                    true,
                    additional1,sizeof(additional1));
    }

    virtual bool ReceiveSetConfigurationResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );


    // Formulate and send a GET_CONFIGURATION command to a target entity
    void SendGetConfiguration(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address ) {
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION);
    }

    virtual bool ReceiveGetConfigurationResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );


    // Formulate and send a SET_NAME command to a target entity
    void SendSetName(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint16_t target_descriptor_type,
            uint16_t target_descriptor_index,
            uint16_t name_index,
            uint16_t configuration_index,
            jdksavdecc_string const &name )  {
        uint8_t additional1[8];
        jdksavdecc_uint16_set(target_descriptor_type,additional1,0);
        jdksavdecc_uint16_set(target_descriptor_index,additional1,2);
        jdksavdecc_uint16_set(name_index,additional1,4);
        jdksavdecc_uint16_set(configuration_index,additional1,6);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_SET_NAME,
                    true,
                    additional1,sizeof(additional1),
                    name.value, sizeof(name.value) );
    }

    virtual bool ReceiveSetNameResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );


    // Formulate and send a GET_NAME command to a target entity
    void SendGetName(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint16_t target_descriptor_type,
            uint16_t target_descriptor_index,
            uint16_t name_index,
            uint16_t configuration_index )  {
        uint8_t additional1[8];
        jdksavdecc_uint16_set(target_descriptor_type,additional1,0);
        jdksavdecc_uint16_set(target_descriptor_index,additional1,2);
        jdksavdecc_uint16_set(name_index,additional1,4);
        jdksavdecc_uint16_set(configuration_index,additional1,6);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_GET_NAME );
    }

    virtual bool ReceiveGetNameResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Formulate and send a SET_CONTROL command to a target entity
    void SendSetControl(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint16_t target_descriptor_index,
            uint8_t *control_value,
            uint16_t control_value_len,
            bool track_for_ack )  {
        uint8_t additional1[4];
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_CONTROL,additional1,0);
        jdksavdecc_uint16_set(target_descriptor_index,additional1,2);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_SET_CONTROL,
                    track_for_ack,
                    additional1,sizeof(additional1),
                    control_value, control_value_len );
    }

    virtual bool ReceiveSetControlResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );


    // Formulate and send an REGISTER_UNSOLICITED_NOTIFICATION command to a target entity
    void SendRegisterUnsolicitedNotification(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address) {
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION,0,0);
    }

    virtual bool ReceiveRegisterUnsolicitedNotificationResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Formulate and send a DEREGISTER_UNSOLICITED_NOTIFICATION command to a target entity
    void SendDeRegisterUnsolicitedNotification(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address) {
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION,0,0);
    }

    virtual bool ReceiveDeRegisterUnsolicitedNotificationResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Notification that a new control value was received from the target entity,
    // either because of a solicited or unsolicited SET_CONTROL response or
    // a GET_CONTROL response
    virtual uint8_t ReceiveControlValue(
            jdksavdecc_eui64 const &target_entity_id,
            uint16_t target_descriptor_index,
            uint8_t const *control_value,
            uint16_t control_value_len
            );

    // Formulate and send a GET_CONTROL command to a target entity
    void SendGetControl(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint16_t target_descriptor_index ) {
        uint8_t additional1[4];
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_CONTROL,additional1,0);
        jdksavdecc_uint16_set(target_descriptor_index,additional1,2);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_GET_CONTROL,
                    true,
                    additional1,sizeof(additional1));
    }
    virtual bool ReceiveGetControlResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

protected:

};

}
