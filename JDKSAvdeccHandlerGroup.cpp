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
        // Ok, this PDU is worth spending time on. Send it on to all known Handlers.

        m_rx_count++;
        r=ReceivedPDU( time_in_millis, buf, len );
        if( r ) {
            m_handled_count++;
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
