#pragma once

#include "JDKSAvdeccNet.h"

#include "JDKSAvdeccNet.h"

namespace JDKSAvdecc {

class WizNetIO : public NetIO {
public:
    virtual ~WizNetIO() {}

    virtual bool CanReceiveRawNet();
    virtual uint16_t ReceiveRawNet( uint8_t const *base_ptr, uint16_t max_len );
    virtual bool StartSendRawNet();
    virtual void SendRawNetBytes( uint8_t *ptr, uint16_t len );
    virtual void SendRawNetOctet( uint8_t val );
    virtual void SendRawNetDoublet( uint16_t val );
    virtual void SendRawNetQuadlet( uint32_t val );
    virtual void SendRawNetEui48( jdksavdecc_eui48 val );
    virtual void SendRawNetEui64( jdksavdecc_eui64 val );
    virtual void FinishSendRawNet();
};

}
