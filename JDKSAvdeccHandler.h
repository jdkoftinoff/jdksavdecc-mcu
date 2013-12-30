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

}

