/*
  Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "JDKSAvdeccWorld.hpp"


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
