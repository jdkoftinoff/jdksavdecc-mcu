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

class ACMPTalkerEvents;
struct ACMPTalkerListenerPair;
struct ACMPTalkerStreamInfo;
class ACMPTalkerHandlerBase;

class ACMPTalkerEvents
{
  public:
    virtual ~ACMPTalkerEvents() {}

    ///
    /// \brief talkerConnected
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
    virtual void talkerConnected( Entity *entity,
                                  uint16_t talker_unique_id,
                                  ACMPTalkerHandlerBase const *talker_handler,
                                  ACMPTalkerStreamInfo const &stream_info,
                                  ACMPTalkerListenerPair const &listener_pair ) = 0;

    ///
    /// \brief talkerDisconnected
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
    virtual void talkerDisconnected( Entity *entity,
                                     uint16_t talker_unique_id,
                                     ACMPTalkerHandlerBase const *talker_handler,
                                     ACMPTalkerStreamInfo const &stream_info,
                                     ACMPTalkerListenerPair const &listener_pair ) = 0;
};

///
/// See IEEE Std 1722.1-2013 Clause 8.2.2.2.3 ListenerPair
///
struct ACMPTalkerListenerPair
{
    ACMPTalkerListenerPair() : m_listener_unique_id( 0 ) {}

    Eui64 m_listener_entity_id;
    uint16_t m_listener_unique_id;
};

///
/// \brief The ACMPTalkerHandlerBase class
///
/// Manages a single Talker unique_id state machine
///
class ACMPTalkerHandlerBase
{
  public:
    ///
    /// \brief ACMPTalkerHandlerBase
    ///
    /// See IEEE Std 1722.1-2013, "Clause 8.2.2.2.4 TalkerStreamInfo" and
    /// IEEE Std 1722.1-2013 "Clause 8.2.2.6 ACMP Talker State Machine"
    ///
    /// \param listener_pair_storage Pointer to an array of
    /// ACMPTalkerListenerPair objects,
    ///        one for each listener
    ///
    /// \param max_connected_listeners Size of the array in count of objects
    ///
    ACMPTalkerHandlerBase( ACMPTalkerListenerPair *listener_pair_storage, uint16_t max_connected_listeners )
        : m_connection_count( 0 )
        , m_connected_listeners( listener_pair_storage )
        , m_stream_vlan_id( 0 )
        , m_max_connected_listeners( max_connected_listeners )
        , m_state( STATE_WAITING )
    {
    }

    virtual ~ACMPTalkerHandlerBase();

    ///
    /// \brief tick
    ///
    /// \param entity
    /// \param unique_id
    /// \param eventTarget
    /// \param timestamp
    ///
    virtual void
        tick( Entity *entity, uint16_t unique_id, ACMPTalkerEvents *eventTarget, jdksavdecc_timestamp_in_milliseconds timestamp );

    ///
    /// \brief receivedACMPDU
    /// \param entity
    /// \param unique_id
    /// \param eventTarget
    /// \param frame
    /// \return
    ///
    virtual uint8_t receivedACMPDU( Entity *entity, uint16_t unique_id, ACMPTalkerEvents *eventTarget, Frame &frame );

    ///
    /// \brief packListeners
    ///
    void packListeners();

    Eui64 m_stream_id;
    Eui48 m_stream_dest_mac;
    uint16_t m_connection_count;
    ACMPTalkerListenerPair *m_connected_listeners;
    uint16_t m_stream_vlan_id;

    uint16_t m_max_connected_listeners;

    enum State
    {
        STATE_WAITING,
        STATE_CONNECT,
        STATE_DISCONNECT,
        STATE_GET_STATE,
        STATE_GET_CONNECTION
    } m_state;
};

template <uint16_t MaxListenersPerTalker>
class ACMPTalkerHandler : public ACMPTalkerHandlerBase
{
  public:
    ACMPTalkerHandler() : ACMPTalkerHandlerBase( &m_listener_pairs_storage[0], MaxListenersPerTalker ) {}

  protected:
    ACMPTalkerListenerPair m_listener_pairs_storage[MaxListenersPerTalker];
};

class ACMPTalkerGroupHandlerBase
{
  public:
    ACMPTalkerGroupHandlerBase( Entity *entity, ACMPTalkerEvents *event_target )
        : m_entity( entity ), m_event_target( event_target )
    {
    }

    virtual void tick( jdksavdecc_timestamp_in_milliseconds timestamp );

    virtual uint8_t receivedACMPDU( RawSocket *incoming_socket,
                                    const jdksavdecc_acmpdu &acmpdu,
                                    Frame &frame );

    virtual ACMPTalkerHandlerBase *getTalkerHandler( uint16_t talker_unique_id ) = 0;

    virtual ACMPTalkerHandlerBase const *getTalkerHandler( uint16_t talker_unique_id ) const = 0;

    virtual uint16_t getTalkerStreamSourceCount() const = 0;

  protected:
    Entity *m_entity;
    ACMPTalkerEvents *m_event_target;
};

template <uint16_t TalkerStreamSourceCount, uint16_t MaxListenersPerTalker>
class ACMPTalkerGroupHandler : public ACMPTalkerGroupHandlerBase
{
  public:
    ACMPTalkerGroupHandler( Entity *entity, ACMPTalkerEvents *event_target ) : ACMPTalkerGroupHandlerBase( entity, event_target ) {}

    virtual ACMPTalkerHandlerBase *getTalkerHandler( uint16_t talker_unique_id ) override
    {
        return &m_talkers_storage[talker_unique_id];
    }

    virtual ACMPTalkerHandlerBase const *getTalkerHandler( uint16_t talker_unique_id ) const override
    {
        return &m_talkers_storage[talker_unique_id];
    }

    virtual uint16_t getTalkerStreamSourceCount() const override { return TalkerStreamSourceCount; }

  private:
    ACMPTalkerHandler<MaxListenersPerTalker> m_talkers_storage[TalkerStreamSourceCount];
};
}
