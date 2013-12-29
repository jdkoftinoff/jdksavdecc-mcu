#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccControlValueHolder.h"

namespace JDKSAvdecc {

ControlValueHolder::ControlValueHolder( uint8_t value_length )
: m_value_length(value_length)
, m_dirty( true ) {
    m_value[0] = 0;
    m_value[1] = 0;
    m_value[2] = 0;
    m_value[3] = 0;
}

uint8_t ControlValueHolder::GetValueOctet() const {
    uint8_t r=0;
    if( m_value_length==1 ) {
        r=m_value[0];
    }
    return r;
}

uint16_t ControlValueHolder::GetValueDoublet() const {
    uint16_t r=0;
    if( m_value_length==2 ) {
        r=(((uint16_t)m_value[0])<<8) + m_value[1];
    }
    return r;
}

uint32_t ControlValueHolder::GetValueQuadlet() const {
    uint32_t r=0;
    if( m_value_length==4 ) {
        r=(((uint32_t)m_value[0])<<24)
        + (((uint32_t)m_value[1])<<16)
        + (((uint32_t)m_value[2])<<8)
        + (((uint32_t)m_value[3])<<0);
    }
    return r;

}

uint32_t ControlValueHolder::GetValue() const {
    uint32_t r=0;
    switch( m_value_length ) {
        case 1:
            r=GetValueOctet();
            break;
        case 2:
            r=GetValueDoublet();
            break;
        case 3:
            r=GetValueQuadlet();
            break;
    }
    return r;
}


void ControlValueHolder::SetValueOctet( uint8_t v ) {
    if( m_value_length==1 && GetValueOctet() != v ) {
        m_dirty=true;
        m_value[0] = v;
    }
}

void ControlValueHolder::SetValueDoublet( uint16_t v ) {
    if( m_value_length==2 && GetValueDoublet() != v ) {
        m_dirty=true;
        m_value[0] = (v>>8)&0xff;
        m_value[1] = (v>>0)&0xff;
    }
}

void ControlValueHolder::SetValueQuadlet( uint32_t v ) {
    if( m_value_length==4 && GetValueQuadlet() != v ) {
        m_dirty=true;
        m_value[0] = (v>>24)&0xff;
        m_value[1] = (v>>16)&0xff;
        m_value[2] = (v>>8)&0xff;
        m_value[3] = (v>>0)&0xff;
    }
}

void ControlValueHolder::SetValue( uint32_t v) {
    switch( m_value_length ) {
        case 1:
            SetValueOctet( v );
            break;
        case 2:
            SetValueDoublet( v );
            break;
        case 3:
            SetValueQuadlet( v );
            break;
    }
}

void ControlValueHolder::SetValue( uint8_t const *v ) {
    for( uint8_t i=0; i<m_value_length; ++i ) {
        if( m_value[i] != v[i] ) {
            m_value[i] = v[i];
            m_dirty=true;
        }
    }
}

    
}
