#pragma once

#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccNetIO.h"

namespace JDKSAvdecc {

#ifdef __AVR__
class WizNetIO : public NetIO {
public:
    WizNetIO( jdksavdecc_eui48 const &mac_address )
    : m_mac_address(mac_address) {}
    
    virtual ~WizNetIO();
    virtual void Initialize();

    virtual jdksavdecc_eui48 const &GetMacAddress() const {
        return m_mac_address;
    }

    virtual uint16_t ReceiveRawNet(uint8_t *data,
                                   uint16_t max_len );

    virtual bool SendRawNet(uint8_t const *data,
                            uint16_t len );

private:
    jdksavdecc_eui48 m_mac_address;
};

#endif

}
