/*
  Copyright (c) 2015, J.D. Koftinoff Software, Ltd.
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
#include "JDKSAvdeccMCU/Helpers.hpp"
#include "JDKSAvdeccMCU/Frame.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief powers_of_ten
///
/// The powers of ten values as float from 1e-12 to 1e12 inclusive
///
extern float powers_of_ten[25];

const char *getAvdeccUnitsSuffix( uint8_t units_code );

///
/// \brief integer_pow10
///
/// Template function to get the value of an integer power of ten
/// Works with integer types and floating point types.
///
template <typename T1, typename T2>
T1 integer_pow10( T2 exponent )
{
    T1 r = 0;
    int e = int( exponent ) + 12;
    if ( e >= 0 && e <= 25 )
    {
        r = powers_of_ten[e];
    }
    return r;
}

/// \brief the RangedValue class
///
/// A holder for a limited range value with options for defaults and inc/dec
/// step size
/// and with the capability to encode/decode into a fixed point integer type
///
template <uint8_t UnitsValue,
          int64_t MinValue,
          int64_t MaxValue,
          int64_t DefaultValue = 0,
          int64_t StepValue = 1,
          int MultiplierPowerValue = 0,
          typename ValueType = float>
class RangedValue
{
#if __cplusplus >= 201103L
    static_assert( MinValue <= DefaultValue, "MinValue is not less than or equal to DefaultValue" );
    static_assert( DefaultValue <= MaxValue, "DefaultValue is not less than or equal to MaxValue" );
#endif

  public:
    typedef ValueType value_type;
    static const uint8_t units = UnitsValue;
    static const int64_t min_value = MinValue;
    static const int64_t max_value = MaxValue;
    static const int64_t step_value = StepValue;
    static const int64_t default_value = DefaultValue;
    static const int multiplier_power = MultiplierPowerValue;

    ///
    /// \brief Value Constructor
    ///
    /// Initialize to the default value
    ///
    RangedValue()
    {
        setDefault();
        setChanged();
    }

    ///
    /// \brief Value implicit Constructor
    ///
    /// Initialize based on value. May throw range_error if the value is out of
    /// range
    ///
    /// \param v value
    ///
    RangedValue( value_type v )
    {
        setValue( v );
        setChanged();
    }

    ///
    /// \brief Value
    ///
    /// Copy constructor is faster since the m_value is already validated
    ///
    /// \param v Source Value object
    ///
    RangedValue( RangedValue const &v )
        : m_value( v.m_value )
        , m_changed( v.m_changed )
    {
    }

    ///
    /// \brief operator =
    ///
    /// Assignment operator is faster since the m_value is already validated
    ///
    /// \param v Source Value object
    /// \return  *this
    ///
    RangedValue &operator=( RangedValue const &v )
    {
        m_value = v.m_value;
        m_changed = v.m_changed;
        return *this;
    }

    ///
    /// \brief operator value_type
    ///
    operator value_type() const { return m_value; }

    ///
    /// \brief setDefault
    ///
    /// Sets the value to the default value
    ///
    /// \return true if the value changed
    ///
    bool setDefault() { return setValue( getDefaultValue() ); }

    ///
    /// \brief setValue
    ///
    /// Set the value to a new value.
    ///
    /// throws range_error if the requested value is out of range
    ///
    /// \param v the requested value
    /// \return true if the value changed
    ///
    bool setValue( value_type v )
    {
        bool r = false;
        value_type min = getMinValue();
        value_type max = getMaxValue();
        if ( v < min )
        {
            throw std::range_error( "setValue() too small" );
        }
        if ( v > max )
        {
            throw std::range_error( "setValue() too large" );
        }
        if ( m_value != v )
        {
            m_value = v;
            m_changed = true;
            r = true;
        }
        return r;
    }

    ///
    /// \brief setValueWithClamp
    ///
    /// Set the value to a new value.
    ///
    /// clamps the value to min or max if the requested value is out of range
    ///
    /// \param v the requested value
    /// \return true if the value changed
    ///
    bool setValueWithClamp( value_type v )
    {
        bool r = false;
        if ( v < getMinValue() )
        {
            v = getMinValue();
        }
        else if ( v > getMaxValue() )
        {
            v = getMaxValue();
        }

        if ( m_value != v )
        {
            m_value = v;
            m_changed = true;
            r = true;
        }
        return r;
    }

    ///
    /// \brief incValue
    ///
    /// Increment the current value by the step size.
    /// Will not increment past the max value
    ///
    /// \return true if the value changed
    ///
    bool incValue()
    {
        bool r = false;
        value_type new_value = m_value + getStepValue();
        if ( new_value > getMaxValue() )
        {
            new_value = getMaxValue();
        }
        if ( m_value != new_value )
        {
            m_value = new_value;
            m_changed = true;
            r = true;
        }
        return r;
    }

    ///
    /// \brief decValue
    ///
    /// Decrement the current value by the step size.
    /// Will not decrement past the min value
    ///
    /// \return true if the value changed
    ///
    bool decValue()
    {
        bool r = false;
        value_type new_value = m_value - getStepValue();
        if ( new_value > getMaxValue() )
        {
            new_value = getMaxValue();
        }
        if ( m_value != new_value )
        {
            m_value = new_value;
            m_changed = true;
            r = true;
        }
        return r;
    }

    ///
    /// \brief getValue
    ///
    /// Get the current value
    ///
    /// \return the value
    ///
    value_type getValue() const { return m_value; }

    ///
    /// \brief getMinValue
    ///
    /// Get the minimum value
    ///
    /// \return the minimum value
    ///
    value_type getMinValue() const
    {
        value_type decoding_multiplier = getDecodingMultiplier();
        value_type decoding_divider = getDecodingDivider();
        value_type v = value_type( min_value ) * decoding_multiplier / decoding_divider;
        return v;
    }

    ///
    /// \brief getMaxValue
    ///
    /// Get the maximum value
    ///
    /// \return the maximum value
    ///
    value_type getMaxValue() const
    {
        value_type decoding_multiplier = getDecodingMultiplier();
        value_type decoding_divider = getDecodingDivider();
        value_type v = value_type( max_value ) * decoding_multiplier / decoding_divider;
        return v;
    }

    ///
    /// \brief getDefaultValue
    ///
    /// Get the default value
    ///
    /// \return the default value
    ///
    value_type getDefaultValue() const
    {
        value_type decoding_multiplier = getDecodingMultiplier();
        value_type decoding_divider = getDecodingDivider();
        value_type v = value_type( default_value ) * decoding_multiplier / decoding_divider;
        return v;
    }

    ///
    /// \brief getStepValue
    ///
    /// Get the step value
    ///
    /// \return the step value
    ///
    value_type getStepValue() const
    {
        value_type decoding_multiplier = getDecodingMultiplier();
        value_type decoding_divider = getDecodingDivider();
        value_type v = value_type( step_value ) * decoding_multiplier / decoding_divider;
        return v;
    }

    ///
    /// \brief getEncodingMultiplier
    ///
    /// Get the multiplier required to convert an unencoded value
    /// to an encoded value.
    ///
    /// If the multiplier_power is <=0 this method returns 1
    ///
    /// \return The value to multiply with
    ///
    value_type getEncodingMultiplier() const
    {
        value_type r = 1;
        if ( multiplier_power < 0 )
        {
            r = integer_pow10<value_type>( -multiplier_power );
        }
        return r;
    }

    ///
    /// \brief getEncodingDivider
    ///
    /// Get the divider required to convert an unencoded value
    /// to an encoded value
    ///
    /// If the multiplier_power is >=0 this method returns 1
    ///
    /// \return the value to divide with
    ///
    value_type getEncodingDivider() const
    {
        value_type r = 1;
        if ( multiplier_power > 0 )
        {
            r = integer_pow10<value_type>( multiplier_power );
        }
        return r;
    }

    ///
    /// \brief getDecodingMultiplier
    ///
    /// Get the multiplier required to convert an encoded value
    /// to an unencoded value.
    ///
    /// If the multiplier_power is >=0 this method returns 1
    ///
    /// \return The value to multiply with
    ///
    value_type getDecodingMultiplier() const
    {
        value_type r = 1;
        if ( multiplier_power > 0 )
        {
            r = integer_pow10<value_type>( multiplier_power );
        }
        return r;
    }

    ///
    /// \brief getDecodingDivider
    ///
    /// Get the divider required to convert an encoded value
    /// to an unencoded value
    ///
    /// If the multiplier_power is <=0 this method returns 1
    ///
    /// \return the value to divide with
    ///
    value_type getDecodingDivider() const
    {
        value_type r = 1;
        if ( multiplier_power < 0 )
        {
            r = integer_pow10<value_type>( -multiplier_power );
        }
        return r;
    }

    ///
    /// Default template function to help round integer types
    ///
    template <typename T>
    static T valueRound( T v )
    {
        return v;
    }

    ///
    /// \brief valueRound
    ///
    /// overload to round float types
    ///
    /// \param v value to round
    /// \return the rounded value
    ///
    static float valueRound( float v ) { return roundf( v ); }

    ///
    /// \brief valueRound
    ///
    /// overload to round double types
    ///
    /// \param v the value to round
    /// \return  the rounded value
    ///
    static double valueRound( double v ) { return round( v ); }

    ///
    /// \brief getEncodedValue
    ///
    /// Convert the current value to an encoded value
    ///
    /// If the result type is integer, then the result is rounded first.
    ///
    /// If the result type does not have enough bits to hold the full range
    /// of the encoded value, this function throws domain_error
    ///
    /// \param dest Pointer to the result
    ///
    template <typename T>
    void getEncodedValue( T *dest ) const
    {
        value_type encoding_multiplier = getEncodingMultiplier();
        value_type encoding_divider = getEncodingDivider();
        value_type v = ( getValue() * encoding_multiplier / encoding_divider );
        value_type rounded_v;
#if __cplusplus >= 201103L

        if ( !std::is_floating_point<T>::value && std::is_floating_point<value_type>::value )
        {
            rounded_v = valueRound( v );
        }
        else
        {
            rounded_v = v;
        }
        if ( getMaxValue() * encoding_multiplier > (value_type)std::numeric_limits<T>::max() )
        {
            throw std::domain_error( "Max Value too large for encoding" );
        }
        if ( getMinValue() * encoding_multiplier < (value_type)std::numeric_limits<T>::min() )
        {
            throw std::domain_error( "Min Value too small for encoding" );
        }
#else
        rounded_v = valueRound( v );
#endif
        *dest = rounded_v;
    }

    ///
    /// \brief setFromEncodedValue
    ///
    /// If the encoded_v is out of range, this function will throw range_error
    ///
    /// \param encoded_v The encoded value to use to set the value to
    ///
    /// \return true if the value changed
    template <typename T>
    bool setFromEncodedValue( T encoded_v )
    {
        value_type decoding_multiplier = getDecodingMultiplier();
        value_type decoding_divider = getDecodingDivider();
        value_type v = value_type( encoded_v ) * decoding_multiplier / decoding_divider;
        return setValue( v );
    }

    ///
    /// \brief setFromEncodedValueWithClamp
    ///
    /// If the encoded_v is out of range, this function will clamp the
    /// value to the appropriate range
    ///
    /// \param encoded_v The encoded value to use to set the value to
    ///
    /// \return true if the value changed
    template <typename T>
    bool setFromEncodedValueWithClamp( T encoded_v )
    {
        value_type decoding_multiplier = getDecodingMultiplier();
        value_type decoding_divider = getDecodingDivider();
        value_type v = value_type( encoded_v ) * decoding_multiplier / decoding_divider;
        return setValueWithClamp( v );
    }

    ///
    /// \brief getUnitsCode
    ///
    /// Get the units code for this value
    ///
    /// \return The Units enumeration value
    ///
    uint8_t getUnitsCode() const { return UnitsValue; }


    ///
    /// \brief setChanged
    ///
    /// Set the changed flag
    ///
    void setChanged()
    {
        m_changed=true;
    }

    ///
    /// \brief clearChanged
    ///
    /// Clear the changed flag
    ///
    void clearChanged()
    {
        m_changed=false;
    }

    ///
    /// \brief getChanged
    /// \return true if the value had changed since the last time clearChanged() was called
    ///
    bool getChanged() const
    {
        return m_changed;
    }



    ///
    /// \brief getEncodedMinValue
    /// \return The encoded minimum value
    ///
    int64_t getEncodedMinValue() const
    {
        return min_value;
    }

    ///
    /// \brief getEncodedMaxValue
    /// \return The encoded maximum value
    ///
    int64_t getEncodedMaxValue() const
    {
        return max_value;
    }

    ///
    /// \brief getEncodedStepValue
    /// \return The encoded step value
    ///
    int64_t getEncodedStepValue() const
    {
        return step_value;
    }

    ///
    /// \brief getEncodedDefaultValue
    /// \return the encoded default value
    ///
    int64_t getEncodedDefaultValue() const
    {
        return default_value;
    }

    const char * getUnitsSuffix() const
    {
        getAvdeccUnitsSuffix( getUnitsCode() );
    }

  private:
    ///
    /// \brief m_value
    ///
    /// The actual non-encoded value
    ///
    value_type m_value;

    ///
    /// \brief m_changed
    ///
    /// True if the value has changed since the last time clearChanged() was called
    ///
    bool m_changed;
};
}
