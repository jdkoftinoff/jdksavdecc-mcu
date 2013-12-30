#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"
#include "JDKSAvdeccHandler.h"

namespace JDKSAvdecc {


/// The HandlerGroupBase class maintains a list of Handler pointers
/// Dispatches Tick() and ReceivedPDU() calls to all Handlers
/// It does not contain the storage of the handlers
/// No bounds checking is done
class HandlerGroupBase : public Handler {
protected:
    uint16_t m_num_items;
    Handler **m_item;
    uint32_t m_rx_count;
    uint32_t m_handled_count;
public:
    HandlerGroupBase(Handler **item_storage);

    /// Add a handler to the list
    void Add( Handler *v ) {
        m_item[m_num_items++] = v;
    }

    uint32_t GetRxCount() const { return m_rx_count; }
    uint32_t GetHandledCount() const { return m_handled_count; }

    // Poll the NetIO object for an incoming frame. If it is multicast, or m
    virtual bool PollNet( uint32_t time_in_millis );

    /// Send Tick() messages to all encapsulated Handlers
    /// and poll incoming network for PDU's and dispatch them
    virtual void Tick( uint32_t time_in_millis );

    /// Send ReceivedPDU message to each handler until one returns true.
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );
};


/// HandlerGroup is a HandlerGroupBase and contains
/// the storage of the contained Handler pointers.
/// The HandlerGroup is templatelized by the MaxItem count.
template <uint16_t MaxItems>
class HandlerGroup : public HandlerGroupBase {
private:
    Handler *m_item_storage[MaxItems];
public:
    HandlerGroup()
        : HandlerGroupBase(m_item_storage) {}
};

}

