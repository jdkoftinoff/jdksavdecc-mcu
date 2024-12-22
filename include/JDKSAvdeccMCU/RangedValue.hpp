#pragma once

#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/Helpers.hpp"
#include <string>
#include <sstream>
#include <limits>

namespace JDKSAvdeccMCU
{
using std::string;

struct AvdeccControlString
{
    AvdeccControlString()
    {
        for( size_t i=0; i<sizeof(m_value); ++i )
        {
            m_value[i] = 0;
        }
    }

    void clear()
    {
        for( size_t i=0; i<sizeof(m_value); ++i )
        {
            m_value[i] = 0;
        }
    }

    void set( const string &s )
    {
        clear();
        for( size_t i=0; i<s.length() && i<sizeof(m_value)-1; ++i )
        {
            if( s[i]!=0 )
            {
                m_value[i] = uint8_t(s[i]);
            }
        }
    }

    string get() const
    {
        string s;
        for( size_t i=0; i<sizeof(m_value)-1; ++i )
        {
            s.push_back( char(m_value[i]));
        }
        return s;
    }

    /// Null terminated UTF-8 string, 406 octets long max including null termination
    uint8_t m_value[406];
};

/// \brief powers_of_ten
///
/// The powers of ten values as float from 1e-12 to 1e12 inclusive
///
extern float powers_of_ten[25];

///
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
template <typename T>
T getEncodingMultiplier(int8_t multiplier_power)
{
    T r = 1;
    if ( multiplier_power < 0 )
    {
        r = integer_pow10<T>( -multiplier_power );
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
template <typename T>
T getEncodingDivider(int8_t multiplier_power)
{
    T r = 1;
    if ( multiplier_power > 0 )
    {
        r = integer_pow10<T>( multiplier_power );
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
template <typename T>
T getDecodingMultiplier(int8_t multiplier_power)
{
    T r = 1;
    if ( multiplier_power > 0 )
    {
        r = integer_pow10<T>( multiplier_power );
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
template <typename T>
T getDecodingDivider(int8_t multiplier_power )
{
    T r = 1;
    if ( multiplier_power < 0 )
    {
        r = integer_pow10<T>( -multiplier_power );
    }
    return r;
}


///
/// \brief The UnitsCode enum
///
/// Enumerations matching the Units definitions from IEEE Std 1722.1-2013 Clause 7.3.3
///
enum class UnitsCode : uint8_t
{
    UNITLESS=0x00,
    COUNT=0x01,
    PERCENT=0x02,
    FSTOP=0x03,
    TIME_SECONDS=0x08,
    TIME_MINUTES=0x09,
    TIME_HOURS=0x0a,
    TIME_DAYS=0x0b,
    TIME_MONTHS=0x0c,
    TIME_YEARS=0x0d,
    TIME_SAMPLES=0x0e,
    TIME_FRAMES=0x0f,
    FREQUENCY_HERTZ=0x10,
    FREQUENCY_SEMITONES=0x11,
    FREQUENCY_CENTS=0x12,
    FREQUENCY_OCTAVES=0x13,
    FREQUENCY_FPS=0x14,
    DISTANCE_METRES=0x18,
    TEMPERATURE_KELVIN=0x20,
    MASS_GRAMS=0x28,
    VOLTAGE_VOLTS=0x30,
    VOLTAGE_DBV=0x31,
    VOLTAGE_DBU=0x32,
    CURRENT_AMPS=0x38,
    POWER_WATTS=0x40,
    POWER_DBM=0x41,
    POWER_DBW=0x42,
    PRESSURE_PASCALS=0x48,
    MEMORY_BITS=0x50,
    MEMORY_BYTES=0x51,
    MEMORY_KIBIBYTES=0x52,
    MEMORY_MEBIBYTES=0x53,
    MEMORY_GIBIBYTES=0x54,
    MEMORY_TEBIBYTES=0x55,
    MEMORY_BANDWIDTH_BITS_PER_SEC=0x58,
    MEMORY_BANDWIDTH_BYTES_PER_SEC=0x59,
    MEMORY_BANDWIDTH_KIBIBYTES_PER_SEC=0x5a,
    MEMORY_BANDWIDTH_MEBIBYTES_PER_SEC=0x5b,
    MEMORY_BANDWIDTH_GIGIBYTES_PER_SEC=0x5c,
    MEMORY_BANDWIDTH_TEBIBYTES_PER_SEC=0x5d,
    LUMINOSITY_CANDELAS=0x60,
    ENERGY_JOULES=0x68,
    ANGLE_RADIANS=0x70,
    FORCE_NEWTONS=0x78,
    RESISTANCE_OHMS=0x80,
    VELOCITY_METRES_PER_SEC=0x88,
    VELOCITY_RADIANS_PER_SEC=0x89,
    ACCELERATION_METRES_PER_SEC_SQUARED=0x90,
    ACCELERATION_RADIANS_PER_SEC_SQUARED=0x91,
    MAGNETIC_FLUX_TESLAS=0x98,
    AREA_METERS_SQUARED=0xa0,
    VOLUME_METERS_CUBED=0xa8,
    VOLUME_LITRES=0xa9,
    LEVEL_DB=0xb0,
    LEVEL_DB_PEAK=0xb1,
    LEVEL_DB_RMS=0xb2,
    LEVEL_DBFS=0xb3,
    LEVEL_DBFS_PEAK=0xb4,
    LEVEL_DBFS_RMS=0xb5,
    LEVEL_DBTP=0xb6,
    LEVEL_DB_A=0xb7,
    LEVEL_DB_B=0xb8,
    LEVEL_DB_C=0xb9,
    LEVEL_DB_SPL=0xba,
    LEVEL_LU=0xbb,
    LEVEL_LUFS=0xbc
};

///
/// \brief getAvdeccUnitsSuffix
///
/// Get description/suffix for a UnitsCode
///
/// \param units_code
/// \return UTF-8 c style string containing a presentable suffix describing the unit
///
const char *getAvdeccUnitsSuffix(UnitsCode units_code);

///
/// \brief The EncodingType enum
///
/// Enumeration of possible encoding types to describe transport encoding and
/// storage encoding
///
enum class EncodingType : uint8_t
{
    ENCODING_INT8,
    ENCODING_UINT8,
    ENCODING_INT16,
    ENCODING_UINT16,
    ENCODING_INT32,
    ENCODING_UINT32,
    ENCODING_INT64,
    ENCODING_UINT64,
    ENCODING_FLOAT,
    ENCODING_DOUBLE,
    ENCODING_STRING
};

///
/// \brief EncodingTypeFor template traits class
///
/// Specialized for the C++ types which map to an EncodingType enumeration value
/// So that code can look up the enumeration value for a static type
///
template <typename T>
struct EncodingTypeFor
{
};

template <>
struct EncodingTypeFor<int8_t>
{
    using type = int8_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_INT8; }
};

template <>
struct EncodingTypeFor<uint8_t>
{
    using type = uint8_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_UINT8; }
};

template <>
struct EncodingTypeFor<int16_t>
{
    using type = int16_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_INT16; }
};

template <>
struct EncodingTypeFor<uint16_t>
{
    using type = uint16_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_UINT16; }
};

template <>
struct EncodingTypeFor<int32_t>
{
    using type = int32_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_INT32; }
};

template <>
struct EncodingTypeFor<uint32_t>
{
    using type = uint32_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_UINT32; }
};

template <>
struct EncodingTypeFor<int64_t>
{
    using type = int64_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_INT64; }
};

