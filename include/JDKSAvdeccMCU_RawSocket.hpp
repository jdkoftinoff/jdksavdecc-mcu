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

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_RawSocketBase.hpp"

namespace JDKSAvdeccMCU
{

class RawSocket : public RawSocketBase
{
    filedescriptor_t m_fd;
    uint16_t m_ethertype;
    jdksavdecc_eui48 m_my_mac;
    jdksavdecc_eui48 m_default_dest_mac;
    int m_interface_id;
    void *m_pcap;

  public:
    RawSocket( uint16_t ethertype, const char *interface_name, const jdksavdecc_eui48 *join_multicast = 0 );

    ~RawSocket();

    virtual jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds() { return ::getTimeInMilliseconds(); }

    virtual bool recvFrame( FrameBase *frame ) = 0;

    virtual bool sendFrame(
        FrameBase const &frame, uint8_t const *data1 = 0, uint16_t len1 = 0, uint8_t const *data2 = 0, uint16_t len2 = 0 ) = 0;

    virtual bool sendReplyFrame(
        FrameBase const &frame, uint8_t const *data1 = 0, uint16_t len1 = 0, uint8_t const *data2 = 0, uint16_t len2 = 0 ) = 0;

    virtual bool joinMulticast( const jdksavdecc_eui48 &multicast_mac );

    virtual void setNonblocking();

    virtual filedescriptor_t getFd() const { return m_fd; }

    virtual jdksavdecc_eui48 const &getMACAddress() const { return m_my_mac; }

    virtual void initialize();
};
}
