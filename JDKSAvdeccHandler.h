#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"

namespace JDKSAvdecc {

/// Abstract base class for classes that need to be notified
/// of time traversal and received PDU's
class Handler {
public:
    virtual ~Handler();

    /// Notification of time passage
    virtual void Tick( uint32_t time_in_millis );

    /// Notification of received raw PDU. Return true if PDU is handled
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );
};


/// HandlerGroup maintains a list of Handlers and
/// dispatches Ticks and PDUs to them
template <uint16_t MaxItems>
class HandlerGroup : public Handler {
private:
    NetIO &m_net;
    Handler *m_item[MaxItems];
    uint16_t m_num_items;
public:
    HandlerGroup(NetIO &net) : m_net(net), m_num_items(0) {}

    /// Add a handler to the list
    void Add( Handler *v ) {
        m_item[m_num_items++] = v;
    }


    virtual void PollNet( uint32_t time_in_millis ) {
        uint8_t buf[640];
        uint16_t len;
        // Try receive data
        len = m_net.ReceiveRawNet(buf,sizeof(buf));

        jdksavdecc_eui48 const &my_mac = m_net.GetMACAddress();

        // Make sure it is AVTP/AVDECC ethertype
        if( (buf[12] == ((JDKSAVDECC_AVTP_ETHERTYPE>>8)&0xff))
           && (buf[13] == ((JDKSAVDECC_AVTP_ETHERTYPE>>0)&0xff))) {

            // Make sure it is either multicast or is for me
            if( (buf[0]&0x1) ||
               ( buf[0] == my_mac.value[0] &&
                buf[1] == my_mac.value[1] &&
                buf[2] == my_mac.value[2] &&
                buf[3] == my_mac.value[3] ) )
            {
                // Ok, this PDU is worth spending time on. Send it on to all known Handlers.

                ReceivedPDU( time_in_millis, buf, len );
            }
        }
    }

    /// Send Tick() messages to all encapsulated Handlers
    /// and poll incoming network for PDU's and dispatch them
    virtual void Tick( uint32_t time_in_millis ) {
        PollNet( time_in_millis );
        for( uint16_t i=0; i<m_num_items; ++i ) {
            m_item[i]->Tick(time_in_millis);
        }
    }

    /// Send ReceivedPDU message to each handler until one returns true.
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
        bool r=false;
        for( uint16_t i=0; i<m_num_items; ++i ) {
            if( m_item[i]->ReceivedPDU(time_in_millis,buf,len) ) {
                r=true;
                break;
            }
        }
        return r;
    }
};

}

