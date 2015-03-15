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

#include "JDKSAvdeccMCU/World.hpp"
#include "jdksavdecc_util.h"
#include "jdksavdecc_print.h"

namespace JDKSAvdeccMCU
{

///
/// \brief The Eui48 class hold an IEEE EUI48 / MAC Address
///
class Eui48 : public jdksavdecc_eui48
{
  public:
    ///
    /// \brief Eui48 constructor
    ///
    /// Initialize all octets to 0xff
    ///
    Eui48() { jdksavdecc_eui48_init( this ); }

    ///
    /// \brief Eui48 Construct with 6 octet values
    /// \param a-f The 6 octets that make up the EUI48
    ///
    Eui48( uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f )
    {
        value[0] = a;
        value[1] = b;
        value[2] = c;
        value[3] = d;
        value[4] = e;
        value[5] = f;
    }

    ///
    /// \brief Eui48 Construct via pointer to 6 octet values
    /// \param p pointer to 6 octets
    ///
    Eui48( uint8_t const *p ) { jdksavdecc_eui48_copy( this, (jdksavdecc_eui48 const *)p ); }

    ///
    /// \brief Eui48 copy constructor from jdksavdecc_eui48 base class
    /// \param other
    ///
    Eui48( jdksavdecc_eui48 const &other ) { jdksavdecc_eui48_copy( this, &other ); }

    ///
    /// \brief Eui48 construct with right justified uint64_t
    /// \param v uint64_t with low 48 bits containing EUI48
    ///
    Eui48( uint64_t v ) { jdksavdecc_eui48_init_from_uint64( this, v ); }

    ///
    /// \brief Eui48 constructor from c string
    ///
    /// Parses in form UU:VV:WW:XX:YY:ZZ and
    /// UU-VV-WW-XX-YY-ZZ
    ///
    /// \param s pointer to C string
    ///
    Eui48( const char *s ) { jdksavdecc_eui48_init_from_cstr( this, s ); }

#if JDKSAVDECCMCU_ENABLE_STRING
    ///
    /// \brief Eui48 constructor from C++ string
    ///
    /// Parses in form UU:VV:WW:XX:YY:ZZ and
    /// UU-VV-WW-XX-YY-ZZ
    ///
    /// \param s std::string reference
    ///
    Eui48( std::string const &s ) { jdksavdecc_eui48_init_from_cstr( this, s.c_str() ); }
#endif

    ///
    /// \brief operator = from jdksavdecc_eui48
    /// \param other reference to jdksavedecc_eui48
    /// \return const reference to self
    ///
    Eui48 const &operator=( jdksavdecc_eui48 const &other )
    {
        jdksavdecc_eui48_copy( this, &other );
        return *this;
    }

    ///
    /// \brief convertToUint64 convert to uint64_t
    /// \return uint64_t with lower 48 bits containing EUI48
    ///
    uint64_t convertToUint64() const { return jdksavdecc_eui48_convert_to_uint64( this ); }

    ///
    /// \brief store Store octets into buffer
    /// \param p destination buffer pointer
    /// \param pos offset from start of buffer
    ///
    void store( uint8_t *p, size_t pos ) const { memcpy( p + pos, &value[0], 6 ); }

    ///
    /// \brief isSet checks if Eui48 has been initialized
    /// \return true if the EUI48 is not FF:FF:FF:FF:FF:FF
    ///
    bool isSet() const { return jdksavdecc_eui48_is_set( *this ) != 0; }

    ///
    /// \brief isNotSet checks if Eui48 is set to FF:FF:FF:FF:FF:FF
    /// \return true if EUI48 is FF:FF:FF:FF:FF:FF
    ///
    bool isNotSet() const { return jdksavdecc_eui48_is_set( *this ) == 0; }

    ///
    /// \brief isZero checks if Eui48 is set to 00:00:00:00:00:00
    /// \return true if EUI48 is 00:00:00:00:00:00
    ///
    bool isZero() const { return jdksavdecc_eui48_is_zero( *this ) != 0; }

    ///
    /// \brief isNotZero checks if Eui48 is not set to 00:00:00:00:00:00
    /// \return true if EUI48 is not 00:00:00:00:00:00
    ///
    bool isNotZero() const { return jdksavdecc_eui48_is_not_zero( *this ) != 0; }

