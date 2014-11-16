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

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_ControlIdentify.hpp"

namespace JDKSAvdeccMCU
{

ControlIdentify::ControlIdentify(
    ControllerEntity &controller_entity,
    uint16_t descriptor_index,
    ControlValueHolder *holder,
    void ( *received_wink_callback )( uint16_t descriptor_index,
                                      uint8_t value ) )
    : Control( controller_entity,
               descriptor_index,
               eui64FromUint64( JDKSAVDECC_AEM_CONTROL_TYPE_IDENTIFY ),
               JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT8,
               holder )
    , m_send_countdown( 0 )
    , m_time_of_last_sent_unsolicited_msg( 0 )
    , m_received_wink_callback( received_wink_callback )
{
}

void
    ControlIdentify::tick( jdksavdecc_timestamp_in_milliseconds time_in_millis )
{
    // TODO: send 3 unsolicited messages when value changes state from 0 to 0xff
    (void)time_in_millis;
}
}
