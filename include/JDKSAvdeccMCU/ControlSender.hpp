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
#include "JDKSAvdeccMCU/RawSocket.hpp"
#include "JDKSAvdeccMCU/Frame.hpp"
#include "JDKSAvdeccMCU/Handler.hpp"
#include "JDKSAvdeccMCU/Helpers.hpp"
#include "JDKSAvdeccMCU/Entity.hpp"
#include "JDKSAvdeccMCU/ControllerEntity.hpp"
#include "JDKSAvdeccMCU/ControlValueHolder.hpp"

namespace JDKSAvdeccMCU
{

class ControlSender : public Handler
{
  public:
    /// Construct the SetControlSender object
    ControlSender( ControllerEntity &controller_entity,
                   Eui64 const &target_entity_id,
                   Eui48 const &target_mac_address,
                   uint16_t target_descriptor_index,
                   jdksavdecc_timestamp_in_milliseconds update_rate_in_millis,
                   ControlValueHolder *holder );

    /// Send the SET_CONTROL message if it is time to
    virtual void
        tick( jdksavdecc_timestamp_in_milliseconds time_in_millis ) override;

    /// Handle incoming PDU
    virtual bool receivedPDU( RawSocket *incoming_socket,
                              Frame &frame ) override;

    /// Formulate the AECPU set control for and send it. Returns true if the
    /// message was
    /// actually sent.
    bool sendSetControl( bool wait_for_ack = false );

    ControllerEntity &getControllerEntity() { return m_controller_entity; }

    RawSocket &getRawSocket() { return m_controller_entity.getRawSocket(); }

    Eui64 const &getEntityID() const
    {
        return m_controller_entity.getEntityID();
    }

    Eui64 const &getTargetEntityID() const { return m_target_entity_id; }

    Eui48 const &getTargetMACAddress() const { return m_target_mac_address; }

    ControlValueHolder *getHolder() { return m_holder; }

    ControlValueHolder const *getHolder() const { return m_holder; }

  protected:
    ControllerEntity &m_controller_entity;
    Eui64 m_target_entity_id;
    Eui48 m_target_mac_address;
    uint16_t m_target_descriptor_index;
    jdksavdecc_timestamp_in_milliseconds m_update_rate_in_millis;
    jdksavdecc_timestamp_in_milliseconds m_last_send_time_in_millis;
    ControlValueHolder *m_holder;
};
}
