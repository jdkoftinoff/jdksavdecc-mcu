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
#include "JDKSAvdeccMCU_FixedBuffer.hpp"
#include "JDKSAvdeccMCU_Frame.hpp"
#include "jdksavdecc_app.h"

namespace JDKSAvdeccMCU
{

struct AppMessage
{
    AppMessage();

    AppMessage( AppMessage const &other );

    AppMessage const &operator=( AppMessage const &other );

    void clear() { setNOP(); }

    void setNOP();

    void setEntityIdRequest( Eui48 const &apc_primary_mac,
                             Eui64 const &requested_entity_id );

    void setEntityIdResponse( Eui48 const &apc_primary_mac,
                              Eui64 const &requested_entity_id );

    void setLinkUp( Eui48 const &network_port_mac );

    void setLinkDown( Eui48 const &network_port_mac );

    void setAvdeccFromAps( Frame const &frame );

    void setAvdeccFromApc( Frame const &frame );

    void setVendor( Eui48 const &vendor_message_type,
                    FixedBuffer const &payload );

    jdksavdecc_fullappdu m_appdu;
};

class AppMessageParser
{
  public:
    static const int max_appdu_message_size
        = JDKSAVDECC_APPDU_HEADER_LEN + JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH;

    AppMessageParser( FixedBuffer *state );

    void clear() { m_state->setLength( 0 ); }

    ssize_t parse( AppMessage *destination_msg, uint8_t octet );

  protected:
    FixedBuffer *m_state;
};

class AppMessageParserWithStorage : public AppMessageParser
{
  public:
    AppMessageParserWithStorage() : AppMessageParser( &m_state_storage ) {}

  protected:
    FixedBufferWithSize<max_appdu_message_size> m_state_storage;
};
}
