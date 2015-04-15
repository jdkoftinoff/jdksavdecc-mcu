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
#include "JDKSAvdeccMCU/FixedBuffer.hpp"
#include "JDKSAvdeccMCU/Helpers.hpp"

namespace JDKSAvdeccMCU
{

template < typename T >
struct EncodedControlItemTraits
{
};

template <>
struct EncodedControlItemTraits<int8_t>
{
    typedef int8_t value_type;
    static bool isSigned() { return true; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {        
        value_type v;
        v = (value_type)jdksavdecc_uint8_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint8_set( (uint8_t)ev,encoded_value,pos);
    }
};

template <>
struct EncodedControlItemTraits<uint8_t>
{
    typedef uint8_t value_type;
    static bool isSigned() { return false; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_uint8_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint8_set( (uint8_t)ev,encoded_value,pos);
    }
};

template <>
struct EncodedControlItemTraits<int16_t>
{
    typedef int16_t value_type;

    static bool isSigned() { return true; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_uint16_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint16_set( (uint16_t)ev,encoded_value,pos);
    }
};

template <>
struct EncodedControlItemTraits<uint16_t>
{
    typedef uint16_t value_type;

    static bool isSigned() { return false; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_uint16_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint16_set( (uint16_t)ev,encoded_value,pos);
    }

};

template <>
struct EncodedControlItemTraits<int32_t>
{
    typedef int32_t value_type;

    static bool isSigned() { return true; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_uint32_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint32_set( (uint32_t)ev,encoded_value,pos);
    }
};

template <>
struct EncodedControlItemTraits<uint32_t>
{
    typedef uint32_t value_type;

    static bool isSigned() { return false; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_uint32_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint32_set( (uint32_t)ev,encoded_value,pos);
    }

};

template <>
struct EncodedControlItemTraits<int64_t>
{
    typedef int64_t value_type;

    static bool isSigned() { return true; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_uint64_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint64_set( (uint64_t)ev,encoded_value,pos);
    }
};

template <>
struct EncodedControlItemTraits<uint64_t>
{
    typedef uint64_t value_type;

    static bool isSigned() { return false; }
    static bool isFloatingPoint() { return false; }
    static bool isIntegral() { return true; }


    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_uint64_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_uint64_set( (uint64_t)ev,encoded_value,pos);
    }

};

template <>
struct EncodedControlItemTraits<float>
{
    typedef float value_type;

    static bool isSigned() { return true; }
    static bool isFloatingPoint() { return true; }
    static bool isIntegral() { return false; }

    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_float_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_float_set( (uint64_t)ev,encoded_value,pos);
    }

};

template <>
struct EncodedControlItemTraits<double>
{
    typedef double value_type;

    static bool isSigned() { return true; }
    static bool isFloatingPoint() { return true; }
    static bool isIntegral() { return false; }


    template <typename U>
    static U decodeNetworkData(
            const void *encoded_value,
            ssize_t pos,
            int8_t multiplier_code )
    {
        value_type v;
        v = (value_type)jdksavdecc_double_get(encoded_value,pos);
        U r = convertEncodedValueToDecoded<U>( v, multiplier_code );
        return r;
    }

    template <typename U>
    static void encodeNetworkData(
            void *encoded_value,
            ssize_t pos,
            U v,
            int8_t multiplier_code )
    {
        value_type ev = convertDecodedValueToEncoded<value_type>(v,multiplier_code);
        jdksavdecc_double_set( (uint64_t)ev,encoded_value,pos);
    }

};

///
/// \brief The ControlDescriptionItemInfoBase class
///
/// Base class for encoded type specific value meta-information.
///
class ControlDescriptionItemInfoBase
{
public:

    ///
    /// \brief ControlDescriptionItemInfoBase
    ///
    /// Construct a ControlDescriptionItemInfoBase with the specified
    /// units code, multiplier code, and localized description code
    ///
    /// \param units_code
    /// \param multiplier_code
    /// \param localized_description_code
    ///
    ControlDescriptionItemInfoBase( uint8_t units_code,
                                    int8_t multiplier_code,
                                    uint16_t localized_description_code )
        : m_units_code( units_code )
        , m_multiplier_code( multiplier_code )
        , m_localized_description_code( localized_description_code )
    {}

    ///
    /// \brief ~ControlDescriptionItemInfoBase
    ///
    virtual ~ControlDescriptionItemInfoBase() {}

