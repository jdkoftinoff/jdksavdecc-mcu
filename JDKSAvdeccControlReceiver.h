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
    , m_num_descriptors(0) {}

    /// Register the ControlValueHolder to relate to the next descriptor_index in line
    void AddDescriptor(ControlValueHolder *v ) {
        m_values[ m_num_descriptors++ ] = v;
    }

    /// Handle incoming PDU
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
        // we already know the message is AVTP ethertype and is either directly
        // targetting my MAC address or is a multicast message

        // DA (6), SA (6), Ethertype (2) brings us to subtype byte at 14
        ssize_t pos=14;

        // Validate subtype is AECP
        if( jdksavdecc_uint8_get(buf,pos)==(0x80+JDKSAVDECC_SUBTYPE_AECP) ) {
            // Yes, read the header
            jdksavdecc_aecpdu_common_control_header header;
            if( jdksavdecc_aecpdu_common_control_header_read(&header,buf,pos,len)>0 ) {
                // make sure it is version 0 and and AEM_COMMAND
                if( header.version==0 && header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND ) {

                    if( jdksavdecc_eui64_compare( &m_entity_id, &header.target_entity_id )==0 ) {
                        // is it a set_control?

                        uint16_t cmd = jdksavdecc_aecpdu_aem_get_command_type(buf,pos+12);
                        if( cmd == JDKSAVDECC_AEM_COMMAND_SET_CONTROL ) {
                            // yes, get the descriptor index
                            uint16_t descriptor_index = jdksavdecc_aem_command_set_control_get_descriptor_index(buf,pos);
                            // is it a descriptor index that we care about?
                            if( (descriptor_index>=m_descriptor_index_offset)
                               && (descriptor_index < (m_descriptor_index_offset+m_num_descriptors)) ) {
                               // yes, it is in range, extract the data value

                                m_values[descriptor_index-m_descriptor_index_offset]->SetValue( &buf[pos+JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_OFFSET_VALUES] );

                                // TODO: Set message status JDKSAVDECC_AECP_STATUS_SUCCESS
                            }
                            else {
                                // TODO: Set message status to JDKSAVDECC_AECP_STATUS_NOT_IMPLEMENTED
                            }
                            // TODO: change message type to AEM_RESPONSE and send it back to sender
                        }
                    }
                }
            }
        }
    }

    /// Get the enitity ID that we are handling
    jdksavdecc_eui64 const &GetEntityID() const { return m_entity_id; }

protected:
    NetIO &m_net;
    jdksavdecc_eui64 const &m_entity_id;
    uint16_t m_descriptor_index_offset;
    uint16_t m_num_descriptors;
    ControlValueHolder *m_values[MaxDescriptors];
};

}
