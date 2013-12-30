#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccHandlerGroup.h"

namespace JDKSAvdecc {

HandlerGroupBase::HandlerGroupBase(Handler **item_storage)
    : m_num_items(0)
    , m_rx_count(0)
    , m_handled_count(0)
    , m_item(item_storage) {}


// Poll the NetIO object for an incoming frame. If it is multicast, or m
bool HandlerGroupBase::PollNet( uint32_t time_in_millis ) {
    bool r=false;
    uint8_t buf[JDKSAVDECC_FRAME_MAX_SIZE];
    uint16_t len;
    // Try receive data
    len = net->ReceiveRawNet(buf,sizeof(buf));

    // Make sure we read DA,SA,Ethertype
    if( len>JDKSAVDECC_FRAME_HEADER_LEN ) {
        // Make sure it is AVTP/AVDECC ethertype

        if( (buf[JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET+0] == ((JDKSAVDECC_AVTP_ETHERTYPE>>8)&0xff))
           && (buf[JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET+1] == ((JDKSAVDECC_AVTP_ETHERTYPE>>0)&0xff))) {

            jdksavdecc_eui48 const &my_mac = net->GetMACAddress();

            // Make sure it is either multicast or is for me
            if( (buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+0]&0x1) ||
               ( buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+0] == my_mac.value[0] &&
                buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+1] == my_mac.value[1] &&
                buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+2] == my_mac.value[2] &&
                buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+3] == my_mac.value[3] &&
                buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+4] == my_mac.value[4] &&
                buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+5] == my_mac.value[5] ) )
            {
                // Ok, this PDU is worth spending time on. Send it on to all known Handlers.

                m_rx_count++;
                r=ReceivedPDU( time_in_millis, buf, len );
                if( r ) {
                    m_handled_count++;
                }
            }
        }
    }
    return r;
}

/// Send Tick() messages to all encapsulated Handlers
/// and poll incoming network for PDU's and dispatch them
void HandlerGroupBase::Tick( uint32_t time_in_millis ) {
    PollNet( time_in_millis );
    for( uint16_t i=0; i<m_num_items; ++i ) {
        m_item[i]->Tick(time_in_millis);
    }
}

/// Send ReceivedPDU message to each handler until one returns true.
bool HandlerGroupBase::ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
    bool r=false;
    for( uint16_t i=0; i<m_num_items; ++i ) {
        if( m_item[i]->ReceivedPDU(time_in_millis,buf,len) ) {
            r=true;
            break;
        }
    }
    return r;
}

}