    ///
    /// \brief compare Numeric compare with other Eui48
    /// \param other jdksavdecc_eui48 reference to compare to
    /// \return -1 if less than other, 0 if equal to other, 1 if greater than
    /// other
    ///
    int compare( const jdksavdecc_eui48 &other ) const { return jdksavdecc_eui48_compare( this, &other ); }

    void clear() { jdksavdecc_eui48_init( this ); }
};

///
/// \brief The Eui64 class hold an IEEE EUI64
///
class Eui64 : public jdksavdecc_eui64
{
  public:
    ///
    /// \brief Eui64 constructor
    ///
    /// Initialize all octets to 0xff
    ///
    Eui64() { jdksavdecc_eui64_init( this ); }

    ///
    /// \brief Eui48 Construct with 8 octet values
    /// \param a-h The 8 octets that make up the EUI48
    ///
    Eui64( uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h )
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

    ///
    /// \brief Eui64 Construct via pointer to 8 octet values
    /// \param p pointer to 8 octets
    ///
    Eui64( uint8_t const *p ) { jdksavdecc_eui64_copy( this, (jdksavdecc_eui64 const *)p ); }

    ///
    /// \brief Eui64 copy constructor from jdksavdecc_eui64 base class
    /// \param other
    ///
    Eui64( jdksavdecc_eui64 const &other ) { jdksavdecc_eui64_copy( this, &other ); }

    ///
    /// \brief Eui48 construct with right justified uint64_t
    /// \param v uint64_t containing EUI48
    ///
    Eui64( uint64_t v ) { jdksavdecc_eui64_init_from_uint64( this, v ); }

    ///
    /// \brief Eui48 constructor from C string
    ///
    /// Parses in form SS:TT:UU:VV:WW:XX:YY:ZZ and
    /// SS-TT-UU-VV-WW-XX-YY-ZZ
    ///
    /// \param s pointer to C string
    ///
    Eui64( const char *s ) { jdksavdecc_eui64_init_from_cstr( this, s ); }

#if JDKSAVDECCMCU_ENABLE_STRING

    ///
    /// \brief Eui48 constructor from C++ string
    ///
    /// Parses in form SS:TT:UU:VV:WW:XX:YY:ZZ and
    /// SS-TT-UU-VV-WW-XX-YY-ZZ
    ///
    /// \param s std::string reference
    ///
    Eui64( std::string const &s ) { jdksavdecc_eui64_init_from_cstr( this, s.c_str() ); }
#endif

    ///
    /// \brief operator = from jdksavdecc_eui64
    /// \param other reference to jdksavedecc_eui64
    /// \return const reference to self
    ///
    Eui64 const &operator=( Eui64 const &other )
    {
        jdksavdecc_eui64_copy( this, &other );
        return *this;
    }

    ///
    /// \brief convertToUint64 convert to uint64_t
    /// \return uint64_t containing EUI48
    ///
    uint64_t convertToUint64() const { return jdksavdecc_eui64_convert_to_uint64( this ); }

    ///
    /// \brief store Store octets into buffer
    /// \param p destination buffer pointer
    /// \param pos offset from start of buffer
    ///
    void store( uint8_t *p, size_t pos ) const { memcpy( p + pos, &value[0], 8 ); }

    ///
    /// \brief isSet checks is Eui48 has been initialized
    /// \return true if the EUI64 is not FF:FF:FF:FF:FF:FF:FF:FF
    ///
    bool isSet() const { return jdksavdecc_eui64_is_set( *this ) != 0; }

    ///
    /// \brief isSet checks is Eui64 has not been initialized
    /// \return true if the EUI64 is FF:FF:FF:FF:FF:FF:FF:FF
    ///
    bool isUnset() const { return jdksavdecc_eui64_is_unset( *this ) != 0; }

    ///
    /// \brief isSet checks is Eui48 is 0
    /// \return true if the EUI64 is 00:00:00:00:00:00:00:00
    ///
    bool isZero() const { return jdksavdecc_eui64_is_zero( *this ) != 0; }

    ///
    /// \brief compare Numeric compare with other Eui64
    /// \param other jdksavdecc_eui64 reference to compare to
    /// \return -1 if less than other, 0 if equal to other, 1 if greater than
    /// other
    ///
    int compare( const Eui64 &other ) const { return jdksavdecc_eui64_compare( this, &other ); }

