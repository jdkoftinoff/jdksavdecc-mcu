
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

    W5100.writeSnDHAR(0, (uint8_t*)&data[JDKSAVDECC_FRAME_HEADER_DA_OFFSET] );

    W5100.send_data_processing(0, (uint8_t *)data, len);
    if( data1!=0 && len1>0) {
        W5100.send_data_processing(0, (uint8_t *)data1, len1);
    }
    if( data2!=0 && len2>0 ) {
        W5100.send_data_processing(0, (uint8_t *)data2, len2);
    }
    W5100.execCmdSn(0, Sock_SEND_MAC);

    while ( (W5100.readSnIR(0) & SnIR::SEND_OK) != SnIR::SEND_OK )
    {
        if (W5100.readSnIR(0) & SnIR::TIMEOUT)
        {
            W5100.writeSnIR(0, (SnIR::SEND_OK | SnIR::TIMEOUT));
            return 0;
        }
    }

    W5100.writeSnIR(0, SnIR::SEND_OK);
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
