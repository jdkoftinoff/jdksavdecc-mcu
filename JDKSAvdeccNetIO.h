#pragma once

#include "JDKSAvdeccWorld.h"


namespace JDKSAvdecc {


/// Base clase for sending or receiving raw ethernet frames
class NetIO {
public:
    NetIO();
    virtual ~NetIO();

    /// Do any post-constructor initialization
    virtual void Initialize() = 0;

    virtual jdksavdecc_eui48 const &GetMacAddress() const = 0;

    /// Poll incoming data for raw ethernet frame
    /// Returns length of entire frame received, or 0 if none.
    /// data will contain entire raw ethernet frame
    virtual uint16_t ReceiveRawNet(uint8_t *data,
                                   uint16_t max_len ) = 0;

    /// Send raw etherent frame.
    virtual bool SendRawNet(jdksavdecc_eui48 const *dest_mac,
                            uint8_t const *data,
                            uint16_t len ) = 0;

};

}
