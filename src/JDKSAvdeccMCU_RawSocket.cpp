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

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_RawSocket.hpp"

namespace JDKSAvdeccMCU
{

RawSocket *RawSocket::net[JDKSAVDECCMCU_MAX_RAWSOCKETS] = {};
uint16_t RawSocket::num_rawsockets = 0;
uint16_t RawSocket::last_recv = 0;

bool RawSocket::multiRecvFrame( FrameBase *frame )
{
    bool r = false;
    if ( net[last_recv]->recvFrame( frame ) )
    {
        r = true;
    }
    last_recv = ( last_recv + 1 ) % num_rawsockets;
    return r;
}

bool RawSocket::multiSendFrame(
    FrameBase const &frame, uint8_t const *data1, uint16_t len1, uint8_t const *data2, uint16_t len2 )
{
    for ( uint16_t i = 0; i < num_rawsockets; ++i )
    {
        if ( ( frame.getBuf()[JDKSAVDECC_FRAME_HEADER_DA_OFFSET] & 0x01 )
             || ( memcmp( frame.getBuf() + JDKSAVDECC_FRAME_HEADER_SA_OFFSET, net[i]->getMACAddress().value, 6 ) == 0 ) )
        {
            net[i]->sendFrame( frame, data1, len1, data2, len2 );
        }
    }
    return true;
}

bool
    RawSocket::multiSendReplyFrame( FrameBase &frame, uint8_t const *data1, uint16_t len1, uint8_t const *data2, uint16_t len2 )
{
    for ( uint16_t i = 0; i < num_rawsockets; ++i )
    {
        if ( ( frame.getBuf()[JDKSAVDECC_FRAME_HEADER_SA_OFFSET] & 0x01 )
             || ( memcmp( frame.getBuf() + JDKSAVDECC_FRAME_HEADER_DA_OFFSET, net[i]->getMACAddress().value, 6 ) == 0 ) )
        {
            net[i]->sendReplyFrame( frame, data1, len1, data2, len2 );
        }
    }
    return true;
}
}
