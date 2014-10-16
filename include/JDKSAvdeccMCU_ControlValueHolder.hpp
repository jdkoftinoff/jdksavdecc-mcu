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
#include "JDKSAvdeccMCU_Frame.hpp"
#include "JDKSAvdeccMCU_FixedBuffer.hpp"
#include "JDKSAvdeccMCU_Handler.hpp"

namespace JDKSAvdeccMCU
{

class ControlValueHolder : public FixedBuffer
{
  public:
    ControlValueHolder( uint8_t *buf,
                        uint16_t value_length,
                        uint8_t num_items = 1 )
        : FixedBuffer( buf, value_length * num_items )
        , m_value_length( value_length )
        , m_num_items( num_items )
        , m_dirty( true )
    {
        memset( buf, 0, m_value_length * m_num_items );
        setLength( m_value_length * m_num_items );
    }

    bool isDirty() const { return m_dirty; }
    void clearDirty() { m_dirty = false; }

    uint8_t getValueLength() const { return m_value_length; }

    uint8_t getNumItems() const { return m_num_items; }

    uint8_t getValueOctet( uint8_t item = 0 ) const
    {
        return getOctet( item * sizeof( uint8_t ) );
    }

    uint16_t getValueDoublet( uint8_t item = 0 ) const
    {
        return getDoublet( item * sizeof( uint16_t ) );
    }

    uint32_t getValueQuadlet( uint8_t item = 0 ) const
    {
        return getQuadlet( item * sizeof( uint32_t ) );
    }

    template <typename T>
    T getValueAs( uint8_t item = 0 ) const
    {
        T r = 0;
        switch ( m_value_length )
        {
        case 1:
            r = reinterpret_cast<T>( getValueOctet( item ) );
            break;
        case 2:
            r = reinterpret_cast<T>( getValueDoublet( item ) );
            break;
        case 4:
            r = reinterpret_cast<T>( getValueQuadlet( item ) );
            break;
        }
        return r;
    }

    void setValueOctet( uint8_t v, uint8_t item = 0 )
    {
        if ( getOctet( item * sizeof( uint8_t ) ) != v )
        {
            setOctet( v, item * sizeof( uint8_t ) );
            m_dirty = true;
        }
    }

    void setValueDoublet( uint16_t v, uint8_t item = 0 )
    {
        if ( getDoublet( item * sizeof( uint16_t ) ) != v )
        {
            setDoublet( v, item * sizeof( uint16_t ) );
            m_dirty = true;
        }
    }

    void setValueQuadlet( uint32_t v, uint8_t item = 0 )
    {
        if ( getQuadlet( item * sizeof( uint32_t ) ) != v )
        {
            setQuadlet( v, item * sizeof( uint32_t ) );
            m_dirty = true;
        }
    }

    template <typename T>
    void setValueAs( T v, uint8_t item = 0 )
    {
        switch ( m_value_length )
        {
        case 1:
        {
            uint8_t octet_value = reinterpret_cast<uint8_t>( v );
            setValueOctet( octet_value, item );
            break;
        }
        case 2:
        {
            uint16_t doublet_value = reinterpret_cast<uint16_t>( v );
            setValueDoublet( doublet_value, item );
            break;
        }
        case 4:
        {
            uint32_t quadlet_value = reinterpret_cast<uint32_t>( v );
            setValueQuadlet( quadlet_value, item );
            break;
        }
        }
    }

#if JDKSAVDECCMCU_ENABLE_FLOAT
    float getValueFloat( uint8_t item = 0 ) const
    {
        uint32_t q = getValueQuadlet( item );
        float *fq = reinterpret_cast<float *>( &q );
        return *fq;
    }

    void setValueFloat( float v, uint8_t item = 0 )
    {
        uint32_t *qv = reinterpret_cast<uint32_t *>( &v );
        setValueQuadlet( *qv, item );
    }
#endif

    void setValue( void const *v )
    {
        if ( memcmp( m_buf, v, m_value_length ) != 0 )
        {
            memcpy( m_buf, v, m_value_length * m_num_items );
            m_dirty = true;
        }
    }

  protected:
    uint16_t m_value_length;
    uint8_t m_num_items;
    bool m_dirty;
};

template <typename BaseValueType, uint8_t NumItems>
class ControlValueHolderWithStorage : public ControlValueHolder
{
  public:
    typedef BaseValueType value_type;
    static const uint8_t num_items = NumItems;

    ControlValueHolderWithStorage()
        : ControlValueHolder(
              m_value_storage, sizeof( BaseValueType ), NumItems )
    {
    }

  protected:
    uint8_t m_value_storage[sizeof( BaseValueType ) * NumItems];
};
}
