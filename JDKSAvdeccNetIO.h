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

#ifndef __AVR__

class NetIODummy {
public:
    NetIODummy() {}
    virtual ~NetIODummy() {}

    virtual void Initialize();

    virtual uint16_t ReceiveRawNet(uint8_t *data,
                                   uint16_t max_len ) {
        return 0;
    }


    virtual bool SendRawNet(jdksavdecc_eui48 const *dest_mac,
                            uint8_t const *data,
                            uint16_t len )
        std::cout << __PRETTY_FUNCTION__ << ":" << std::endl;
        std::cout << "dest_mac" << ":" << std::endl;
        for( uint16_t i=0; i<sizeof(dest_mac->values); ++i ) {
            std::cout << std::hex << dest_mac->values[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "data" << ":" << std::endl;
        for( uint16_t i=0; i<len; ++i ) {
            std::cout << std::hex << data[i] << " ";
        }
        std::cout << std::endl;
    }
};

#endif

}
