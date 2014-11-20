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

#include "jdksavdecc_util.h"

namespace JDKSAvdeccMCU
{

class Eui48 : public jdksavdecc_eui48
{
  public:
    Eui48() { jdksavdecc_eui48_init( this ); }

    Eui48( uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f )
    {
        value[0] = a;
        value[1] = b;
        value[2] = c;
        value[3] = d;
        value[4] = e;
        value[5] = f;
    }

    Eui48( jdksavdecc_eui48 const &other )
    {
        jdksavdecc_eui48_copy( this, &other );
    }

    Eui48( uint64_t v ) { jdksavdecc_eui48_init_from_uint64( this, v ); }

    Eui48( const char *s ) { jdksavdecc_eui48_init_from_cstr( this, s ); }

    Eui48( std::string const &s )
    {
        jdksavdecc_eui48_init_from_cstr( this, s.c_str() );
    }

    Eui48 const &operator=( Eui48 const *other )
    {
        jdksavdecc_eui48_copy( this, other );
        return *this;
    }

    bool isSet() const { return jdksavdecc_eui48_is_set( *this ); }
};

class Eui64 : public jdksavdecc_eui64
{
  public:
    Eui64() { jdksavdecc_eui64_init( this ); }

    Eui64( uint8_t a,
           uint8_t b,
           uint8_t c,
           uint8_t d,
           uint8_t e,
           uint8_t f,
           uint8_t g,
           uint8_t h )
    {
        value[0] = a;
        value[1] = b;
        value[2] = c;
        value[3] = d;
        value[4] = e;
        value[5] = f;
        value[6] = g;
        value[7] = h;
    }

    Eui64( jdksavdecc_eui64 const &other )
    {
        jdksavdecc_eui64_copy( this, &other );
    }

    Eui64( uint64_t v ) { jdksavdecc_eui64_init_from_uint64( this, v ); }

    Eui64( const char *s ) { jdksavdecc_eui64_init_from_cstr( this, s ); }

    Eui64( std::string const &s )
    {
        jdksavdecc_eui64_init_from_cstr( this, s.c_str() );
    }

    Eui64 const &operator=( Eui64 const *other )
    {
        jdksavdecc_eui64_copy( this, other );
        return *this;
    }

    bool isSet() const { return jdksavdecc_eui64_is_set( *this ); }
};

inline void Eui48_set( Eui48 const &v, void *buf, size_t pos )
{
    jdksavdecc_eui48_set( v, buf, pos );
}

inline Eui48 Eui48_get( void *buf, size_t pos )
{
    Eui48 v( jdksavdecc_eui48_get( buf, pos ) );
    return v;
}

inline void Eui64_set( Eui64 const &v, void *buf, size_t pos )
{
    jdksavdecc_eui64_set( v, buf, pos );
}

inline Eui64 Eui64_get( void *buf, size_t pos )
{
    Eui64 v( jdksavdecc_eui64_get( buf, pos ) );
    return v;
}

inline int Eui48_compare( Eui48 const &lhs, Eui48 const &rhs )
{
    return jdksavdecc_eui48_compare( &lhs, &rhs );
}

inline int Eui64_compare( Eui64 const &lhs, Eui64 const &rhs )
{
    return jdksavdecc_eui64_compare( &lhs, &rhs );
}

inline bool Eui48_is_set( Eui48 const &v )
{
    return jdksavdecc_eui48_is_set( v );
}

inline bool Eui48_is_unset( Eui48 const &v )
{
    return jdksavdecc_eui48_is_unset( v );
}

inline bool Eui64_is_set( Eui64 const &v )
{
    return jdksavdecc_eui64_is_set( v );
}

inline bool Eui64_is_unset( Eui64 const &v )
{
    return jdksavdecc_eui64_is_unset( v );
}

inline bool Eui64_is_zero( Eui64 const &v )
{
    return jdksavdecc_eui64_is_zero( v );
}
}
