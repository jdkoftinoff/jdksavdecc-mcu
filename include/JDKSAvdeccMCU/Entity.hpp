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

namespace JDKSAvdeccMCU
{
class EntityState;

#ifndef JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS
#define JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS ( 4 )
#endif

class Entity : public Handler
{
  public:
    Entity( ADPManager &adp_manager, EntityState *entity_state );

    /// Run periodic state machines (from Handler)
    virtual void
        tick( jdksavdecc_timestamp_in_milliseconds time_in_millis ) override;

    /// Handle received AECPDU's (from Handler)
    virtual bool receivedPDU( RawSocket *incoming_socket,
                              Frame &frame ) override;

    /// Notification that a command to a target entity timed out
    virtual void commandTimedOut( Eui64 const &target_entity_id,
                                  uint16_t command_type,
                                  uint16_t sequence_id );

    /// Get the ADP Manager
    ADPManager &getADPManager() { return m_adp_manager; }

    /// Get the RawSocket from the ADP Manager
    RawSocket &getRawSocket() { return m_adp_manager.getRawSocket(); }

    /// Get the Entity ID
    Eui64 const &getEntityID() const { return m_adp_manager.getEntityID(); }

    /// Check to make sure the command is allowed or disallowed due to acquire
    /// or locking
    uint8_t validatePermissions( jdksavdecc_aecpdu_aem const &aem );