template <>
struct EncodingTypeFor<uint64_t>
{
    using type = uint64_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_UINT64; }
};

template <>
struct EncodingTypeFor<float>
{
    using type = float;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_FLOAT; }
};

template <>
struct EncodingTypeFor<double>
{
    using type = uint64_t;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_DOUBLE; }
};

template <>
struct EncodingTypeFor<string>
{
    using type = string;
    static EncodingType getEncodingType() { return EncodingType::ENCODING_STRING; }
};

///
/// \brief The RangedValueBase class
///
/// Base class for the RangedValue
/// Pure-abstract interfaces to allow querying of meta data and values
/// and setting of values
///
class RangedValueBase
{
public:
    virtual ~RangedValueBase() {}

    ///
    /// \brief getUnitsCode
    ///
    /// \return The UnitsCode for this value
    ///
    virtual UnitsCode getUnitsCode() const = 0;

    ///
    /// \brief getStorageType
    /// \return The EncodingType used for the storage of this value
    ///
    virtual EncodingType getStorageType() const = 0;

    ///
    /// \brief getEncodingType
    /// \return The EncodingType used for the encoded transport of this value
    ///
    virtual EncodingType getEncodingType() const = 0;


    ///
    /// \brief setUnencodedValueString
    ///
    /// If storage type and encoding type are both string, then set
    /// the string value
    ///
    /// \param v The new string value
    /// \return true if the value changed
    ///
    virtual bool setUnencodedValueString( string const &v ) = 0;

