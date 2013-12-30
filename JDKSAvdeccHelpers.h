#pragma once

#include "JDKSAvdeccWorld.h"

namespace JDKSAvdecc {

/// Helper function to parse AECP AEM message
bool ParseAEM(jdksavdecc_aecpdu_aem *aem,
              uint8_t const *buf,
              uint16_t pos,
              uint16_t len);

/// Test to see if AECPDU AEM message is a command for the specified target entity
bool IsAEMForTarget(jdksavdecc_aecpdu_aem const &aem,
                    jdksavdecc_eui64 const &expected_target_entity_id);

/// Test to see if AECPDU AEM message is a response for the specified controller entity
bool IsAEMForController(jdksavdecc_aecpdu_aem const &aem,
                       jdksavdecc_eui64 const &expected_controller_entity_id);

/// Twiddle the header bytes to convert this message from AEM_COMMAND to AEM_RESPONSE in place
/// with the new status code, and the new control_data_length to match the new_length
void SetAEMReply( uint8_t status_code,
                  uint16_t new_length,
                  uint8_t *buf,
                  uint16_t pos,
                  uint16_t len );

/// Helper function to parse AECP AA message
bool ParseAA(jdksavdecc_aecp_aa *aa,
             uint8_t const *buf,
             uint16_t pos,
             uint16_t len);

/// Test to see if AECPDU AA message is a command for the specified target entity
bool IsAAForTarget(jdksavdecc_aecp_aa const &aa,
                    jdksavdecc_eui64 const &expected_target_entity_id);

/// Test to see if AECPDU AA message is a response for the specified controller entity
bool IsAAForController(jdksavdecc_aecp_aa const &aa,
                        jdksavdecc_eui64 const &expected_controller_entity_id);

/// Twiddle the header bytes to convert this message from ADDRESS_ACCESS_COMMAND to ADDRESS_ACCESS_RESPONSE in place
/// with the new status code, and the new control_data_length to match the new_length
void SetAAReply( uint16_t new_length, uint8_t *buf, uint16_t pos, uint16_t len );

/// Formulate an AEM SET_CONTROL style message with payload
void FormAEMSetControl( jdksavdecc_eui48 const &dest_mac,
                        jdksavdecc_eui48 const &src_mac,
                        jdksavdecc_eui64 const &my_controller_entity_id,
                        jdksavdecc_eui64 const &destination_entity_id,
                        bool unsolicited,
                        uint16_t descriptor_index,
                        uint8_t const *value_data,
                        uint16_t value_data_length,
                        uint8_t *buf );


inline bool ParseAEM(jdksavdecc_aecpdu_aem *aem,
              uint8_t const *buf,
              uint16_t pos,
              uint16_t len) {
    bool r=false;
    // Validate subtype is AECP
    if( jdksavdecc_uint8_get(buf,pos)==(0x80+JDKSAVDECC_SUBTYPE_AECP) ) {
        // Yes, read the aem header
        if( jdksavdecc_aecpdu_aem_read(aem,buf,pos,len)>0 ) {
            // make sure it is version 0 and an AEM_COMMAND or AEM_RESPONSE
            if( aem->aecpdu_header.header.version==0 &&
               (aem->aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND ||
                aem->aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE) ) {
                   r=true;
            }
        }
    }
    return r;
}

inline bool IsAEMForTarget(jdksavdecc_aecpdu_aem const &aem,
                    jdksavdecc_eui64 const &expected_target_entity_id) {
    bool r=false;
    // Is it an AEM_COMMAND?
    if(aem.aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND) {
        // Yes
        // Is it for me?
        if( jdksavdecc_eui64_compare( &expected_target_entity_id, &aem.aecpdu_header.header.target_entity_id )==0 ) {
            r=true;
        }
    }
    return r;
}

inline bool IsAEMForController(jdksavdecc_aecpdu_aem const &aem,
                        jdksavdecc_eui64 const &expected_controller_entity_id) {
    bool r=false;
    // Is it an AEM_RESPONSE?
    if(aem.aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE) {
        // Yes
        // Is it for me?
        if( jdksavdecc_eui64_compare( &expected_controller_entity_id, &aem.controller_entity_id )==0 ) {
            r=true;
        }
    }
    return r;

}

inline void SetAEMReply( uint8_t status_code,
                  uint16_t new_length,
                  uint8_t *buf,
                  uint16_t pos,
                  uint16_t len ) {
    // offset 1: sv=0, version=0, control_data = AEM_RESPONSE
    buf[pos+1] = JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE;
    // new control data length is new_length minus frame beginning position pos and the common control header.
    // See IEEE 1722-2011 Clause 5.3.3
    uint16_t control_data_length=new_length-pos-JDKSAVDECC_COMMON_CONTROL_HEADER_LEN;
    // offset 2: status = status code, top 3 bits of new control_data_length
    buf[pos+2] = (status_code<<3) + ((control_data_length>>8)&0x7);
    // offset 3: bottom 8 bits of new control_data_length
}


inline bool ParseAA(jdksavdecc_aecp_aa *aa,
             uint8_t const *buf,
             uint16_t pos,
             uint16_t len) {
    bool r=false;
    // Validate subtype is AECP
    if( jdksavdecc_uint8_get(buf,pos)==(0x80+JDKSAVDECC_SUBTYPE_AECP) ) {
        // Yes, read the aem header

        if( jdksavdecc_aecp_aa_read(aa,buf,pos,len)>0 ) {
            // make sure it is version 0 and an AA_COMMAND or AA_RESPONSE
            if( aa->aecpdu_header.header.version==0 &&
               (aa->aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND ||
                aa->aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE) ) {
                   r=true;
               }
        }
    }
    return r;
}

inline bool IsAAForTarget(jdksavdecc_aecp_aa const &aa,
                            jdksavdecc_eui64 const &expected_target_entity_id) {
    bool r=false;

    // Is it an AA_COMMAND?
    if(aa.aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND) {
        // Yes
        // Is it for me?
        if( jdksavdecc_eui64_compare( &expected_target_entity_id, &aa.aecpdu_header.header.target_entity_id )==0 ) {
            r=true;
        }
    }
    return r;
}

inline bool IsAAForController(jdksavdecc_aecp_aa const &aa,
                                jdksavdecc_eui64 const &expected_controller_entity_id)  {
    bool r=false;

    // Is it an AA_RESPONSE?
    if(aa.aecpdu_header.header.message_type==JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE) {
        // Yes
        // Is it for me?
        if( jdksavdecc_eui64_compare( &expected_controller_entity_id, &aa.controller_entity_id )==0 ) {
            r=true;
        }
    }
    return r;
}

inline void SetAAReply( uint8_t status_code,
                  uint16_t new_length,
                  uint8_t *buf,
                  uint16_t pos,
                  uint16_t len ) {
    // offset 1: sv=0, version=0, control_data = ADDRESS_ACCESS_RESPONSE
    buf[pos+1] = JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE;
    // new control data length is new_length minus frame beginning position pos and the common control header.
    // See IEEE 1722-2011 Clause 5.3.3
    uint16_t control_data_length=new_length-pos-JDKSAVDECC_COMMON_CONTROL_HEADER_LEN;
    // offset 2: status = status code, top 3 bits of new control_data_length
    buf[pos+2] = (status_code<<3) + ((control_data_length>>8)&0x7);
    // offset 3: bottom 8 bits of new control_data_length
}

}

