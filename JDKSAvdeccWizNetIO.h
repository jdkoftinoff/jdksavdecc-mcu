#pragma once

#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccNetIO.h"

namespace JDKSAvdecc {

#ifdef __AVR__
class WizNetIO : public NetIO {
public:
    WizNetIO();
    virtual ~WizNetIO();
    virtual void Initialize();

    virtual uint16_t ReceiveRawNet(uint8_t *data,
                                   uint16_t max_len );

    virtual bool SendRawNet(jdksavdecc_eui48 const *dest_mac,
                            uint8_t const *data,
                            uint16_t len );


private:
};

#endif

}