    void clear() { jdksavdecc_eui64_init( this ); }
};
}

inline bool isSet( jdksavdecc_eui48 const &v ) { return jdksavdecc_eui48_is_set( v ) != 0; }

inline bool isUnset( jdksavdecc_eui48 const &v ) { return jdksavdecc_eui48_is_unset( v ) != 0; }

inline bool isZero( jdksavdecc_eui48 const &v ) { return jdksavdecc_eui48_is_zero( v ) != 0; }

inline bool isSet( jdksavdecc_eui64 const &v ) { return jdksavdecc_eui64_is_set( v ) != 0; }

inline bool isUnset( jdksavdecc_eui64 const &v ) { return jdksavdecc_eui64_is_unset( v ) != 0; }

inline bool isZero( jdksavdecc_eui64 const &v ) { return jdksavdecc_eui64_is_zero( v ) != 0; }

inline bool operator<( const jdksavdecc_eui48 &lhs, const jdksavdecc_eui48 &rhs )
{
    return jdksavdecc_eui48_compare( &lhs, &rhs ) < 0;
}

inline bool operator<=( const jdksavdecc_eui48 &lhs, const jdksavdecc_eui48 &rhs )
{
    return jdksavdecc_eui48_compare( &lhs, &rhs ) <= 0;
}

inline bool operator==( const jdksavdecc_eui48 &lhs, const jdksavdecc_eui48 &rhs )
{
    return jdksavdecc_eui48_compare( &lhs, &rhs ) == 0;
}

inline bool operator>=( const jdksavdecc_eui48 &lhs, const jdksavdecc_eui48 &rhs )
{
    return jdksavdecc_eui48_compare( &lhs, &rhs ) >= 0;
}

inline bool operator>( const jdksavdecc_eui48 &lhs, const jdksavdecc_eui48 &rhs )
{
    return jdksavdecc_eui48_compare( &lhs, &rhs ) > 0;
}

inline bool operator!=( const jdksavdecc_eui48 &lhs, const jdksavdecc_eui48 &rhs )
{
    return jdksavdecc_eui48_compare( &lhs, &rhs ) != 0;
}

inline bool operator<( const jdksavdecc_eui64 &lhs, const jdksavdecc_eui64 &rhs )
{
    return jdksavdecc_eui64_compare( &lhs, &rhs ) < 0;
}

inline bool operator<=( const jdksavdecc_eui64 &lhs, const jdksavdecc_eui64 &rhs )
{
    return jdksavdecc_eui64_compare( &lhs, &rhs ) <= 0;
}

inline bool operator==( const jdksavdecc_eui64 &lhs, const jdksavdecc_eui64 &rhs )
{
    return jdksavdecc_eui64_compare( &lhs, &rhs ) == 0;
}

inline bool operator>=( const jdksavdecc_eui64 &lhs, const jdksavdecc_eui64 &rhs )
{
    return jdksavdecc_eui64_compare( &lhs, &rhs ) >= 0;
}

inline bool operator>( const jdksavdecc_eui64 &lhs, const jdksavdecc_eui64 &rhs )
{
    return jdksavdecc_eui64_compare( &lhs, &rhs ) > 0;
}

inline bool operator!=( const jdksavdecc_eui64 &lhs, const jdksavdecc_eui64 &rhs )
{
    return jdksavdecc_eui64_compare( &lhs, &rhs ) != 0;
}

#if JDKSAVDECCMCU_ENABLE_IOSTREAM

inline std::ostream &operator<<( std::ostream &o, jdksavdecc_eui48 const &v )
{
    jdksavdecc_printer p;
    char buf[128];
    jdksavdecc_printer_init( &p, buf, sizeof( buf ) );
    jdksavdecc_printer_print_eui48( &p, v );
    o << buf;
    return o;
}

inline std::ostream &operator<<( std::ostream &o, jdksavdecc_eui64 const &v )
{
    jdksavdecc_printer p;
    char buf[128];
    jdksavdecc_printer_init( &p, buf, sizeof( buf ) );
    jdksavdecc_printer_print_eui64( &p, v );
    o << buf;
    return o;
}

#endif
