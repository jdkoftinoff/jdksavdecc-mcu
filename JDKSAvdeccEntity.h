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


#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"
#include "JDKSAvdeccHandler.h"
#include "JDKSAvdeccHelpers.h"
#include "JDKSAvdeccFrame.h"
#include "JDKSAvdeccADPManager.h"

namespace JDKSAvdecc {

#ifndef JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS
#define JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS (4)
#endif


class Entity : public Handler {
public:
    Entity( ADPManager &adp_manager );

    /// Run periodic state machines
    virtual void Tick( uint32_t time_in_millis );

    /// Notification that a command to a target entity timed out
    virtual void CommandTimedOut(
            jdksavdecc_eui64 const &target_entity_id,
            uint16_t command_type,
            uint16_t sequence_id);

    /// Handle received AECPDU's
    virtual bool ReceivedPDU(
            uint32_t time_in_millis,
            uint8_t *buf,
            uint16_t len );

    /// Check to make sure the command is allowed or disallowed due to acquire or locking
    uint8_t ValidatePermissions( jdksavdecc_aecpdu_aem const &aem );

    uint8_t ReceivedAEMCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceivedAACommand(
            jdksavdecc_aecp_aa const &aa,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    /// Can we send a command now? i.e. are there no in-flight commands waiting to be acknowledged?
    inline bool CanSendCommand() const {
        return m_last_sent_command_type!=JDKSAVDECC_AEM_COMMAND_EXPANSION;
    }

    void SendCommand(
            jdksavdecc_eui64 const &target_entity_id,
            jdksavdecc_eui48 const &target_mac_address,
            uint16_t aem_command_type,
            bool track_for_ack=true,
            uint8_t const *additional_data1 = 0,
            uint16_t additional_data_length1 = 0,
            uint8_t const *additional_data2 = 0,
            uint16_t additional_data_length2 = 0 );

    /// Send a direct response to the target entity id, and unsolicited
    /// responses to all other subscribed controllers
    void SendResponses(
            bool internally_generated,
            bool send_to_registered_controllers,
            uint8_t *buf,
            uint16_t len,
            uint8_t const *additional_data1 = 0,
            uint16_t additional_data_length1 = 0,
            uint8_t const *additional_data2 = 0,
            uint16_t additional_data_length2 = 0 );


    void SendUnsolicitedResponses(
            uint16_t aem_command_type,
            uint8_t const *additional_data1 = 0,
            uint16_t additional_data_length1 = 0,
            uint8_t const *additional_data2 = 0,
            uint16_t additional_data_length2 = 0 );


    uint8_t ReceiveAcquireEntityCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );


    uint8_t ReceiveLockEntityCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveEntityAvailableCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Formulate and send a CONTROLLER_AVAILABLE command to a target controller
    void SendControllerAvailable(
            jdksavdecc_eui64 const &target_controller_entity_id,
            jdksavdecc_eui48 const &target_mac_address ) {
        SendCommand(target_controller_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE);
    }

    uint8_t ReceiveControllerAvailableCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    virtual bool ReceiveControllerAvailableResponse(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveReadDescriptorCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveSetConfigurationCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveGetConfigurationCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveSetNameCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveGetNameCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    // Formulate and send a SET_CONTROL unsolicited response to all subscribed controllers
    void SendSetControlUnsolicitedResponse(
            uint16_t target_descriptor_index,
            uint8_t *control_value,
            uint16_t control_value_len)  {
        uint8_t additional1[4];
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_CONTROL,additional1,0);
        jdksavdecc_uint16_set(target_descriptor_index,additional1,2);
        SendUnsolicitedResponses(
                    JDKSAVDECC_AEM_COMMAND_SET_CONTROL,
                    additional1,sizeof(additional1),
                    control_value, control_value_len );
    }

    uint8_t ReceiveSetControlCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveGetControlCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveRegisterUnsolicitedNotificationCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    uint8_t ReceiveDeRegisterUnsolicitedNotificationCommand(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );


    virtual uint8_t ReadDescriptorEntity(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    virtual uint8_t ReadDescriptorConfiguration(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    virtual uint8_t ReadDescriptorControl(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    virtual uint8_t ReadDescriptorLocale(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    virtual uint8_t ReadDescriptorString(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    virtual uint8_t ReadDescriptorMemory(
            jdksavdecc_aecpdu_aem const &aem,
            uint8_t *buf,
            uint16_t pos,
            uint16_t len );

    virtual uint8_t ReceiveAARead(
            jdksavdecc_aecp_aa const &aa,
            uint32_t virtual_base_address,
            uint16_t length,
            uint8_t *response );

    virtual uint8_t ReceiveAAWrite(
            jdksavdecc_aecp_aa const &aa,
            uint32_t virtual_base_address,
            uint16_t length,
            uint8_t const *request );

    virtual uint8_t ReceiveAAExecute(
            jdksavdecc_aecp_aa const &aa,
            uint32_t virtual_base_address,
            uint16_t length,
            uint8_t const *request );

protected:
    ADPManager &m_adp_manager;
    uint32_t m_cur_time;
    uint16_t m_outgoing_sequence_id;

    jdksavdecc_eui64 m_acquired_by_controller_entity_id;
    jdksavdecc_eui48 m_acquired_by_controller_mac_address;
    jdksavdecc_eui64 m_acquire_in_progress_by_controller_entity_id;
    uint32_t m_acquire_in_progress_time;

    jdksavdecc_eui64 m_locked_by_controller_entity_id;
    uint32_t m_locked_time;

    jdksavdecc_eui64 m_registered_controllers_entity_id[JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS];
    jdksavdecc_eui48 m_registered_controllers_mac_address[JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS];

    uint16_t m_last_sent_command_time;
    jdksavdecc_eui64 m_last_sent_command_target_entity_id;
    uint16_t m_last_sent_command_type;

};


}
