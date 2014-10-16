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
#include "JDKSAvdeccMCU_Frame.hpp"

#ifndef JDKSAVDECCMCU_MAX_RAWSOCKETS
#define JDKSAVDECCMCU_MAX_RAWSOCKETS ( 1 )
#endif

namespace JDKSAvdeccMCU
{
class RawSocket
{
  public:
#if defined( _WIN32 )
    typedef SOCKET filedescriptor_type;
    static const filedescriptor_type bad_filedescriptor = INVALID_HANDLE;
#else
    typedef int filedescriptor_type;
    static const filedescriptor_type bad_filedescriptor = -1;
#endif

    RawSocket() { net[num_rawsockets++] = this; }

    virtual ~RawSocket() {}

    virtual jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds() = 0;

    virtual bool recvFrame( Frame *frame ) = 0;

    virtual bool sendFrame( Frame const &frame,
                            uint8_t const *data1 = 0,
                            uint16_t len1 = 0,
                            uint8_t const *data2 = 0,
                            uint16_t len2 = 0 ) = 0;

    virtual bool sendReplyFrame( Frame &frame,
                                 uint8_t const *data1 = 0,
                                 uint16_t len1 = 0,
                                 uint8_t const *data2 = 0,
                                 uint16_t len2 = 0 ) = 0;

    /**
    * Attempt to join an additional multicast mac address group
    */
    virtual bool joinMulticast( const jdksavdecc_eui48 &multicast_mac ) = 0;

    /**
    * Set the socket to non blocking mode
    */
    virtual void setNonblocking() = 0;

    virtual void initialize() {}

    /**
    * Get the file descriptor
    */
    virtual filedescriptor_type getFd() const = 0;

    /**
     * Get the MAC address of the ethernet port
     */
    virtual jdksavdecc_eui48 const &getMACAddress() const = 0;

    /**
     * Ask the first ethernet port for the current time in milliseconds
     */
    static jdksavdecc_timestamp_in_milliseconds multiGetTimeInMilliseconds()
    {
        return net[0]->getTimeInMilliseconds();
    }

    /**
     * @brief multiRecvFrame Poll all ethernet ports for a received ethernet
     * frame
     * @param frame pointer to the frame
     * @return bool true if the frame was received
     */
    static bool multiRecvFrame( Frame *frame );

    static bool multiSendFrame( Frame const &frame,
                                uint8_t const *data1 = 0,
                                uint16_t len1 = 0,
                                uint8_t const *data2 = 0,
                                uint16_t len2 = 0 );

    static bool multiSendReplyFrame( Frame &frame,
                                     uint8_t const *data1 = 0,
                                     uint16_t len1 = 0,
                                     uint8_t const *data2 = 0,
                                     uint16_t len2 = 0 );

  protected:
    static RawSocket *net[JDKSAVDECCMCU_MAX_RAWSOCKETS];
    static uint16_t num_rawsockets;
    static uint16_t last_recv_socket;
};
}
