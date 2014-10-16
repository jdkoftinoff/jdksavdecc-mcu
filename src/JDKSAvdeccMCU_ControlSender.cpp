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
#include "JDKSAvdeccMCU_ControlSender.hpp"

namespace JDKSAvdeccMCU
{

ControlSender::ControlSender(
    ControllerEntity &controller_entity,
    jdksavdecc_eui64 const &target_entity_id,
    jdksavdecc_eui48 const &target_mac_address,
    uint16_t target_descriptor_index,
    jdksavdecc_timestamp_in_milliseconds update_rate_in_millis,
    ControlValueHolder *holder )
    : m_controller_entity( controller_entity )
    , m_target_entity_id( target_entity_id )
    , m_target_mac_address( target_mac_address )
    , m_target_descriptor_index( target_descriptor_index )
    , m_update_rate_in_millis( update_rate_in_millis )
    , m_last_send_time_in_millis( 0 )
    , m_holder( holder )
{
}

void ControlSender::tick( jdksavdecc_timestamp_in_milliseconds time_in_millis )
{
    if ( m_holder->isDirty() || wasTimeOutHit( time_in_millis,
                                               m_last_send_time_in_millis,
                                               m_update_rate_in_millis ) )
    {
        sendSetControl();
        m_holder->clearDirty();
        m_last_send_time_in_millis = time_in_millis;
    }
}

bool ControlSender::sendSetControl( bool wait_for_ack )
{
    bool r = false;
    // If we are told to wait for an ACK, then we have to wait until we can send
    // a command that can be tracked
    if ( ( !wait_for_ack )
         || ( wait_for_ack && m_controller_entity.canSendCommand() ) )
    {
        FrameWithSize<4> pdufragment;

        pdufragment.putDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
        pdufragment.putDoublet( m_target_descriptor_index );

        getControllerEntity().sendCommand( m_target_entity_id,
                                           m_target_mac_address,
                                           JDKSAVDECC_AEM_COMMAND_SET_CONTROL,
                                           wait_for_ack,
                                           pdufragment.getBuf(),
                                           pdufragment.getLength(),
                                           m_holder->getBuf(),
                                           m_holder->getLength() );
        r = true;
    }
    return r;
}

bool ControlSender::receivedPDU( Frame &frame )
{
    (void)frame;
    return false;
}
}
