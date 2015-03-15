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
#include "JDKSAvdeccMCU/RawSocket.hpp"

#include "JDKSAvdeccMCU/Helpers.hpp"
#include "JDKSAvdeccMCU/Frame.hpp"

#include "JDKSAvdeccMCU/Handler.hpp"
#include "JDKSAvdeccMCU/ADPManager.hpp"
#include "JDKSAvdeccMCU/Entity.hpp"
#include "JDKSAvdeccMCU/EntityState.hpp"

namespace JDKSAvdeccMCU
{

class ACMPListenerEvents;

struct ACMPListenerStreamInfo;
class ACMPListenerHandlerBase;

class ACMPListenerEvents
{
  public:
    virtual ~ACMPListenerEvents() {}

    ///
    /// \brief listenerConnected
    ///
    /// Notification that ACMP Talker State Machine has successfully connected a
    /// stream
    ///
    /// \param entity
    /// \param talker_unique_id
    /// \param talker_handler
    /// \param stream_info
    /// \param listener_pair
    ///
    virtual void listenerConnected( Entity *entity, uint16_t listener_unique_id, ACMPListenerHandlerBase const *listener_handler )
        = 0;

    ///
    /// \brief listenerDisconnected
    ///
    /// Notification that ACMP Talker State Machine has successfully
    /// disconnected a stream
    ///
    /// \param entity
    /// \param talker_unique_id
    /// \param talker_handler
    /// \param stream_info
    /// \param listener_pair
    ///
    virtual void
        listenerDisconnected( Entity *entity, uint16_t listener_unique_id, ACMPListenerHandlerBase const *listener_handler ) = 0;
};

///
/// \brief The ACMPListenerHandlerBase class
///
/// Manages a single Listener unique_id state machine
///
class ACMPListenerHandler
{
  public:
    ///
    /// \brief ACMPListenerHandler
    ///
    /// See IEEE Std 1722.1-2013, "Clause 8.2.2.2.4 TalkerStreamInfo" and
    /// IEEE Std 1722.1-2013 "Clause 8.2.2.6 ACMP Talker State Machine"
    ///
    /// \param listener_pair_storage Pointer to an array of
    /// ACMPListenerListenerPair objects,
    ///        one for each listener
    ///
    /// \param max_connected_listeners Size of the array in count of objects
    ///
    ACMPListenerHandler() {}

    virtual ~ACMPListenerHandler();

    ///
    /// \brief tick
    ///
    /// \param entity
    /// \param unique_id
    /// \param eventTarget
    /// \param timestamp
    ///
    virtual void
        tick( Entity *entity, uint16_t unique_id, ACMPListenerEvents *eventTarget, jdksavdecc_timestamp_in_milliseconds timestamp );

    ///
    /// \brief receivedACMPDU
    /// \param entity
    /// \param unique_id
    /// \param eventTarget
    /// \param frame
    /// \return
    ///
    virtual uint8_t receivedACMPDU( Entity *entity, uint16_t unique_id, ACMPListenerEvents *eventTarget, Frame &frame );
};

class ACMPListenerGroupHandlerBase
{
  public:
    ACMPListenerGroupHandlerBase( Entity *entity, ACMPListenerEvents *event_target )
        : m_entity( entity ), m_event_target( event_target )
    {
    }

    virtual void tick( jdksavdecc_timestamp_in_milliseconds timestamp );

    virtual uint8_t receivedACMPDU( const jdksavdecc_acmpdu &acmpdu, Frame &frame );

    virtual ACMPListenerHandlerBase *getTalkerHandler( uint16_t talker_unique_id ) = 0;

    virtual ACMPListenerHandlerBase const *getTalkerHandler( uint16_t talker_unique_id ) const = 0;

    virtual uint16_t getListenerStreamSinkCount() const = 0;

  protected:
    Entity *m_entity;
    ACMPListenerEvents *m_event_target;
};

template <uint16_t ListenerStreamSinkCount>
class ACMPListenerGroupHandler : public ACMPListenerGroupHandlerBase
{
  public:
    ACMPListenerGroupHandler( Entity *entity, ACMPListenerEvents *event_target )
        : ACMPListenerGroupHandlerBase( entity, event_target )
    {
    }

    virtual ACMPListenerHandlerBase *getTalkerHandler( uint16_t listener_unique_id ) override
    {
        return &m_listener_storage[listener_unique_id];
    }

    virtual ACMPListenerHandlerBase const *getTalkerHandler( uint16_t listener_unique_id ) const override
    {
        return &m_listener_storage[listener_unique_id];
    }

    virtual uint16_t getListenerStreamSinkCount() const override { return ListenerStreamSinkCount; }

  private:
    ACMPListenerHandler m_listener_storage[ListenerStreamSinkCount];
};
}
