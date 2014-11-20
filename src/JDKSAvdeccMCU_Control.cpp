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
#include "JDKSAvdeccMCU_Control.hpp"

namespace JDKSAvdeccMCU
{

Control::Control( Entity &entity,
                  uint16_t descriptor_index,
                  Eui64 control_type,
                  uint16_t control_value_type,
                  ControlValueHolder *holder )
    : m_entity( entity )
    , m_descriptor_index( descriptor_index )
    , m_control_type( control_type )
    , m_control_value_type( control_value_type )
    , m_holder( holder )
{
}

void Control::tick( jdksavdecc_timestamp_in_milliseconds time_in_millis )
{
    (void)time_in_millis;
}

bool Control::receivedPDU( Frame &frame )
{
    // TODO: parse incoming SET_CONTROL and GET_CONTROL command and response
    (void)frame;
    return false;
}

uint8_t Control::formControlValueMetaData( Frame &pdu )
{
    uint8_t status = JDKSAVDECC_AEM_STATUS_ENTITY_MISBEHAVING;
    // TODO: Fill in meta data for the control descriptor
    (void)pdu;

    switch ( m_control_value_type )
    {
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT8:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT8:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT16:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT16:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT32:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT32:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT64:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT64:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_FLOAT:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_DOUBLE:
        break;

    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT8:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT8:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT16:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT16:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT32:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT32:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT64:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT64:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_FLOAT:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_DOUBLE:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_STRING:
        break;

    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT8:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT8:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT16:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT16:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT32:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT32:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT64:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT64:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_FLOAT:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_DOUBLE:
        break;

    case JDKSAVDECC_CONTROL_VALUE_UTF8:
    case JDKSAVDECC_CONTROL_VALUE_BODE_PLOT:
    case JDKSAVDECC_CONTROL_VALUE_SMPTE_TIME:
    case JDKSAVDECC_CONTROL_VALUE_SAMPLE_RATE:
    case JDKSAVDECC_CONTROL_VALUE_GPTP_TIME:
        break;
    case JDKSAVDECC_CONTROL_VALUE_VENDOR:
        break;
    case JDKSAVDECC_CONTROL_VALUE_EXPANSION:
        break;
    default:
        break;
    }
    return status;
}

uint8_t Control::formControlPayload( Frame &pdu )
{
    pdu.putBuf( getControlValueHolder()->getBuf(),
                getControlValueHolder()->getLength() );
    return JDKSAVDECC_AECP_STATUS_SUCCESS;
}

uint8_t Control::validateSetControlCommand( Frame &pdu )
{
    uint8_t status = JDKSAVDECC_AEM_STATUS_ENTITY_MISBEHAVING;

    // TODO: validate the value in a set control command
    (void)pdu;

    switch ( m_control_value_type )
    {
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT8:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT8:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT16:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT16:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT32:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT32:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_INT64:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_UINT64:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_FLOAT:
    case JDKSAVDECC_CONTROL_VALUE_LINEAR_DOUBLE:
        break;

    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT8:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT8:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT16:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT16:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT32:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT32:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_INT64:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_UINT64:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_FLOAT:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_DOUBLE:
    case JDKSAVDECC_CONTROL_VALUE_SELECTOR_STRING:
        break;

    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT8:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT8:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT16:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT16:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT32:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT32:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_INT64:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_UINT64:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_FLOAT:
    case JDKSAVDECC_CONTROL_VALUE_ARRAY_DOUBLE:
        break;

    case JDKSAVDECC_CONTROL_VALUE_UTF8:
    case JDKSAVDECC_CONTROL_VALUE_BODE_PLOT:
    case JDKSAVDECC_CONTROL_VALUE_SMPTE_TIME:
    case JDKSAVDECC_CONTROL_VALUE_SAMPLE_RATE:
    case JDKSAVDECC_CONTROL_VALUE_GPTP_TIME:
        break;
    case JDKSAVDECC_CONTROL_VALUE_VENDOR:
        break;
    case JDKSAVDECC_CONTROL_VALUE_EXPANSION:
        break;
    default:
        break;
    }
    return status;
}

uint8_t Control::readControlDescriptor( Frame &pdu )
{
    uint8_t status = JDKSAVDECC_AEM_STATUS_ENTITY_MISBEHAVING;

    // TODO: fill in control descriptor contents for READ_DESCRIPTOR
    (void)pdu;
    return status;
}
}
