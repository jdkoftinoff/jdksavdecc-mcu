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

template <typename T>
struct ControlDescriptionItemInfo
{
    T m_min;
    T m_max;
    T m_default;
    T m_step;
    uint8_t m_units;
    int8_t m_multiplier;
    uint16_t m_localized_description;
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
