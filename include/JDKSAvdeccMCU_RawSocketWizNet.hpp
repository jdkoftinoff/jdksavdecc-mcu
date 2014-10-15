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

#include "JDKSAvdeccMCU_RawSocket.hpp"

#define JDKSAVDECCWINNETIO_DEBUG 0

#if JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET
namespace JDKSAvdeccMCU
{

class RawSocketWizNet : public RawSocket
{
  public:
    RawSocketWizNet( jdksavdecc_eui48 const &mac_address,
                     uint16_t ethertype,
                     const jdksavdecc_eui48 *multicast_to_join = 0 )
        : m_mac_address( mac_address ), m_ethertype( ethertype )
    {
        if ( multicast_to_join )
        {
            m_multicast = *multicast_to_join;
        }
        else
        {
            jdksavdecc_eui48_init( &m_multicast );
        }
    }

    virtual ~RawSocketWizNet();

    virtual jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
    {
        return millis();
    }

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

    virtual void initialize();

    virtual bool joinMulticast( const jdksavdecc_eui48 &multicast_mac )
    {
        m_multicast = multicast_mac;
    }

    virtual void setNonblocking() {}

    virtual filedescriptor_t getFd() const { return 0; }

    virtual jdksavdecc_eui48 const &getMACAddress() const
    {
        return m_mac_address;
    }

  private:
    jdksavdecc_eui48 m_mac_address;
    uint16_t m_ethertype;
    jdksavdecc_eui48 m_multicast;
};
}

#endif
