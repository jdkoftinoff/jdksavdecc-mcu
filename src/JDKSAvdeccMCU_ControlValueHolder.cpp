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
#include "JDKSAvdeccMCU_ControlValueHolder.hpp"

namespace JDKSAvdeccMCU
{

ControlValueHolder::ControlValueHolder( uint8_t value_length, uint8_t *value )
    : m_value_length( value_length ), m_value( value ), m_dirty( true )
{
}

uint8_t ControlValueHolder::getValueOctet( uint16_t item ) const
{
    uint8_t r = 0;
    uint16_t pos = item * sizeof( uint8_t );
    if ( m_value_length > pos )
    {
        r = m_value[pos];
    }
    return r;
}

uint16_t ControlValueHolder::getValueDoublet( uint16_t item ) const
{
    uint16_t r = 0;
    uint16_t pos = item * sizeof( uint16_t );
    if ( m_value_length > pos )
    {
        r = ( ( (uint16_t)m_value[pos + 0] ) << 8 ) + m_value[item + 1];
    }
    return r;
}

uint32_t ControlValueHolder::getValueQuadlet( uint16_t item ) const
{
    uint32_t r = 0;
    uint16_t pos = item * sizeof( uint32_t );
    if ( m_value_length == item * sizeof( uint32_t ) )
    {

        r = ( ( (uint32_t)m_value[pos + 0] ) << 24 )
            + ( ( (uint32_t)m_value[pos + 1] ) << 16 )
            + ( ( (uint32_t)m_value[pos + 2] ) << 8 )
            + ( ( (uint32_t)m_value[pos + 3] ) << 0 );
    }
    return r;
}

uint32_t ControlValueHolder::getValue( uint16_t item ) const
{
    uint32_t r = 0;
    switch ( m_value_length )
    {
    case 1:
        r = getValueOctet( item );
        break;
    case 2:
        r = getValueDoublet( item );
        break;
    case 3:
        r = getValueQuadlet( item );
        break;
    }
    return r;
}

void ControlValueHolder::setValueOctet( uint8_t v, uint16_t item )
{
    if ( m_value_length == 1 && getValueOctet( item ) != v )
    {
        uint16_t pos = item * sizeof( uint8_t );

        m_dirty = true;
        m_value[pos] = v;
    }
}

void ControlValueHolder::setValueDoublet( uint16_t v, uint16_t item )
{
    if ( m_value_length == 2 && getValueDoublet() != v )
    {
        uint16_t pos = item * sizeof( uint16_t );
        m_dirty = true;
        m_value[pos + 0] = ( v >> 8 ) & 0xff;
        m_value[pos + 1] = ( v >> 0 ) & 0xff;
    }
}

void ControlValueHolder::setValueQuadlet( uint32_t v, uint16_t item )
{
    if ( m_value_length == 4 && getValueQuadlet() != v )
    {
        uint16_t pos = item * sizeof( uint32_t );
        m_dirty = true;
        m_value[pos + 0] = ( v >> 24 ) & 0xff;
        m_value[pos + 1] = ( v >> 16 ) & 0xff;
        m_value[pos + 2] = ( v >> 8 ) & 0xff;
        m_value[pos + 3] = ( v >> 0 ) & 0xff;
    }
}

void ControlValueHolder::setValue( uint32_t v, uint16_t item )
{
    switch ( m_value_length )
    {
    case 1:
        setValueOctet( v, item );
        break;
    case 2:
        setValueDoublet( v, item );
        break;
    case 3:
        setValueQuadlet( v, item );
        break;
    }
}

void ControlValueHolder::setValue( uint8_t const *v )
{
    for ( uint8_t i = 0; i < m_value_length; ++i )
    {
        if ( m_value[i] != v[i] )
        {
            m_value[i] = v[i];
            m_dirty = true;
        }
    }
}
}
