#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccHandler.h"

namespace JDKSAvdecc {

Handler::~Handler() {
}

void Handler::Tick( uint32_t time_in_millis ) {
}

bool Handler::ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
    return false;
}


/// Helper function to parse AECP AEM message
bool Handler::ParseAEM(jdksavdecc_aecpdu_aem *aem,
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

/// Test to see if AECPDU AEM message is a command for the specified target entity
bool Handler::IsAEMForTarget(jdksavdecc_aecpdu_aem const &aem,
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

    /// Test to see if AECPDU AEM message is a response for the specified controller entity
bool Handler::IsAEMForController(jdksavdecc_aecpdu_aem const &aem,
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


    /// Helper function to parse AECP AA message
bool Handler::ParseAA(jdksavdecc_aecp_aa *aa,
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

    /// Test to see if AECPDU AA message is a command for the specified target entity
bool Handler::IsAAForTarget(jdksavdecc_aecp_aa const &aa,
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

    /// Test to see if AECPDU AA message is a response for the specified controller entity
bool Handler::IsAAForController(jdksavdecc_aecp_aa const &aa,
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

}