    /// The received pdu contains a valid AEM command for me.
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    uint8_t receivedAEMCommand( jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    /// The received pdu contains a valid AA command for me.
    /// Fill in the response in place in the pdu and return an AECP AA status
    /// code
    uint8_t receivedAACommand( jdksavdecc_aecp_aa const &aa, Frame &pdu );

    virtual uint8_t receivedACMPMessage( jdksavdecc_acmpdu const &acmpdu,
                                         Frame &pdu );

    /// Can we send a command now? i.e. are there no in-flight commands waiting
    /// to be acknowledged?
    bool canSendCommand() const
    {
        // last sent command type is set to JDKSAVDECC_AEM_COMMAND_EXPANSION
        // when there is no command in flight
        return m_last_sent_command_type != JDKSAVDECC_AEM_COMMAND_EXPANSION;
    }

    void sendCommand( Eui64 const &target_entity_id,
                      Eui48 const &target_mac_address,
                      uint16_t aem_command_type,
                      bool track_for_ack = true,
                      uint8_t const *additional_data1 = 0,
                      uint16_t additional_data_length1 = 0,
                      uint8_t const *additional_data2 = 0,
                      uint16_t additional_data_length2 = 0 );

    /// Send a direct response to the target entity id, and unsolicited
    /// responses to all other subscribed controllers
    void sendResponses( bool internally_generated,
                        bool send_to_registered_controllers,
                        uint8_t aecp_status_code,
                        Frame &pdu,
                        uint8_t const *additional_data1 = 0,
                        uint16_t additional_data_length1 = 0,
                        uint8_t const *additional_data2 = 0,
                        uint16_t additional_data_length2 = 0 );

    /// Formulate an AEM Unsolicited response of specified command_type with the
    /// specified additional data as payload, and send it to all interested
    /// controllers
    void sendUnsolicitedResponses( uint16_t aem_command_type,
                                   uint8_t const *additional_data1 = 0,
                                   uint16_t additional_data_length1 = 0,
                                   uint8_t const *additional_data2 = 0,
                                   uint16_t additional_data_length2 = 0 );

    /// The pdu contains a valid Acquire Entity command.
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    uint8_t receiveAcquireEntityCommand( jdksavdecc_aecpdu_aem const &aem,
                                         Frame &pdu );

    /// The pdu contains a valid Lock Entity command.
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t receiveLockEntityCommand( jdksavdecc_aecpdu_aem const &aem,
                                              Frame &pdu );

    /// The pdu contains a valid Entity Available command.
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    uint8_t receiveEntityAvailableCommand( jdksavdecc_aecpdu_aem const &aem,
                                           Frame &pdu );

    // Formulate and send a CONTROLLER_AVAILABLE command to a target controller
    void sendControllerAvailable( Eui64 const &target_controller_entity_id,
                                  Eui48 const &target_mac_address )
    {
        sendCommand( target_controller_entity_id,
                     target_mac_address,
                     JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE );
    }

    /// The pdu contains a valid Controller Available command.
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    virtual uint8_t
        receiveControllerAvailableCommand( jdksavdecc_aecpdu_aem const &aem,
                                           Frame &pdu );

    /// The pdu contains a valid Controller Available response.
    /// return true if the response is handled
    virtual bool
        receiveControllerAvailableResponse( jdksavdecc_aecpdu_aem const &aem,
                                            Frame &pdu );

    // Formulate and send a SET_CONTROL unsolicited response to all subscribed
    // controllers
    void sendSetControlUnsolicitedResponse( uint16_t target_descriptor_index,
                                            uint8_t *control_value,
                                            uint16_t control_value_len )
    {
        uint8_t additional1[4];
        jdksavdecc_uint16_set( JDKSAVDECC_DESCRIPTOR_CONTROL, additional1, 0 );
        jdksavdecc_uint16_set( target_descriptor_index, additional1, 2 );
        sendUnsolicitedResponses( JDKSAVDECC_AEM_COMMAND_SET_CONTROL,
                                  additional1,
                                  sizeof( additional1 ),
                                  control_value,
                                  control_value_len );
    }

    /// The pdu contains a valid Register for Unsolicited Notifications Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    uint8_t receiveRegisterUnsolicitedNotificationCommand(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

    /// The pdu contains a valid De-Register for Unsolicited Notifications
    /// Command
    /// Fill in the response in place in the pdu and return an AECP AEM status
    /// code
    uint8_t receiveDeRegisterUnsolicitedNotificationCommand(
        jdksavdecc_aecpdu_aem const &aem, Frame &pdu );

  protected:
    /// The advertising manager, also contains capabilities, entity_id, and
    /// entity_model_id
    ADPManager &m_adp_manager;

    /// The sequence_id of the last send outgoing command
    uint16_t m_outgoing_sequence_id;

    /// If we are acquired by a controller, then this is set to the controller's
    /// entity id.
    /// If we are not acquired, then this will be FF:FF:FF:FF:FF:FF:FF:FF
    Eui64 m_acquired_by_controller_entity_id;

    /// If we are acquired by a controller, then this is the MAC address of the
    /// controller
    Eui48 m_acquired_by_controller_mac_address;

    /// If we are acquired by one controller and another controller is trying to
    /// acquire this entity, then this contains the new controller's entity_id
    /// during the controller available negotiation mechanism
    Eui64 m_acquire_in_progress_by_controller_entity_id;

    /// If we are currently interrogating a controller with a controller
    /// available, then this is the timestamp of when we started the controller
    /// available negotiation mechanism
    jdksavdecc_timestamp_in_milliseconds m_acquire_in_progress_time;

    /// If we are locked by a controller, then this contains the controller's
    /// entity id which locked us
    Eui64 m_locked_by_controller_entity_id;

    /// If we are locked by a controller, then this contains the timestamp of
    /// when the lock first ocurred
    jdksavdecc_timestamp_in_milliseconds m_locked_time;

    /// This contains the unordered list of controller entity id's that are
    /// currently registered via the register for unsolicited notifications
    /// command. The entity id is FF:FF:FF:FF:FF:FF:FF:FF If the the slot is not
    /// in use
    Eui64 m_registered_controllers_entity_id
        [JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS];

    /// This contains the associated MAC address of the controller's that are
    /// currently registered
    Eui48 m_registered_controllers_mac_address
        [JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS];

    /// This is the timestamp of the last command that we sent to another entity
    jdksavdecc_timestamp_in_milliseconds m_last_sent_command_time;

    /// This is the entity id that was the target of the last command that we
    /// sent
    Eui64 m_last_sent_command_target_entity_id;

    /// This is the command_type of the last command that we sent to another
    /// entity
    uint16_t m_last_sent_command_type;

    /// The entity state object, if any
    EntityState *m_entity_state;
};
}
