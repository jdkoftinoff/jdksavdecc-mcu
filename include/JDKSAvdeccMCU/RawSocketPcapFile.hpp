#pragma once

/*
Copyright (c) 2014, Jeff Koftinoff
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/RawSocket.hpp"
#include "JDKSAvdeccMCU/PcapFileReader.hpp"
#include "JDKSAvdeccMCU/PcapFileWriter.hpp"

#if JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE && JDKSAVDECCMCU_ENABLE_PCAPFILE
namespace JDKSAvdeccMCU
{
class RawSocketPcapFile : public RawSocket
{
    uint16_t m_ethertype;
    Eui48 m_my_mac;
    Eui48 m_default_dest_mac;
    Eui48 m_join_multicast;
    PcapFileReader m_pcap_file_reader;
    PcapFileWriter m_pcap_file_writer;
    jdksavdecc_timestamp_in_milliseconds m_current_time;
    jdksavdecc_timestamp_in_milliseconds m_time_granularity_in_ms;
    FrameWithSize<1500> m_next_incoming_frame;

    RawSocketPcapFile( RawSocketPcapFile const &other );

  public:
    /**
    *  Open a raw socket connected to the specified interface name and join the
    *  specified multicast address
    */
    RawSocketPcapFile(
        uint16_t ethertype,
        Eui48 my_mac,
        Eui48 default_dest_mac,
        Eui48 join_multicast,
        const std::string &input_file,
        const std::string &output_file,
        jdksavdecc_timestamp_in_milliseconds time_granularity_in_ms );

    ~RawSocketPcapFile();

    virtual void setHandlerGroup( HandlerGroup *handler_group )
    {
        m_handler_group = handler_group;
    }

    virtual jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds();

    virtual bool recvFrame( Frame *frame );

    virtual bool sendFrame( Frame const &frame,
                            uint8_t const *data1,
                            uint16_t len1,
                            uint8_t const *data2,
                            uint16_t len2 );

    virtual bool sendReplyFrame( Frame &frame,
                                 uint8_t const *data1,
                                 uint16_t len1,
                                 uint8_t const *data2,
                                 uint16_t len2 );

    virtual bool joinMulticast( const Eui48 &multicast_mac );

    virtual void setNonblocking();

    virtual const Eui48 &getMACAddress() const;

    virtual void initialize() {}

    virtual const char *getDeviceName() const { return "RawSocketPcapFile"; }

  private:
    bool readNextIncomingFrame();
    HandlerGroup *m_handler_group;
};
}
#endif