    ///
    /// \brief getItemSize
    ///
    /// \return uint16_t size of the encoded item value
    ///
    virtual uint16_t getItemSize() const = 0;

    ///
    /// \brief isBool
    /// \return true if the item represents a boolean value
    ///
    virtual bool isBool() const = 0;

    ///
    /// \brief isSigned
    /// \return true if the item encoding is something that can be negative
    ///
    virtual bool isSigned() const = 0;

    ///
    /// \brief isInteger
    /// \return true if the item encoding is an integer value
    ///
    virtual bool isInteger() const = 0;

    ///
    /// \brief isFloatingPoint
    /// \return true if the item encoding is a floating point value
    ///
    virtual bool isFloatingPoint() const = 0;

    ///
    /// \brief getValueAsBool
    /// \param value_pointer pointer to network byte order encoded item value
    /// \return bool representation of the unencoded item value
    ///
    virtual bool getValueAsBool( const void *value_pointer, ssize_t pos ) const = 0;

    ///
    /// \brief setValueFromBool
    /// \param value_pointer pointer to buffer to hold the network byte order encoded item value
    /// \param v bool representation of the unencoded item value
    /// \return
    ///
    virtual void setValueFromBool( void *value_pointer, ssize_t pos, bool v ) const = 0;

    ///
    /// \brief getDefaultAsBool
    /// \return bool representation of the unencoded default value
    ///
    virtual bool getDefaultAsBool() const = 0;

    ///
    /// \brief getValueAsFloat
    /// \param value_pointer pointer to network byte order encoded item value
    /// \return floating point representation of the unencoded item value
    ///
    virtual float getValueAsFloat( const void *value_pointer, ssize_t pos ) const = 0;

    ///
    /// \brief setValueFromFloat
    /// \param value_pointer pointer to buffer to hold network byee order encoded item value
    /// \param v floating point representation of the unencoded item value
    ///
    virtual void setValueFromFloat( void *value_pointer, ssize_t pos, float v ) const = 0;

    ///
    /// \brief getMinAsFloat
    /// \return floating point representation of the unencoded minimum value
    ///
    virtual float getMinAsFloat() const = 0;

    ///
    /// \brief getMaxAsFloat
    /// \return floating point representtion of the unencoded maxmimum value
    ///
    virtual float getMaxAsFloat() const = 0;

    ///
    /// \brief getDefaultAsFloat
    /// \return floating point representation of the unencoded default value
    ///
    virtual float getDefaultAsFloat() const = 0;

    ///
    /// \brief getStepAsFloat
    /// \return  floating point representation of the unencoded step value
    ///
    virtual float getStepAsFloat() const = 0;

    ///
    /// \brief getValueAsInt64
    /// \param value_pointer pointer to network byte order encoded item value
    /// \return int64_t representation of the unencoded item value
    ///
    virtual int64_t getValueAsInt64( const void *value_pointer, ssize_t pos ) const = 0;

    ///
    /// \brief setValueFromInt64
    /// \param value_pointer pointer to buffer to hold network byte order encoded item value
    /// \param v int64_t representation of the unencoded item value
    ///
    virtual void setValueFromInt64( void *value_pointer, ssize_t pos, int64_t v ) const = 0;

    ///
    /// \brief getMinAsInt64
    /// \return int64_t representation of the unencoded minimum value
    ///
    virtual int64_t getMinAsInt64() const = 0;

    ///
    /// \brief getMaxAsInt64
    /// \return int64_t representation of the unencoded maximum value
    ///
    virtual int64_t getMaxAsInt64() const = 0;

    ///
    /// \brief getDefaultAsInt64
    /// \return int64_t representation of the unencoded default value
    ///
    virtual int64_t getDefaultAsInt64() const = 0;

    ///
    /// \brief getStepAsInt64
    /// \return int64_t representation of the unencoded step value
    ///
    virtual int64_t getStepAsInt64() const = 0;

    ///
    /// \brief getValueAsUInt64
    /// \param value_pointer pointer to network byte order encoded item value
    /// \return uint64_t representation of the unencoded item value
    ///
    virtual uint64_t getValueAsUInt64( const void *value_pointer, ssize_t pos ) const = 0;

    ///
    /// \brief setValueFromUInt64
    /// \param value_pointer pointer to buffer to hold network byte order encoded item value
    /// \param v uint64_t representation of the unencoded item value
    ///
    virtual void setValueFromUInt64( void *value_pointer, ssize_t pos, uint64_t v ) const = 0;

