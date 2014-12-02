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

namespace JDKSAvdeccMCU
{

class RawSocketMulti : public RawSocket
{
  public:
    RawSocketMulti( uint16_t ethertype, const Eui48 &multicast_to_join )
    {
        RawSocketTracker::openAllEthernetPorts( ethertype, multicast_to_join );
    }

    virtual jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
    {
        return RawSocketTracker::multiGetTimeInMilliseconds();
    }

    virtual bool recvFrame( Frame *frame )
    {
        return RawSocketTracker::multiRecvFrame( frame );
    }

    virtual bool sendFrame( Frame const &frame,
                            uint8_t const *data1 = 0,
                            uint16_t len1 = 0,
                            uint8_t const *data2 = 0,
                            uint16_t len2 = 0 )
    {
        return RawSocketTracker::multiSendFrame(
            frame, data1, len1, data2, len2 );
    }

    virtual bool sendReplyFrame( Frame &frame,
                                 uint8_t const *data1 = 0,
                                 uint16_t len1 = 0,
                                 uint8_t const *data2 = 0,
                                 uint16_t len2 = 0 )
    {
        return RawSocketTracker::multiSendReplyFrame(
            frame, data1, len1, data2, len2 );
    }

    virtual bool joinMulticast( const Eui48 &multicast_mac ) { return false; }

    /**
    * Set the socket to non blocking mode
    */
    virtual void setNonblocking()
    {
        for ( int i = 0; i < RawSocketTracker::num_rawsockets; ++i )
        {
            RawSocketTracker::net[i]->setNonblocking();
        }
    }

    virtual void initialize()
    {
        for ( int i = 0; i < RawSocketTracker::num_rawsockets; ++i )
        {
            RawSocketTracker::net[i]->initialize();
        }
    }

    /**
    * Get the file descriptor
    */
    virtual filedescriptor_type getFd() const { return bad_filedescriptor; }

    /**
     * Get the MAC address of the ethernet port
     */
    virtual Eui48 const &getMACAddress() const
    {
        return RawSocketTracker::net[0]->getMACAddress();
    }

    /**
     * Get the device's name
     */
    virtual const char *getDeviceName() const { return "RawSocketMulti"; }
};
}
