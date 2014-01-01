#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"
#include "JDKSAvdeccHandler.h"
#include "JDKSAvdeccHelpers.h"
#include "JDKSAvdeccFrame.h"
#include "JDKSAvdeccADPManager.h"

namespace JDKSAvdecc {

#define JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS (4)


class Entity : public Handler {
public:
    Entity( ADPManager &adp_manager );

    /// Run periodic state machines
    virtual void Tick( uint32_t time_in_millis );

    /// Notification that a command to a target entity timed out
    virtual void CommandTimedOut( jdksavdecc_eui64 const &target_entity_id,
                                  uint16_t command_type,
                                  uint16_t sequence_id);
    /// Handle received AECPDU's
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );

    uint8_t ReceivedAEMCommand( jdksavdecc_aecpdu_aem const &aem, uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceivedAACommand( jdksavdecc_aecp_aa const &aa, uint8_t *buf, uint16_t pos, uint16_t len );

    /// Can we send a command now? i.e. are there no in-flight commands waiting to be acknowledged?
    inline bool CanSendCommand() const { return m_last_sent_command_type!=JDKSAVDECC_AEM_COMMAND_EXPANSION; }

    void SendCommand( jdksavdecc_eui64 const &target_entity_id,
                      jdksavdecc_eui48 const &target_mac_address,
                      uint16_t aem_command_type,
                      bool track_for_ack=true,
                      uint8_t const *additional_data1 = 0,
                      uint16_t additional_data_length1 = 0,
                      uint8_t const *additional_data2 = 0,
                      uint16_t additional_data_length2 = 0 );

    /// Send a direct response to the target entity id, and unsolicited
    /// responses to all other subscribed controllers
    void SendResponses( bool internally_generated,
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


    uint8_t ReceiveAcquireEntityCommand( uint8_t *buf, uint16_t pos, uint16_t len );


    uint8_t ReceiveLockEntityCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveEntityAvailableCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a CONTROLLER_AVAILABLE command to a target controller
    void SendControllerAvailable( jdksavdecc_eui64 const &target_controller_entity_id,
                                  jdksavdecc_eui48 const &target_mac_address ) {
        SendCommand(target_controller_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE);
    }

    uint8_t ReceiveControllerAvailableCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveControllerAvailableResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveReadDescriptorCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveSetConfigurationCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveGetConfigurationCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveSetNameCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveGetNameCommand( uint8_t *buf, uint16_t pos, uint16_t len );

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

    uint8_t ReceiveSetControlCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveGetControlCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveRegisterUnsolicitedNotificationCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    uint8_t ReceiveDeRegisterUnsolicitedNotificationCommand( uint8_t *buf, uint16_t pos, uint16_t len );


    virtual uint8_t ReadDescriptorEntity(  uint8_t *buf, uint16_t pos, uint16_t len );
    virtual uint8_t ReadDescriptorConfiguration( uint8_t *buf, uint16_t pos, uint16_t len );
    virtual uint8_t ReadDescriptorControl( uint8_t *buf, uint16_t pos, uint16_t len );
    virtual uint8_t ReadDescriptorLocale( uint8_t *buf, uint16_t pos, uint16_t len );
    virtual uint8_t ReadDescriptorString( uint8_t *buf, uint16_t pos, uint16_t len );
    virtual uint8_t ReadDescriptorMemory( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual uint8_t ReceiveAARead( uint32_t virtual_base_address, uint16_t length, uint8_t *response );
    virtual uint8_t ReceiveAAWrite( uint32_t virtual_base_address, uint16_t length, uint8_t const *request );
    virtual uint8_t ReceiveAAExecute( uint32_t virtual_base_address, uint16_t length, uint8_t const *request );

protected:
    ADPManager &m_adp_manager;
    uint32_t m_cur_time;
    uint16_t m_outgoing_sequence_id;

    jdksavdecc_eui64 m_acquired_by_controller_entity_id;
    jdksavdecc_eui64 m_acquire_in_progress_by_controller_entity_id;

    jdksavdecc_eui64 m_registered_controllers_entity_id[JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS];
    jdksavdecc_eui48 m_registered_controllers_mac_address[JDKSAVDECC_ENTITY_MAX_REGISTERED_CONTROLLERS];

    uint16_t m_last_sent_command_time;
    jdksavdecc_eui64 m_last_sent_command_target_entity_id;
    uint16_t m_last_sent_command_type;

};


}
