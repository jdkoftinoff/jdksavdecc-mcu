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
#include "JDKSAvdeccMCU_Handler.hpp"
#include "JDKSAvdeccMCU_Helpers.hpp"
#include "JDKSAvdeccMCU_Entity.hpp"
#include "JDKSAvdeccMCU_ControllerEntity.hpp"

namespace JDKSAvdeccMCU
{

class ControlSender : public Handler
{
  public:
    /// Construct the SetControlSender object
    ControlSender( ControllerEntity &controller_entity,
                   jdksavdecc_eui64 const &target_entity_id,
                   jdksavdecc_eui48 const &target_mac_address,
                   uint16_t target_descriptor_index,
                   jdksavdecc_timestamp_in_milliseconds update_rate_in_millis,
                   uint16_t value_length,
                   uint8_t *value_storage
                   );

    /// Send the SET_CONTROL message if it is time to
    virtual void tick();

    /// Handle incoming PDU
    virtual bool receivedPDU( Frame &frame );

    /// Set a one byte value. If it actually changed, then force Tick to send
    /// ASAP
    void setValueOctet( uint8_t val );

    /// Set a doublet value. If it actually changed, then force Tick to send
    /// ASAP
    void setValueDoublet( uint16_t val );

    /// Set a quadlet value. If it actually changed, then force Tick to send
    /// ASAP
    void setValueQuadlet( uint32_t val );

    /// Formulate the ADPDU and send it
    void sendSetControl();

    ControllerEntity &getControllerEntity() { return m_controller_entity; }
    RawSocket &getRawSocket() { return m_controller_entity.getRawSocket(); }

    jdksavdecc_eui64 const &getEntityID() const { return m_controller_entity.getEntityID(); }

    jdksavdecc_eui64 const &getTargetEntityID() const
    {
        return m_target_entity_id;
    }
    jdksavdecc_eui48 const &getTargetMACAddress() const
    {
        return m_target_mac_address;
    }

  protected:
    ControllerEntity &m_controller_entity;
    jdksavdecc_eui64 m_target_entity_id;
    jdksavdecc_eui48 m_target_mac_address;
    uint16_t m_target_descriptor_index;
    jdksavdecc_timestamp_in_milliseconds m_update_rate_in_millis;
    jdksavdecc_timestamp_in_milliseconds m_last_send_time_in_millis;
    uint16_t m_value_length;
    uint8_t *m_value;
};

template <size_t StorageSize>
class ControlSenderWithStorage : public ControlSender
{
    ControlSenderWithStorage( ControllerEntity &controller_entity,
                              jdksavdecc_eui64 const &target_entity_id,
                              jdksavdecc_eui48 const &target_mac_address,
                              uint16_t target_descriptor_index,
                              jdksavdecc_timestamp_in_milliseconds update_rate_in_millis )
        : ControlSender(
              controller_entity,
              target_entity_id,
              target_mac_address,
              target_descriptor_index,
              update_rate_in_millis,
              StorageSize,
              m_value_storage )
    {
        bzero(m_value_storage,StorageSize);
    }
private:
    uint8_t m_value_storage[StorageSize];
};

}
