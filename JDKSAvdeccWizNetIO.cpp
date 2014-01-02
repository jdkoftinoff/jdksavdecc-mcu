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


#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccWizNetIO.h"

extern "C" {
void avr_debug_log(const char *str, uint16_t v );
}

#ifdef __AVR__
#include "utility/w5100.h"
#include "utility/socket.h"

namespace JDKSAvdecc {

WizNetIO::~WizNetIO() {
    close(0);
}


void WizNetIO::Initialize() {
    W5100.init();
    W5100.setMACAddress(m_mac_address.value);
    //W5100.setIPAddress(IPAddress(0,0,0,0).raw_address());
    socket(0,SnMR::MACRAW,0x22f0,0);
}


uint16_t WizNetIO::ReceiveRawNet(uint8_t *data,
                                 uint16_t max_len ) {
    uint8_t head[2];
    uint16_t data_len=0;
    uint16_t ptr=0;
    uint16_t r=0;

    if( (W5100.readSnIR(0)&SnIR::RECV) !=0 ) {
        ptr = W5100.readSnRX_RD(0);
        W5100.read_data(0,(uint8_t*)ptr,head,2);
        ptr+=2;
        data_len = head[0];
        data_len = (data_len<<8) + head[1] - 2;
        // read the frame header to make sure we care
        W5100.read_data(0,(uint8_t*)ptr,data,JDKSAVDECC_FRAME_HEADER_LEN);
        ptr += JDKSAVDECC_FRAME_HEADER_LEN;
        // we care if it is ethertype 0x22f0
        if( (data[JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET+0]==0x22) &&
             (data[JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET+1]==0xf0) ) {

            // we care if it is for me or the ADP/ACMP multicast address
            if( memcmp(&data[JDKSAVDECC_FRAME_HEADER_DA_OFFSET],m_mac_address.value,6)==0 ||
                    memcmp(&data[JDKSAVDECC_FRAME_HEADER_DA_OFFSET],jdksavdecc_multicast_adp_acmp.value,6)==0) {
                // ok, then read the rest of the data
                if( data_len<=max_len ) {
                    W5100.read_data(0,(uint8_t*) ptr,data+JDKSAVDECC_FRAME_HEADER_LEN,data_len-JDKSAVDECC_FRAME_HEADER_LEN);
                    r=data_len;
                } else {
                    //            avr_debug_log("pkt len", data_len );
                }
            }
        }
        // update the pointers in the chip regardless if we read the packet
        ptr += data_len-JDKSAVDECC_FRAME_HEADER_LEN;
        W5100.writeSnRX_RD(0, ptr);
        W5100.writeSnIR(0, SnIR::RECV);
        W5100.execCmdSn(0, Sock_RECV);
    }
    return r;
}

bool WizNetIO::SendRawNet(uint8_t const *data,
                          uint16_t len,
                          uint8_t const *data1,
                          uint16_t len1,
                          uint8_t const *data2,
                          uint16_t len2 ) {

    uint8_t try_count=0;
    bool done=false;
    // Try up to 3 times to send
    while( !done && try_count<3 ) {
        ++try_count;
        uint8_t wait_count=0;

        // Tell W5100 the destination hardware address
        W5100.writeSnDHAR(0, (uint8_t*)&data[JDKSAVDECC_FRAME_HEADER_DA_OFFSET] );

        // Send the data chunk
        W5100.send_data_processing(0, (uint8_t *)data, len);

        // If there is a data1 chunk then send it
        if( data1!=0 && len1>0) {
            W5100.send_data_processing(0, (uint8_t *)data1, len1);
        }

        // If there is a data2 chunk then send it
        if( data2!=0 && len2>0 ) {
            W5100.send_data_processing(0, (uint8_t *)data2, len2);
        }

        // Tell W5100 to send the raw ethernet frame now
        W5100.execCmdSn(0, Sock_SEND_MAC);

        // assume we have success
        done=true;

        // Check for an OK signal from the chip
        while ( (W5100.readSnIR(0) & SnIR::SEND_OK) != SnIR::SEND_OK )
        {
            // Was it a timeout message?
            if (W5100.readSnIR(0) & SnIR::TIMEOUT)
            {
                // Yes, acknowledge the timeout and maybe try again
                W5100.writeSnIR(0, (SnIR::SEND_OK | SnIR::TIMEOUT));
                done=false;
                break;
            }
            // Did we wait too long?
            if( wait_count++>8 ) {
                // Yes, try again
                done=false;
                break;
            }
        }

        W5100.writeSnIR(0, SnIR::SEND_OK);
    }
#if JDKSAVDECCWINNETIO_DEBUG
    // Count the sent packets
    if( done ) {
        m_sent_packet_count++;
        if( (m_sent_packet_count&0xff)==0) {
            avr_debug_log("sent:",m_sent_packet_count);
        }
    } else {
        m_unsent_packet_count++;
        avr_debug_log("unsent:",m_sent_packet_count);
    }
#endif
    return done;
}

bool WizNetIO::SendReplyRawNet(uint8_t const *data,
                               uint16_t len,
                               uint8_t const *data1,
                               uint16_t len1,
                               uint8_t const *data2,
                               uint16_t len2 ) {
    uint8_t *p = (uint8_t *)data;
    // Set DA to what was SA, set SA to m_mac_address
    for( uint8_t i=0; i<6; ++ i ) {
        p[i+JDKSAVDECC_FRAME_HEADER_DA_OFFSET] = p[i+JDKSAVDECC_FRAME_HEADER_SA_OFFSET];
        p[i+JDKSAVDECC_FRAME_HEADER_SA_OFFSET] = m_mac_address.value[i];
    }
    return SendRawNet(data,len,data1,len1,data2,len2);
}

}

#endif
