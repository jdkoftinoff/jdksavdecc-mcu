#pragma once

#include "JDKSAvdeccWorld.h"

namespace JDKSAvdecc {

class NetIO {
public:
    virtual ~NetIO() {}

    virtual bool CanReceiveRawNet() = 0;
    virtual uint16_t ReceiveRawNet( uint8_t const *base_ptr, uint16_t max_len ) = 0;
    virtual bool StartSendRawNet() = 0;
    virtual void SendRawNetBytes( uint8_t *ptr, uint16_t len ) = 0;
    virtual void SendRawNetOctet( uint8_t val ) = 0;
    virtual void SendRawNetDoublet( uint16_t val ) = 0;
    virtual void SendRawNetQuadlet( uint32_t val ) = 0;
    virtual void SendRawNetEui48( jdksavdecc_eui48 val ) = 0;
    virtual void SendRawNetEui64( jdksavdecc_eui64 val ) = 0;
    virtual void FinishSendRawNet() = 0;
};

}
