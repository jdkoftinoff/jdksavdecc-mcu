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

#include "JDKSAvdeccMCU_World.hpp"

#include "JDKSAvdeccMCU_NetIO.hpp"

#ifdef __AVR__
#include "JDKSAvdeccMCU_WizNetIO.hpp"
namespace JDKSAvdeccMCU
{
typedef WizNetIO RawNetIO;
}
#elif defined( JDKSAVDECCMCU_ENABLE_RAW )

namespace JDKSAvdeccMCU
{
class RawNetIO : public NetIO
{
#if defined( _WIN32 )
    SOCKET m_fd;
#else
    int m_fd;
#endif

    uint16_t m_ethertype;
    jdksavdecc_eui48 m_my_mac;
    jdksavdecc_eui48 m_default_dest_mac;
    int m_interface_id;
    void *m_pcap;


  public:
    /**
    *  Open a raw socket connected to the specified interface name and join the
    *  specified multicast address
    */
    RawNetIO( uint16_t ethertype, const char *interface_name, const uint8_t join_multicast[6] );

    /**
    * Close a raw socket
    */
    ~RawNetIO();

    void setNonblocking();

    bool joinMulticast( const jdksavdecc_eui48 &multicast_mac );

#if defined( _WIN32 )
    SOCKET getFd() const { return m_fd; }
#else
    int getFd() const { return m_fd; }
#endif

    virtual void initialize();

    virtual const jdksavdecc_eui48 &getMACAddress() const;

    virtual uint16_t receiveRawNet( uint8_t *data, uint16_t max_len );

    virtual bool sendRawNet(
        uint8_t const *data, uint16_t len, uint8_t const *data1, uint16_t len1, uint8_t const *data2, uint16_t len2 );

    virtual bool sendReplyRawNet(
        uint8_t const *data, uint16_t len, uint8_t const *data1, uint16_t len1, uint8_t const *data2, uint16_t len2 );
};
}

#endif
