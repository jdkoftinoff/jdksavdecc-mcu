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

namespace JDKSAvdeccMCU
{


template <typename EncodedType, typename UnencodedType>
void encodeControlValue( EncodedType *encoded_value, UnencodedType unencoded_value, int8_t multiplier_code );

template <typename EncodedType, typename UnencodedType>
void decodeControlValue( UnencodedType *decoded_value, EncodedType encoded_value, int8_t multiplier_code );

template < typename U >
struct EncodedControlItemTraits
{
};

template <>
struct EncodedControlItemTraits<int8_t>
{
    static bool is_signed() { return true; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<uint8_t>
{
    static bool is_signed() { return false; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<int16_t>
{
    static bool is_signed() { return true; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<uint16_t>
{
    static bool is_signed() { return false; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<int32_t>
{
    static bool is_signed() { return true; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<uint32_t>
{
    static bool is_signed() { return false; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<int64_t>
{
    static bool is_signed() { return true; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<uint64_t>
{
    static bool is_signed() { return false; }
    static bool is_floating_point() { return false; }
    static bool is_integral() { return true; }
};

template <>
struct EncodedControlItemTraits<float>
{
    static bool is_signed() { return true; }
    static bool is_floating_point() { return true; }
    static bool is_integral() { return false; }
};

template <>
struct EncodedControlItemTraits<double>
{
    static bool is_signed() { return true; }
    static bool is_floating_point() { return true; }
    static bool is_integral() { return false; }
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
    /// \brief getValueAsFloat
    /// \param value_pointer pointer to network byte order encoded item value
    /// \return floating point representation of the unencoded item value
    ///
    virtual float getValueAsFloat( const void *value_pointer ) = 0;

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
    virtual int64_t getValueAsInt64( const void *value_pointer ) = 0;

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
    virtual uint64_t getValueAsUInt64( const void *value_pointer ) = 0;

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
private:

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

    virtual uint16_t getItemSize() const override
    {
        return uint16_t(sizeof(T));
    }

    virtual bool isBool() const override
    {
        bool r=false;
        if( (getItemSize()==1) && !isSigned() && isInteger() && (m_min==0) && (m_max==255) && (m_step==255) )
        {
            r=true;
        }
        return r;
    }

    virtual bool isSigned() const override
    {
        return EncodedControlItemTraits<T>::is_signed() || EncodedControlItemTraits<T>::is_floating_point();
    }

    virtual bool isInteger() const override
    {
        return EncodedControlItemTraits<T>::is_integral();
    }

    virtual bool isFloatingPoint() const
    {
        return EncodedControlItemTraits<T>::is_floating_point();
    }

    virtual float getMinAsFloat() const override
    {
        float r;
        decodeControlValue(&r,m_min,getMultiplierCode());
        return r;
    }

    virtual float getMaxAsFloat() const override
    {
        float r;
        decodeControlValue(&r,m_max,getMultiplierCode());
        return r;
    }

    virtual float getDefaultAsFloat() const override
    {
        float r;
        decodeControlValue(&r,m_default,getMultiplierCode());
        return r;
    }

    virtual float getStepAsFloat() const override
    {
        float r;
        decodeControlValue(&r,m_step,getMultiplierCode());
        return r;
    }

    virtual int64_t getMinAsInt64() const override
    {
        int64_t r;
        decodeControlValue(&r,m_min,getMultiplierCode());
        return r;
    }

    virtual int64_t getMaxAsInt64() const override
    {
        int64_t r;
        decodeControlValue(&r,m_max,getMultiplierCode());
        return r;
    }

    virtual int64_t getDefaultAsInt64() const override
    {
        int64_t r;
        decodeControlValue(&r,m_default,getMultiplierCode());
        return r;
    }

    virtual int64_t getStepAsInt64() const override
    {
        int64_t r;
        decodeControlValue(&r,m_step,getMultiplierCode());
        return r;
    }


    virtual uint64_t getMinAsUInt64() const override
    {
        uint64_t r;
        decodeControlValue(&r,m_min,getMultiplierCode());
        return r;
    }

    virtual uint64_t getMaxAsUInt64() const override
    {
        int64_t r;
        decodeControlValue(&r,m_max,getMultiplierCode());
        return r;
    }

    virtual uint64_t getDefaultAsUInt64() const override
    {
        int64_t r;
        decodeControlValue(&r,m_default,getMultiplierCode());
        return r;
    }

    virtual uint64_t getStepAsUInt64() const override
    {
        int64_t r;
        decodeControlValue(&r,m_step,getMultiplierCode());
        return r;
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