    ///
    /// \brief getMinAsUInt64
    /// \return uint64_t representation of the unencoded minimum value
    ///
    virtual uint64_t getMinAsUInt64() const = 0;

    ///
    /// \brief getMaxAsUInt64
    /// \return uint64_t representation of the unencoded maximum value
    ///
    virtual uint64_t getMaxAsUInt64() const = 0;

    ///
    /// \brief getDefaultAsUInt64
    /// \return uint64_t representation of the unencoded default value
    ///
    virtual uint64_t getDefaultAsUInt64() const = 0;

    ///
    /// \brief getStepAsUInt64
    /// \return uint64_t representation of the unencoded step value
    ///
    virtual uint64_t getStepAsUInt64() const = 0;

    ///
    /// \brief getUnitsCode
    /// \return the IEEE Std 1722.1-2013 units code - See Clause 7.3.3
    ///
    uint8_t getUnitsCode() const
    {
        return m_units_code;
    }

    ///
    /// \brief getMultiplierCode
    /// \return the IEEE Std 1722.1-2013 multiplier code - See Clause 7.3.3
    ///
    int8_t getMultiplierCode() const
    {
        return m_multiplier_code;
    }

    ///
    /// \brief getLocalizedDescription
    /// \return the IEEE Std 1722.1-2013 localized description code - See Clause 7.3.6
    ///
    uint16_t getLocalizedDescription() const
    {
        return m_localized_description_code;
    }

private:
    uint8_t m_units_code;
    int8_t m_multiplier_code;
    uint16_t m_localized_description_code;
};

template <typename T>
class ControlDescriptionItemInfo : ControlDescriptionItemInfoBase
{
public:
    ControlDescriptionItemInfo(
            T min_value,
            T max_value,
            T default_value,
            T step_value,
            uint8_t units,
            int8_t multiplier_code=0,
            uint16_t localized_description=JDKSAVDECC_NO_STRING )
        : ControlDescriptionItemInfoBase( units, multiplier_code, localized_description )
        , m_min( min_value )
        , m_max( max_value )
        , m_default( default_value )
        , m_step( step_value )
    {}

    uint16_t getItemSize() const override
    {
        return uint16_t(sizeof(T));
    }

    bool isBool() const override
    {
        bool r=false;
        if( (getItemSize()==1)
                && !isSigned()
                && isInteger()
                && (m_min==0)
                && (m_max==255)
                && (m_step==255) )
        {
            r=true;
        }
        return r;
    }

    bool isSigned() const override
    {
        return EncodedControlItemTraits<T>::isSigned()
                || EncodedControlItemTraits<T>::isFloatingPoint();
    }

    bool isInteger() const override
    {
        return EncodedControlItemTraits<T>::isIntegral();
    }

    bool isFloatingPoint() const override
    {
        return EncodedControlItemTraits<T>::isFloatingPoint();
    }

    bool getValueAsBool( const void *value_pointer, ssize_t pos ) const override
    {
        bool r=false;
        if( getItemSize()==1 )
        {
            const uint8_t *p = reinterpret_cast<const uint8_t *>(value_pointer) + pos;
            if( *p != 0 )
            {
                r=true;
            }
        }
        return r;
    }

    void setValueFromBool( void *value_pointer, ssize_t pos, bool v ) const override
    {
        if( getItemSize()==1 )
        {
            uint8_t *p = reinterpret_cast<uint8_t *>(value_pointer) + pos;
            *p = (v==false) ? m_min : m_max;
        }
    }

    bool getDefaultAsBool() const override
    {
        bool r=false;
        if( m_default!=0 )
        {
            r=true;
        }
        return r;
    }

    float getValueAsFloat( const void *value_pointer, ssize_t pos ) const override
    {
        return EncodedControlItemTraits<T>::template decodeNetworkData<float>( value_pointer, pos, getMultiplierCode() );
    }

    void setValueFromFloat( void *value_pointer, ssize_t pos, float v ) const override
    {
        EncodedControlItemTraits<T>::template encodeNetworkData<float>( value_pointer, pos, v, getMultiplierCode() );
    }

    float getMinAsFloat() const override
    {
        return convertEncodedValueToDecoded<float>(m_min,getMultiplierCode());
    }

    float getMaxAsFloat() const override
    {
        return convertEncodedValueToDecoded<float>(m_max,getMultiplierCode());
    }

