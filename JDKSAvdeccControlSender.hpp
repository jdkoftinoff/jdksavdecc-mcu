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

#include "JDKSAvdeccWorld.hpp"
#include "JDKSAvdeccNetIO.hpp"
#include "JDKSAvdeccFrame.hpp"
#include "JDKSAvdeccHandler.hpp"
#include "JDKSAvdeccHelpers.hpp"

namespace JDKSAvdecc {

class ControlSender : public Handler {
public:

    /// Construct the SetControlSender object
    ControlSender(jdksavdecc_eui64 const &entity_id,
               jdksavdecc_eui64 const &target_entity_id,
               jdksavdecc_eui48 const &target_mac_address,
               uint16_t &sequence_id,
               uint16_t target_descriptor_index,
               uint16_t value_length,
               uint32_t update_rate_in_millis);

    /// Send the SET_CONTROL message if it is time to
    virtual void Tick( uint32_t time_in_millis );

    /// Handle incoming PDU
    virtual bool ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len );

    /// Set a one byte value. If it actually changed, then force Tick to send ASAP
    void SetValueOctet( uint8_t val );

    /// Set a doublet value. If it actually changed, then force Tick to send ASAP
    void SetValueDoublet( uint16_t val );

    /// Set a quadlet value. If it actually changed, then force Tick to send ASAP
    void SetValueQuadlet( uint32_t val );


    /// Formulate the ADPDU and send it
    void SendSetControl();


    jdksavdecc_eui64 const &GetEntityID() const { return m_entity_id; }
    jdksavdecc_eui64 const &GetTargetEntityID() const { return m_target_entity_id; }
    jdksavdecc_eui48 const &GetTargetMACAddress() const { return m_target_mac_address; }
    uint16_t GetSequenceID() const { return m_sequence_id; }


protected:
    jdksavdecc_eui64 m_entity_id;
    jdksavdecc_eui64 m_target_entity_id;
    jdksavdecc_eui48 m_target_mac_address;
    uint16_t &m_sequence_id;
    uint16_t m_target_descriptor_index;
    uint16_t m_value_length;
    uint8_t m_value[4];
    uint32_t m_update_rate_in_millis;
    uint32_t m_next_send_time_millis;
};

}