    ///
    /// \brief setUnencodedValueBool
    ///
    /// Set the unencoded value from a bool
    ///
    /// \param v value to set
    /// \return true if the value changed
    ///
    virtual bool setUnencodedValueBool( bool v ) =0;

    ///
    /// \brief setUnencodedValueFloat
    ///
    /// Set the unencoded value from a float
    ///
    /// \param v value to set
    /// \return true if the value changed
    ///
    virtual bool setUnencodedValueFloat( float v ) = 0;

    ///
    /// \brief setUnencodedValueDouble
    ///
    /// Set the unencoded value from a double
    ///
    /// \param v value to set
    /// \return true if the value changed
    ///
    virtual bool setUnencodedValueDouble( double v ) = 0;

    ///
    /// \brief setUnencodedValueInt64
    ///
    /// Set the unencoded value from an int64_t
    ///
    /// \param v value to set
    /// \return true if the value changed
    ///
    virtual bool setUnencodedValueInt64( int64_t v ) = 0;

    ///
    /// \brief setUnencodedValueUInt64
    ///
    /// Set the unencoded value from an uint64_t
    ///
    /// \param v value to set
    /// \return true if the value changed
    ///
    virtual bool setUnencodedValueUInt64( uint64_t v ) = 0;


    ///
    /// \brief getUnencodedValueString
    ///
    /// Get the unencoded value string. If the
    /// storage type and encoding type is string, then
    /// it returns the value. If the storage type is not
    /// string then it returns the textual representation
    /// optionally with units
    ///
    /// \return the string representation of the unencoded value
    ///
    virtual string getUnencodedValueString(bool enable_units=true) const = 0;

    ///
    /// \brief getUnencodedValueBool
    /// \return the unencoded value as a bool
    ///
    virtual bool getUnencodedValueBool() const =0;

    ///
    /// \brief getUnencodedValueFloat
    /// \return the unencoded value as a float
    ///
    virtual float getUnencodedValueFloat() const =0;

    ///
    /// \brief getUnencodedValueDouble
    /// \return the unencoded value as a double
    ///
    virtual double getUnencodedValueDouble() const =0;

    ///
    /// \brief getUnencodedValueInt64
    /// \return the unencoded value as an int64_t
    ///
    virtual int64_t getUnencodedValueInt64() const =0;

    ///
    /// \brief getUnencodedValueUInt64
    /// \return the unencoded value as a uint64_t
    ///
    virtual uint64_t getUnencodedValueUInt64() const =0;

    ///
    /// \brief incValue
    ///
    /// Increment the value by the step value
    ///
    /// \return true if the value changed
    ///
    virtual bool incValue() = 0;

    ///
    /// \brief decValue
    ///
    /// Decrement the value by the step value
    ///
    /// \return true if the value changed
    ///
    virtual bool decValue() = 0;

    virtual void getEncodedValueAvdeccString( AvdeccControlString *storage ) const = 0;
    virtual int8_t getEncodedValueInt8() const =0;
    virtual uint8_t getEncodedValueUInt8() const =0;
    virtual int16_t getEncodedValueInt16() const =0;
    virtual uint16_t getEncodedValueUInt16() const =0;
    virtual int32_t getEncodedValueInt32() const =0;
    virtual uint32_t getEncodedValueUInt32() const =0;
    virtual int64_t getEncodedValueInt64() const =0;
    virtual uint64_t getEncodedValueUInt64() const =0;
    virtual float getEncodedValueFloat() const =0;
    virtual double getEncodedValueDouble() const =0;

