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

#if defined( __linux__ ) && JDKSAVDECCMCU_ENABLE_RAWSOCKETLINUX
namespace JDKSAvdeccMCU
{
class RawSocketLinux : public RawSocket
{
  public:
    RawSocketLinux( const char *device,
                    uint16_t ethertype,
                    const jdksavdecc_eui48 *multicast_to_join = 0 );

    virtual ~RawSocketLinux();

    virtual jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
    {
        return JDKSAvdeccMCU::getTimeInMilliseconds();
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

    virtual bool joinMulticast( const jdksavdecc_eui48 &multicast_mac );

    virtual void setNonblocking();

    virtual filedescriptor_type getFd() const { return m_fd; }

    virtual jdksavdecc_eui48 const &getMACAddress() const
    {
        return m_mac_address;
    }

    virtual const char *getDeviceName() const { return m_device; }

    virtual void initialize();

  private:
    filedescriptor_type m_fd;
    const char *m_device;
    int m_interface_id;
    jdksavdecc_eui48 m_mac_address;
    jdksavdecc_eui48 m_default_dest_mac_address;
    uint16_t m_ethertype;
};
}
#endif
