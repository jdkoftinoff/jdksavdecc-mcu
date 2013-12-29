#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"
#include "JDKSAvdeccFrame.h"
#include "JDKSAvdeccHandler.h"
#include "JDKSAvdeccControlValueHolder.h"

namespace JDKSAvdecc {

template <size_t MaxDescriptors>
class ControlReceiver : public Handler {
public:
    /// Construct the ControlReceiver object
    ControlReceiver(NetIO &net,
               jdksavdecc_eui64 const &entity_id,
               uint16_t descriptor_index_offset=0 )
    : m_net( net )
    , m_entity_id( entity_id )
    , m_descriptor_index_offset( descriptor_index_offset )
    , m_num_descriptors(0)
    , m_last_sequence_id(0xffff) {}

    /// Register the ControlValueHolder to relate to the next descriptor_index in line
    void AddDescriptor(ControlValueHolder *v ) {
        m_values[ m_num_descriptors++ ] = v;
    }

    virtual bool ReceivedGetControlCommand( uint32_t time_in_millis,
                                           jdksavdecc_aecpdu_aem const &aem,
                                           uint8_t *buf,
                                           uint16_t pos,
                                           uint16_t len ) {
        bool r=false;
        // yes, get the descriptor index
        uint16_t descriptor_index = jdksavdecc_aem_command_set_control_get_descriptor_index(buf,pos);
        // is it a descriptor index that we care about?
        if( (descriptor_index>=m_descriptor_index_offset)
           && (descriptor_index < (m_descriptor_index_offset+m_num_descriptors)) ) {
            // yes, it is in range, extract the data value

            // TODO Add GetValue method in ControlValueHolder.
//            m_values[descriptor_index-m_descriptor_index_offset]->SetValue(
//                                                                           &buf[pos+JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_OFFSET_VALUES] );
            // TODO: Set control_data_length appropriately.

            r=true;
        }
        return r;
    }

    virtual bool ReceivedSetControlCommand( uint32_t time_in_millis,
                                            jdksavdecc_aecpdu_aem const &aem,
                                            uint8_t *buf,
                                            uint16_t pos,
                                            uint16_t len ) {
        bool r=false;
        // yes, get the descriptor index
        uint16_t descriptor_index = jdksavdecc_aem_command_set_control_get_descriptor_index(buf,pos);
        // is it a descriptor index that we care about?
        if( (descriptor_index>=m_descriptor_index_offset)
           && (descriptor_index < (m_descriptor_index_offset+m_num_descriptors)) ) {
            // yes, it is in range, extract the data value

            m_values[descriptor_index-m_descriptor_index_offset]->SetValue(
                                                                           &buf[pos+JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_OFFSET_VALUES] );

            r=true;
        }
        return r;
    }

    virtual bool ReceivedReadDescriptorCommand( uint32_t time_in_millis,
                                        jdksavdecc_aecpdu_aem const &aem,
                                        uint8_t *buf,
                                        uint16_t pos,
                                        uint16_t len ) {
        bool r=false;
        return r;

    }

    /// Handle incoming PDU
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
        bool r=false;
        // we already know the message is AVTP ethertype and is either directly
        // targetting my MAC address or is a multicast message

        // DA (6), SA (6), Ethertype (2) brings us to subtype byte at 14
        ssize_t pos=14;

        // Try see if it is an AEM message
        jdksavdecc_aecpdu_aem aem;
        if( ParseAEM(&aem,buf,pos,len)) {
            // Yes, Is it a command for me?
            if( IsAEMForTarget(aem,m_entity_id)) {
                // Yes. Is the sequence_id ok?
                if( aem.sequence_id != m_last_sequence_id )
                {
                    // Yes, it is different.
                    m_last_sequence_id = aem.sequence_id;
                    if( aem.command_type == JDKSAVDECC_AEM_COMMAND_GET_CONTROL ) {
                        // Handle GET_CONTROL commands
                        r=ReceivedGetControlCommand( time_in_millis, aem, buf, pos, len );
                    } else if( aem.command_type == JDKSAVDECC_AEM_COMMAND_SET_CONTROL ) {
                        // Handle SET_CONTROL commands
                        r=ReceivedSetControlCommand( time_in_millis, aem, buf, pos, len );
                    } else if( aem.command_type == JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR ) {
                        // Handle READ_DESCRIPTOR commands
                        r=ReceivedReadDescriptorCommand( time_in_millis, aem, buf, pos, len );
                    }
                }
            }
        }
        if( r ) {
            // TODO: change message type to AEM_RESPONSE and send it back to sender
        }
        return r;
    }

    /// Get the enitity ID that we are handling
    jdksavdecc_eui64 const &GetEntityID() const { return m_entity_id; }

protected:
    NetIO &m_net;
    jdksavdecc_eui64 const &m_entity_id;
    uint16_t m_descriptor_index_offset;
    uint16_t m_num_descriptors;
    ControlValueHolder *m_values[MaxDescriptors];
    uint16_t m_last_sequence_id;
};

}
