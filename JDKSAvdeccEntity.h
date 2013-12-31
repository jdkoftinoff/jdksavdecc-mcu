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


    // Run periodic state machines
    virtual void Tick( uint32_t time_in_millis );

    // Handle received AECPDU's
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );

    uint8_t ReceivedAEMCommand( jdksavdecc_aecpdu_aem const &aem, uint8_t *buf, uint16_t pos, uint16_t len );
    bool ReceivedAEMResponse( jdksavdecc_aecpdu_aem const &aem, uint8_t *buf, uint16_t pos, uint16_t len );
    uint8_t ReceivedAACommand( jdksavdecc_aecp_aa const &aa, uint8_t *buf, uint16_t pos, uint16_t len );
    virtual bool ReceivedAAResponse( jdksavdecc_aecp_aa const &aa, uint8_t *buf, uint16_t pos, uint16_t len );

    // Send a direct response to the target entity id, and unsolicited
    // responses to all other subscribed controllers
    void SendResponses( bool internally_generated,
                        uint8_t *buf,
                        uint16_t len );

    void SendCommand( jdksavdecc_eui64 const &target_entity_id,
                      jdksavdecc_eui48 const &target_mac_address,
                      uint16_t aem_command_type,
                      uint8_t const *additional_data1 = 0,
                      uint16_t additional_data_length1 = 0,
                      uint8_t const *additional_data2 = 0,
                      uint16_t additional_data_length2 = 0 );

    // Formulate and send a ACQUIRE_ENTITY command to a target entity
    void SendAcquireEntity( jdksavdecc_eui64 const &target_entity_id,
                            jdksavdecc_eui48 const &target_mac_address,
                            uint32_t flags ) {
        uint8_t additional1[16];
        jdksavdecc_uint32_set(flags,additional1,0); // offset 12 in Figure 7.34, and Table 7.127 for flags
        jdksavdecc_eui64_set(m_adp_manager.GetEntityID(),additional1,4); // offset 16 in Figure 7.34
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_ENTITY,additional1,12); // offset 24 in Figure 7.34
        jdksavdecc_uint16_set(0,additional1,14); // offset 26 in Figure 7.34
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY,additional1,sizeof(additional1));
    }

    uint8_t ReceiveAcquireEntityCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveAcquireEntityResponse( uint8_t *buf, uint16_t pos, uint16_t len );


    // Formulate and send a LOCK_ENTITY command to a target entity
    void SendLockEntity( jdksavdecc_eui64 const &target_entity_id,
                         jdksavdecc_eui48 const &target_mac_address,
                         uint32_t flags
                         ) {
        uint8_t additional1[16];
        jdksavdecc_uint32_set(flags,additional1,0); // offset 12 in Figure 7.35, and Table 7.128 for flags
        jdksavdecc_eui64_set(m_adp_manager.GetEntityID(),additional1,4); // offset 16 in Figure 7.35
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_ENTITY,additional1,12); // offset 24 in Figure 7.35
        jdksavdecc_uint16_set(0,additional1,14); // offset 26 in Figure 7.35
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY,additional1,sizeof(additional1));
    }

    uint8_t ReceiveLockEntityCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveLockEntityResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send an ENTITY_AVAILABLE command to a target entity
    void SendEntityAvailable( jdksavdecc_eui64 const &target_entity_id,
                              jdksavdecc_eui48 const &target_mac_address) {
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE);
    }

    uint8_t ReceiveEntityAvailableCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveEntityAvailableResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a CONTROLLER_AVAILABLE command to a target controller
    void SendControllerAvailable( jdksavdecc_eui64 const &target_controller_entity_id,
                                  jdksavdecc_eui48 const &target_mac_address ) {
        SendCommand(target_controller_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE);
    }

    uint8_t ReceiveControllerAvailableCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveControllerAvailableResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a READ_DESCRIPTOR command to a target entity
    void SendReadDescriptor( jdksavdecc_eui64 const &target_entity_id,
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
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR,additional1,sizeof(additional1));
    }

    uint8_t ReceiveReadDescriptorCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveReadDescriptorResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a SET_CONFIGURATION command to a target entity
    void SendSetConfiguration( jdksavdecc_eui64 const &target_entity_id,
                               jdksavdecc_eui48 const &target_mac_address,
                               uint16_t configuration_index ) {
        uint8_t additional1[4];
        jdksavdecc_uint16_set(0,additional1,0);
        jdksavdecc_uint16_set(configuration_index,additional1,2);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION,
                    additional1,sizeof(additional1));
    }

    uint8_t ReceiveSetConfigurationCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveSetConfigurationResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a GET_CONFIGURATION command to a target entity
    void SendGetConfiguration( jdksavdecc_eui64 const &target_entity_id,
                               jdksavdecc_eui48 const &target_mac_address ) {
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION);
    }

    uint8_t ReceiveGetConfigurationCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveGetConfigurationResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a SET_NAME command to a target entity
    void SendSetName( jdksavdecc_eui64 const &target_entity_id,
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
                    additional1,sizeof(additional1),
                    name.value, sizeof(name.value) );
    }

    uint8_t ReceiveSetNameCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveSetNameResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a GET_NAME command to a target entity
    void SendGetName( jdksavdecc_eui64 const &target_entity_id,
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

    uint8_t ReceiveGetNameCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveGetNameResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a SET_CONTROL command to a target entity
    void SendSetControl( jdksavdecc_eui64 const &target_entity_id,
                         jdksavdecc_eui48 const &target_mac_address,
                         uint16_t target_descriptor_index,
                         uint8_t *control_value,
                         uint16_t control_value_len )  {
        uint8_t additional1[4];
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_CONTROL,additional1,0);
        jdksavdecc_uint16_set(target_descriptor_index,additional1,2);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_SET_CONTROL,
                    additional1,sizeof(additional1),
                    control_value, control_value_len );
    }

    uint8_t ReceiveSetControlCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveSetControlResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a GET_CONTROL command to a target entity
    void SendGetControl( jdksavdecc_eui64 const &target_entity_id,
                         jdksavdecc_eui48 const &target_mac_address,
                         uint16_t target_descriptor_index ) {
        uint8_t additional1[4];
        jdksavdecc_uint16_set(JDKSAVDECC_DESCRIPTOR_CONTROL,additional1,0);
        jdksavdecc_uint16_set(target_descriptor_index,additional1,2);
        SendCommand(target_entity_id,
                    target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_GET_CONTROL,
                    additional1,sizeof(additional1));
    }

    uint8_t ReceiveGetControlCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveGetControlResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Notification that a new control value was received from the target entity,
    // either because of a solicited or unsolicited SET_CONTROL response or
    // a GET_CONTROL response
    virtual uint8_t ReceiveControlValue(
            jdksavdecc_eui64 const &target_entity_id,
            uint16_t target_descriptor_index,
            uint8_t const *control_value,
            uint16_t control_value_len
            );

    // Formulate and send an REGISTER_UNSOLICITED_NOTIFICATION command to a target entity
    void SendRegisterUnsolicitedNotification( jdksavdecc_eui64 const &target_entity_id,
                              jdksavdecc_eui48 const &target_mac_address) {
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_REGISTER_UNSOLICITED_NOTIFICATION,0,0);
    }

    uint8_t ReceiveRegisterUnsolicitedNotificationCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveRegisterUnsolicitedNotificationResponse( uint8_t *buf, uint16_t pos, uint16_t len );

    // Formulate and send a DEREGISTER_UNSOLICITED_NOTIFICATION command to a target entity
    void SendDeRegisterUnsolicitedNotification( jdksavdecc_eui64 const &target_entity_id,
                              jdksavdecc_eui48 const &target_mac_address) {
        SendCommand(target_entity_id,target_mac_address,JDKSAVDECC_AEM_COMMAND_DEREGISTER_UNSOLICITED_NOTIFICATION,0,0);
    }

    uint8_t ReceiveDeRegisterUnsolicitedNotificationCommand( uint8_t *buf, uint16_t pos, uint16_t len );

    virtual bool ReceiveDeRegisterUnsolicitedNotificationResponse( uint8_t *buf, uint16_t pos, uint16_t len );


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