    virtual void setFromEncodedValueAvdeccString( const AvdeccControlString *storage ) =0;
    virtual bool setFromEncodedValueInt8( int8_t v ) =0;
    virtual bool setFromEncodedValueUInt8( uint8_t v ) =0;
    virtual bool setFromEncodedValueInt16( int16_t v ) =0;
    virtual bool setFromEncodedValueUInt16( uint16_t v ) =0;
    virtual bool setFromEncodedValueInt32( int32_t v ) =0;
    virtual bool setFromEncodedValueUInt32( uint32_t v ) =0;
    virtual bool setFromEncodedValueInt64( int64_t v ) =0;
    virtual bool setFromEncodedValueUInt64( uint64_t v ) =0;
    virtual bool setFromEncodedValueFloat( float v ) =0;
    virtual bool setFromEncodedValueDouble( double v ) =0;

    virtual bool setFromEncodedValueWithClampInt8( int8_t v ) =0;
    virtual bool setFromEncodedValueWithClampUInt8( uint8_t v ) =0;
    virtual bool setFromEncodedValueWithClampInt16( int16_t v ) =0;
    virtual bool setFromEncodedValueWithClampUInt16( uint16_t v ) =0;
    virtual bool setFromEncodedValueWithClampInt32( int32_t v ) =0;
    virtual bool setFromEncodedValueWithClampUInt32( uint32_t v ) =0;
    virtual bool setFromEncodedValueWithClampInt64( int64_t v ) =0;
    virtual bool setFromEncodedValueWithClampUInt64( uint64_t v ) =0;
    virtual bool setFromEncodedValueWithClampFloat( float v ) =0;
    virtual bool setFromEncodedValueWithClampDouble( double v ) =0;


    ///
    /// \brief setChanged
    ///
    /// Set the changed flag
    ///
    virtual void setChanged() = 0;

    ///
    /// \brief clearChanged
    ///
    /// Clear the changed flag
    ///
    virtual void clearChanged() = 0;

    ///
    /// \brief getChanged
    /// \return true if the value had changed since the last time clearChanged() was called
    ///
    virtual bool getChanged() const = 0;

    ///
    /// \brief getEncodedMinValue
    /// \return The encoded minimum value
    ///
    virtual int64_t getEncodedMinValue() const = 0;

    ///
    /// \brief getEncodedMaxValue
    /// \return The encoded maximum value
    ///
    virtual int64_t getEncodedMaxValue() const = 0;

    ///
    /// \brief getEncodedStepValue
    /// \return The encoded step value
    ///
    virtual int64_t getEncodedStepValue() const = 0;

    ///
    /// \brief getEncodedDefaultValue
    /// \return the encoded default value
    ///
    virtual int64_t getEncodedDefaultValue() const = 0;

    ///
    /// \brief getEncodingMultiplierPower
    /// \return the power of 10 used for encoding
    ///
    virtual int8_t getEncodingMultiplierPower() const = 0;

    virtual const char * getUnitsSuffix() const = 0;
};


/// \brief the RangedValue class
///
/// A holder for a limited range value with options for defaults and inc/dec
/// step size
/// and with the capability to encode/decode into a fixed point integer type
///
template <UnitsCode UnitsValue,
          int64_t MinValue,
          int64_t MaxValue,
          int64_t DefaultValue = 0,
          int64_t StepValue = 1,
          int MultiplierPowerValue = 0,
          typename EncodedT = int32_t,
          typename ValueT = float>
class RangedValue : public RangedValueBase
{
    static_assert( MinValue <= DefaultValue, "MinValue is not less than or equal to DefaultValue" );
    static_assert( DefaultValue <= MaxValue, "DefaultValue is not less than or equal to MaxValue" );

  public:
    typedef ValueT value_type;
    typedef EncodedT encoded_type;

    static const UnitsCode units = UnitsValue;
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

    EncodingType getStorageType() const override
    {
        return EncodingTypeFor<ValueT>::getEncodingType();
    }

    EncodingType getEncodingType() const override
    {
        return EncodingTypeFor<EncodedT>::getEncodingType();
    }

    bool setUnencodedValueString( string const &v ) override
    {
        return false;
    }


    bool setUnencodedValueBool( bool v ) override
    {
        uint8_t v8 = v ? 255 : 0;

        return setValue(value_type(v8));
    }

    bool setUnencodedValueFloat( float v ) override
    {
        return setValue(v);
    }

    bool setUnencodedValueDouble( double v ) override
    {
        return setValue(v);
    }

