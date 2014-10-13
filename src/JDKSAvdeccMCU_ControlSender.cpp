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
    uint16_t value_length,
    uint8_t *value_storage )
    : m_controller_entity( controller_entity )
    , m_target_entity_id( target_entity_id )
    , m_target_mac_address( target_mac_address )
    , m_target_descriptor_index( target_descriptor_index )
    , m_update_rate_in_millis( update_rate_in_millis )
    , m_last_send_time_in_millis( 0 )
    , m_value_length( value_length )
    , m_value( value_storage )
{
}

void ControlSender::setValueOctet( uint8_t val )
{
    if ( val != m_value[0] )
    {
        m_last_send_time_in_millis -= m_update_rate_in_millis;
        m_value[0] = val;
    }
}

/// Set a doublet value. If it actually changed, then force Tick to send ASAP
void ControlSender::setValueDoublet( uint16_t val )
{
    if ( ( ( ( val >> 8 ) & 0xff ) != m_value[0] )
         || ( ( ( val >> 0 ) & 0xff ) != m_value[1] ) )
    {
        m_last_send_time_in_millis -= m_update_rate_in_millis;
        m_value[0] = ( val >> 8 ) & 0xff;
        m_value[1] = ( val >> 0 ) & 0xff;
    }
}

/// Set a quadlet value. If it actually changed, then force Tick to send ASAP
void ControlSender::setValueQuadlet( uint32_t val )
{
    if ( ( ( ( val >> 24 ) & 0xff ) != m_value[0] )
         || ( ( ( val >> 16 ) & 0xff ) != m_value[1] )
         || ( ( ( val >> 8 ) & 0xff ) != m_value[2] )
         || ( ( ( val >> 0 ) & 0xff ) != m_value[3] ) )
    {

        m_last_send_time_in_millis -= m_update_rate_in_millis;
        m_value[0] = ( val >> 24 ) & 0xff;
        m_value[1] = ( val >> 16 ) & 0xff;
        m_value[2] = ( val >> 8 ) & 0xff;
        m_value[3] = ( val >> 0 ) & 0xff;
    }
}

void ControlSender::tick()
{
    jdksavdecc_timestamp_in_milliseconds time_in_millis
        = getRawSocket().getTimeInMilliseconds();
    if ( wasTimeOutHit( time_in_millis,
                        m_last_send_time_in_millis,
                        m_update_rate_in_millis ) )
    {
        sendSetControl();
        m_last_send_time_in_millis = time_in_millis;
    }
}

void ControlSender::sendSetControl()
{
    bool wait_for_ack=false;
    if( m_controller_entity.CanSendCommand() )
    {
        FrameWithSize<16> pdufragment;

        pdufragment.putDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
        pdufragment.putDoublet( m_target_descriptor_index );
        pdufragment.putBuf( m_value, m_value_length );

        getControllerEntity().sendCommand(
                    m_target_entity_id,
                    m_target_mac_address,
                    JDKSAVDECC_AEM_COMMAND_SET_CONTROL,
                    wait_for_ack,
                    m_value,
                    m_value_length);

    }
}

bool ControlSender::receivedPDU( Frame &frame )
{
    (void)frame;
    return false;
}
}