    float getDefaultAsFloat() const override
    {
        return convertEncodedValueToDecoded<float>(m_default,getMultiplierCode());
    }

    float getStepAsFloat() const override
    {
        return convertEncodedValueToDecoded<float>(m_step,getMultiplierCode());
    }

    int64_t getValueAsInt64( const void *value_pointer, ssize_t pos ) const override
    {
        return EncodedControlItemTraits<T>::template decodeNetworkData<int64_t>( value_pointer, pos, getMultiplierCode() );
    }

    void setValueFromInt64( void *value_pointer, ssize_t pos, int64_t v ) const override
    {
        EncodedControlItemTraits<T>::template encodeNetworkData<int64_t>( value_pointer, pos, v, getMultiplierCode() );
    }

    int64_t getMinAsInt64() const override
    {
        return convertEncodedValueToDecoded<int64_t>(m_min,getMultiplierCode());
    }

    int64_t getMaxAsInt64() const override
    {
        return convertEncodedValueToDecoded<int64_t>(m_max,getMultiplierCode());
    }

    int64_t getDefaultAsInt64() const override
    {
        return convertEncodedValueToDecoded<int64_t>(m_default,getMultiplierCode());
    }

    int64_t getStepAsInt64() const override
    {
        return convertEncodedValueToDecoded<int64_t>(m_step,getMultiplierCode());
    }

    uint64_t getValueAsUInt64( const void *value_pointer, ssize_t pos ) const override
    {
        return EncodedControlItemTraits<T>::template decodeNetworkData<uint64_t>( value_pointer, pos, getMultiplierCode() );

    }

    void setValueFromUInt64( void *value_pointer, ssize_t pos, uint64_t v ) const override
    {
        EncodedControlItemTraits<T>::template encodeNetworkData<uint64_t>( value_pointer, pos, v, getMultiplierCode() );
    }

    uint64_t getMinAsUInt64() const override
    {
        return convertEncodedValueToDecoded<uint64_t>(m_min,getMultiplierCode());
    }

    uint64_t getMaxAsUInt64() const override
    {
        return convertEncodedValueToDecoded<uint64_t>(m_max,getMultiplierCode());
    }

    uint64_t getDefaultAsUInt64() const override
    {
        return convertEncodedValueToDecoded<uint64_t>(m_default,getMultiplierCode());
    }

    uint64_t getStepAsUInt64() const override
    {
        return convertEncodedValueToDecoded<uint64_t>(m_step,getMultiplierCode());
    }

private:
    T m_min;
    T m_max;
    T m_default;
    T m_step;
};

class ControlDescription
{
public:
    ControlDescription();
    virtual ~ControlDescription();

    Eui64 getControlType() const;
    uint16_t getNumValues() const;
    uint16_t getControlValueType() const;

    ControlDescription &setLinear( Eui64 control_type );

    template <typename T>
    ControlDescription &addLinearItem( ControlDescriptionItemInfo<T> item );

    bool isLinearItem() const;

    template <typename T>
    ControlDescriptionItemInfo<T> getLinearItem( uint16_t item_number );

    template <typename T>
    ControlDescription &setSelector( Eui64 control_type,
                                     ControlDescriptionItemInfo<T> item
                                     );

    template <typename T>
    ControlDescription &addSelectorItem( T value );

    template <typename T>
    ControlDescriptionItemInfo<T> getSelectorItem( uint16_t item_number );

    bool isSelector() const;

    template <typename T>
    ControlDescription &setArray( Eui64 control_type,
                                  ControlDescriptionItemInfo<T> item,
                                  uint16_t num_items
                                  );

    bool isArray() const;
    template <typename T>
    ControlDescriptionItemInfo<T> getArrayInfo();

    ControlDescription &setUTF8( const char *s = "" );

    bool isUTF8() const;

    ControlDescription &setBodePlot( Eui64 control_type, jdksavdecc_values_bode_plot const &bode_details );
    bool isBodePlot() const;

    ControlDescription &setVendor( Eui64 control_type, Eui64 vendor_blob_type, uint16_t blob_size );
    bool isVendor() const;
    Eui64 getVendorBlobType() const;
    uint16_t getVendorBlobLength() const;

    void readFromDescriptor( FixedBuffer const &buf, uint16_t start_pos );
    void storeToDescriptor( FixedBuffer *dest ) const;

    jdksavdecc_descriptor_control m_descriptor_info;
    FixedBufferWithSize<404> m_descriptor_value_details;
};


}