    bool setUnencodedValueInt64( int64_t v ) override
    {
        return setValue(v);
    }

    bool setUnencodedValueUInt64( uint64_t v ) override
    {
        return setValue(v);
    }

    string getUnencodedValueString(bool enable_units) const
    {
        std::ostringstream buf;
        buf << m_value;
        if( enable_units )
        {
            const char *suffix =getAvdeccUnitsSuffix(units);
            if( suffix && *suffix )
            {
                buf << " " << suffix;
            }
        }
        return buf.str();
    }

    bool getUnencodedValueBool() const override
    {
        return m_value != value_type();
    }

    float getUnencodedValueFloat() const override
    {
        return m_value;
    }

    double getUnencodedValueDouble() const override
    {
        return m_value;
    }

    int64_t getUnencodedValueInt64() const override
    {
        return m_value;
    }


    uint64_t getUnencodedValueUInt64() const override
    {
        return m_value;
    }


    ///
    /// \brief incValue
    ///
    /// Increment the current value by the step size.
    /// Will not increment past the max value
    ///
    /// \return true if the value changed
    ///
    bool incValue() override
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
    bool decValue() override
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
        *dest = rounded_v;
    }

    void getEncodedValueAvdeccString( AvdeccControlString *storage ) const override
    {
        string s = getUnencodedValueString(false);

        storage->set(s);
    }

    int8_t getEncodedValueInt8() const override
    {
        int8_t v;
        getEncodedValue( &v );
        return v;
    }

    uint8_t getEncodedValueUInt8() const override
    {
        uint8_t v;
        getEncodedValue( &v );
        return v;
    }

    int16_t getEncodedValueInt16() const override
    {
        int16_t v;
        getEncodedValue( &v );
        return v;
    }

    uint16_t getEncodedValueUInt16() const override
    {
        uint16_t v;
        getEncodedValue( &v );
        return v;
    }

    int32_t getEncodedValueInt32() const override
    {
        int32_t v;
        getEncodedValue( &v );
        return v;
    }

    uint32_t getEncodedValueUInt32() const override
    {
        uint32_t v;
        getEncodedValue( &v );
        return v;
    }

    int64_t getEncodedValueInt64() const override
    {
        int64_t v;
        getEncodedValue( &v );
        return v;
    }

    uint64_t getEncodedValueUInt64() const override
    {
        uint64_t v;
        getEncodedValue( &v );
        return v;
    }

    float getEncodedValueFloat() const override
    {
        float v;
        getEncodedValue( &v );
        return v;
    }

    double getEncodedValueDouble() const override
    {
        double v;
        getEncodedValue( &v );
        return v;
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

    void setFromEncodedValueAvdeccString( const AvdeccControlString *storage ) override
    {
        string s = storage->get();
        std::istringstream buf(s);
        buf >> m_value;
    }

    bool setFromEncodedValueInt8( int8_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt8( uint8_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueInt16( int16_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt16( uint16_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueInt32( int32_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt32( uint32_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueInt64( int64_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt64( uint64_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueFloat( float v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueDouble( double v ) override
    {
        return setFromEncodedValue(v);
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

    bool setFromEncodedValueWithClampInt8( int8_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt8( uint8_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampInt16( int16_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt16( uint16_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampInt32( int32_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt32( uint32_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampInt64( int64_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt64( uint64_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampFloat( float v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampDouble( double v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    ///
    /// \brief getUnitsCode
    ///
    /// Get the units code for this value
    ///
    /// \return The Units enumeration value
    ///
    UnitsCode getUnitsCode() const override { return UnitsValue; }


    ///
    /// \brief setChanged
    ///
    /// Set the changed flag
    ///
    void setChanged() override
    {
        m_changed=true;
    }

    ///
    /// \brief clearChanged
    ///
    /// Clear the changed flag
    ///
    void clearChanged() override
    {
        m_changed=false;
    }

    ///
    /// \brief getChanged
    /// \return true if the value had changed since the last time clearChanged() was called
    ///
    bool getChanged() const override
    {
        return m_changed;
    }

    ///
    /// \brief getEncodedMinValue
    /// \return The encoded minimum value
    ///
    int64_t getEncodedMinValue() const override
    {
        return min_value;
    }

    ///
    /// \brief getEncodedMaxValue
    /// \return The encoded maximum value
    ///
    int64_t getEncodedMaxValue() const override
    {
        return max_value;
    }

    ///
    /// \brief getEncodedStepValue
    /// \return The encoded step value
    ///
    int64_t getEncodedStepValue() const override
    {
        return step_value;
    }

    ///
    /// \brief getEncodedDefaultValue
    /// \return the encoded default value
    ///
    int64_t getEncodedDefaultValue() const override
    {
        return default_value;
    }

    ///
    /// \brief getEncodingMultiplierPower
    /// \return the power of 10 used for encoding
    ///
    int8_t getEncodingMultiplierPower() const override
    {
        return multiplier_power;
    }


    const char * getUnitsSuffix() const override
    {
        return getAvdeccUnitsSuffix( getUnitsCode() );
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


template <UnitsCode UnitsValue,
          int64_t MinValue,
          int64_t MaxValue,
          int64_t DefaultValue,
          int64_t StepValue,
          int MultiplierPowerValue>
class RangedValue<
        UnitsValue,MinValue,MaxValue,DefaultValue,StepValue,MultiplierPowerValue,string,string
        > : public RangedValueBase
{
    static_assert( MinValue <= DefaultValue, "MinValue is not less than or equal to DefaultValue" );
    static_assert( DefaultValue <= MaxValue, "DefaultValue is not less than or equal to MaxValue" );

  public:
    typedef string value_type;
    typedef string encoded_type;

    static const UnitsCode units = UnitsValue;
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
        bool r=false;
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
        bool r=false;
        if ( m_value != v )
        {
            m_value = v;
            m_changed = true;
            r = true;
        }
        return r;
    }

    EncodingType getStorageType() const override
    {
        return EncodingTypeFor<value_type>::getEncodingType();
    }

    EncodingType getEncodingType() const override
    {
        return EncodingTypeFor<encoded_type>::getEncodingType();
    }

    bool setUnencodedValueString( string const &v ) override
    {
        return setValue(v);
    }

    bool setUnencodedValueBool( bool v ) override
    {
        return setValue( v ? "true" : "false" );
    }

    bool setUnencodedValueFloat( float v ) override
    {
        std::ostringstream buf;
        buf << v;
        return setValue(buf.str());
    }

    bool setUnencodedValueDouble( double v ) override
    {
        std::ostringstream buf;
        buf << v;
        return setValue(buf.str());
    }

    bool setUnencodedValueInt64( int64_t v ) override
    {
        std::ostringstream buf;
        buf << v;
        return setValue(buf.str());
    }

    bool setUnencodedValueUInt64( uint64_t v ) override
    {
        std::ostringstream buf;
        buf << v;
        return setValue(buf.str());
    }

    string getUnencodedValueString(bool enable_units) const
    {
        std::ostringstream buf;
        buf << m_value;
        if( enable_units )
        {
            const char *suffix =getAvdeccUnitsSuffix(units);
            if( suffix && *suffix )
            {
                buf << " " << suffix;
            }
        }
        return buf.str();
    }

    bool getUnencodedValueBool() const override
    {
        return m_value == "true" ? true : false;
    }

    float getUnencodedValueFloat() const override
    {
        std::istringstream buf(m_value);
        float v;
        buf >> v;
        return v;
    }

    double getUnencodedValueDouble() const override
    {
        std::istringstream buf(m_value);
        double v;
        buf >> v;
        return v;
    }

    int64_t getUnencodedValueInt64() const override
    {
        std::istringstream buf(m_value);
        int64_t v;
        buf >> v;
        return v;
    }


    uint64_t getUnencodedValueUInt64() const override
    {
        std::istringstream buf(m_value);
        uint64_t v;
        buf >> v;
        return v;
    }


    ///
    /// \brief incValue
    ///
    /// Increment the current value by the step size.
    /// Will not increment past the max value
    ///
    /// \return true if the value changed
    ///
    bool incValue() override
    {
        return false;
    }

    ///
    /// \brief decValue
    ///
    /// Decrement the current value by the step size.
    /// Will not decrement past the min value
    ///
    /// \return true if the value changed
    ///
    bool decValue() override
    {
        return false;
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
        return "";
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
        return "";
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
        return "";
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
        return "";
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
        return "";
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
        return "";
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
        return "";
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
        return "";
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
        std::istringstream buf(m_value);
        buf >> *dest;
    }

    void getEncodedValueAvdeccString( AvdeccControlString *storage ) const override
    {
        storage->set(m_value);
    }

    int8_t getEncodedValueInt8() const override
    {
        int8_t v;
        getEncodedValue( &v );
        return v;
    }

    uint8_t getEncodedValueUInt8() const override
    {
        uint8_t v;
        getEncodedValue( &v );
        return v;
    }

    int16_t getEncodedValueInt16() const override
    {
        int16_t v;
        getEncodedValue( &v );
        return v;
    }

    uint16_t getEncodedValueUInt16() const override
    {
        uint16_t v;
        getEncodedValue( &v );
        return v;
    }

    int32_t getEncodedValueInt32() const override
    {
        int32_t v;
        getEncodedValue( &v );
        return v;
    }

    uint32_t getEncodedValueUInt32() const override
    {
        uint32_t v;
        getEncodedValue( &v );
        return v;
    }

    int64_t getEncodedValueInt64() const override
    {
        int64_t v;
        getEncodedValue( &v );
        return v;
    }

    uint64_t getEncodedValueUInt64() const override
    {
        uint64_t v;
        getEncodedValue( &v );
        return v;
    }

    float getEncodedValueFloat() const override
    {
        float v;
        getEncodedValue( &v );
        return v;
    }

    double getEncodedValueDouble() const override
    {
        double v;
        getEncodedValue( &v );
        return v;
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
        std::ostringstream buf;
        buf << encoded_v;
        return setValue(buf.str());
    }

    void setFromEncodedValueAvdeccString( const AvdeccControlString *storage ) override
    {
        m_value = storage->get();
    }

    bool setFromEncodedValueInt8( int8_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt8( uint8_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueInt16( int16_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt16( uint16_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueInt32( int32_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt32( uint32_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueInt64( int64_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueUInt64( uint64_t v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueFloat( float v ) override
    {
        return setFromEncodedValue(v);
    }

    bool setFromEncodedValueDouble( double v ) override
    {
        return setFromEncodedValue(v);
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
        return setFromEncodedValue(encoded_v);
    }

    bool setFromEncodedValueWithClampInt8( int8_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt8( uint8_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampInt16( int16_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt16( uint16_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampInt32( int32_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt32( uint32_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampInt64( int64_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampUInt64( uint64_t v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampFloat( float v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    bool setFromEncodedValueWithClampDouble( double v ) override
    {
        return setFromEncodedValueWithClamp(v);
    }

    ///
    /// \brief getUnitsCode
    ///
    /// Get the units code for this value
    ///
    /// \return The Units enumeration value
    ///
    UnitsCode getUnitsCode() const override { return UnitsValue; }


    ///
    /// \brief setChanged
    ///
    /// Set the changed flag
    ///
    void setChanged() override
    {
        m_changed=true;
    }

    ///
    /// \brief clearChanged
    ///
    /// Clear the changed flag
    ///
    void clearChanged() override
    {
        m_changed=false;
    }

    ///
    /// \brief getChanged
    /// \return true if the value had changed since the last time clearChanged() was called
    ///
    bool getChanged() const override
    {
        return m_changed;
    }

    ///
    /// \brief getEncodedMinValue
    /// \return The encoded minimum value
    ///
    int64_t getEncodedMinValue() const override
    {
        return min_value;
    }

    ///
    /// \brief getEncodedMaxValue
    /// \return The encoded maximum value
    ///
    int64_t getEncodedMaxValue() const override
    {
        return max_value;
    }

    ///
    /// \brief getEncodedStepValue
    /// \return The encoded step value
    ///
    int64_t getEncodedStepValue() const override
    {
        return step_value;
    }

    ///
    /// \brief getEncodedDefaultValue
    /// \return the encoded default value
    ///
    int64_t getEncodedDefaultValue() const override
    {
        return default_value;
    }

    ///
    /// \brief getEncodingMultiplierPower
    /// \return the power of 10 used for encoding
    ///
    int8_t getEncodingMultiplierPower() const override
    {
        return multiplier_power;
    }


    const char * getUnitsSuffix() const override
    {
        return getAvdeccUnitsSuffix( getUnitsCode() );
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

