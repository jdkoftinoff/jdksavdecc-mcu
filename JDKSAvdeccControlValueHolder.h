#pragma once

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccNetIO.h"
#include "JDKSAvdeccFrame.h"
#include "JDKSAvdeccHandler.h"

namespace JDKSAvdecc {

class ControlValueHolder : public Handler {
public:
    ControlValueHolder( uint8_t value_length );

    bool IsDirty() const { return m_dirty; }
    void ClearDirty() { m_dirty=false; }

    uint8_t GetValueLength() const { return m_value_length; }

    uint8_t GetValueOctet() const;
    uint16_t GetValueDoublet() const;
    uint32_t GetValueQuadlet() const;

    uint32_t GetValue() const;

    void SetValueOctet( uint8_t v );
    void SetValueDoublet( uint16_t v );
    void SetValueQuadlet( uint32_t v );

    void SetValue( uint32_t v );
    void SetValue( uint8_t const *v );

protected:
    uint8_t m_value[4];
    bool m_dirty;
    uint8_t m_value_length;
};

}
