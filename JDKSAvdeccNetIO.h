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

    /// Get the MAC address of the ethernet port
    virtual jdksavdecc_eui48 const &GetMACAddress() const = 0;

    /// Poll incoming data for raw ethernet frame
    /// Returns length of entire frame received, or 0 if none.
    /// data will contain entire raw ethernet frame
    virtual uint16_t ReceiveRawNet(uint8_t *data,
                                   uint16_t max_len ) = 0;

    /// Send raw etherent frame.
    virtual bool SendRawNet(uint8_t const *data,
                            uint16_t len,
                            uint8_t const *data1=0,
                            uint16_t len1=0,
                            uint8_t const *data2=0,
                            uint16_t len2=0 ) = 0;

    /// Reply to the sender of the frame (Same as SendRawNet put swaps SA/DA
    virtual bool SendReplyRawNet(uint8_t const *data,
                                 uint16_t len,
                                 uint8_t const *data1=0,
                                 uint16_t len1=0,
                                 uint8_t const *data2=0,
                                 uint16_t len2=0 ) = 0;


};

extern NetIO *net;

}
