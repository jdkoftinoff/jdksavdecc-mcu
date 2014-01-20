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

#include "JDKSAvdeccWorld.hpp"
#include "JDKSAvdeccHandlerGroup.hpp"

namespace JDKSAvdecc {

HandlerGroupBase::HandlerGroupBase(Handler **item_storage)
    : m_num_items(0)
    , m_rx_count(0)
    , m_handled_count(0)
    , m_item(item_storage) {}


// Poll the NetIO object for an incoming frame. If it is multicast, or m
bool HandlerGroupBase::PollNet( jdksavdecc_timestamp_in_milliseconds time_in_millis ) {
    bool r=false;
    uint8_t buf[JDKSAVDECC_AECP_FRAME_MAX_SIZE];
    uint16_t len;
    // Try receive data
    len = net->ReceiveRawNet(buf,sizeof(buf));

    // Make sure we read DA,SA,Ethertype
    if( len>JDKSAVDECC_FRAME_HEADER_LEN ) {
        // Ok, this PDU is worth spending time on. Send it on to all known Handlers.

        m_rx_count++;
        r=ReceivedPDU( time_in_millis, buf, len );
        if( r ) {
            m_handled_count++;
        }
    }
    return r;
}

/// Send Tick() messages to all encapsulated Handlers
/// and poll incoming network for PDU's and dispatch them
void HandlerGroupBase::Tick( jdksavdecc_timestamp_in_milliseconds time_in_millis ) {
    PollNet( time_in_millis );
    for( uint16_t i=0; i<m_num_items; ++i ) {
        m_item[i]->Tick(time_in_millis);
    }
}

/// Send ReceivedPDU message to each handler until one returns true.
bool HandlerGroupBase::ReceivedPDU( jdksavdecc_timestamp_in_milliseconds time_in_millis, uint8_t *buf, uint16_t len ) {
    bool r=false;
    for( uint16_t i=0; i<m_num_items; ++i ) {
        if( m_item[i]->ReceivedPDU(time_in_millis,buf,len) ) {
            r=true;
            break;
        }
    }
    return r;
}

}
