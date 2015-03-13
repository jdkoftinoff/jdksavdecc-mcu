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

    virtual void talkerConnected( ACMPTalkerHandlerBase const *talker_handler,
                                  ACMPTalkerStreamInfo const &stream_info,
                                  ACMPTalkerListenerPair const &listener_pair ) = 0;

    virtual void talkerDisconnected( ACMPTalkerHandlerBase const *talker_handler,
                                     ACMPTalkerStreamInfo const &stream_info,
                                     ACMPTalkerListenerPair const &listener_pair ) = 0;

};

///
/// See IEEE Std 1722.1-2013 Clause 8.2.2.2.3 ListenerPair
///
struct ACMPTalkerListenerPair
{
    ACMPTalkerListenerPair()
        : m_listener_unique_id(0)
    {}

    Eui64 m_listener_entity_id;
    uint16_t m_listener_unique_id;
};

///
/// See IEEE Std 1722.1-2013 Clause 8.2.2.2.4 TalkerStreamInfo
///
struct ACMPTalkerStreamInfo
{
    ACMPTalkerStreamInfo()
        : m_connection_count(0)
        , m_connected_listeners(nullptr)
        , m_stream_vlan_id(0)
        , m_max_connected_listeners(0)
    {}

    Eui64 m_stream_id;
    Eui48 m_stream_dest_mac;
    uint16_t m_connection_count;
    ACMPTalkerListenerPair *m_connected_listeners;
    uint16_t m_stream_vlan_id;

    uint16_t m_max_connected_listeners;

    void packListeners();

    uint16_t getMaxListenersPerTalker() const
    {
        return m_max_connected_listeners;
    }
};


class ACMPTalkerHandlerBase : public Handler
{
public:
    ACMPTalkerHandlerBase( ACMPTalkerStreamInfo *talker_stream_info, Entity *entity = 0 )
        : m_entity( entity )
        , m_state(STATE_WAITING)
        , m_talker_stream_info( talker_stream_info )
    {}

    virtual void tick( jdksavdecc_timestamp_in_milliseconds timestamp ) override;

    virtual bool receivedPDU( Frame &frame ) override;

    void setEntity( Entity *entity )
    {
        m_entity = entity;
    }

    Entity *getEntity()
    {
        return m_entity;
    }

    Entity const *getEntity() const
    {
        return m_entity;
    }

protected:
    Entity *m_entity;

    enum State
    {
        STATE_WAITING,
        STATE_CONNECT,
        STATE_DISCONNECT,
        STATE_GET_STATE,
        STATE_GET_CONNECTION
    } m_state;

    ACMPTalkerStreamInfo *m_talker_stream_info;
};

template <uint16_t MaxListenersPerTalker>
class ACMPTalkerHandler : public ACMPTalkerHandlerBase
{
public:
    ACMPTalkerHandler( Entity *entity = 0 )
     : ACMPTalkerHandlerBase( &m_stream_info_storage )
    {
        m_stream_info_storage.m_max_connected_listeners = MaxListenersPerTalker;
        m_stream_info_storage.m_connected_listeners = &m_listener_pairs_storage[0];
    }

 protected:

    ACMPTalkerStreamInfo m_stream_info_storage;
    ACMPTalkerListenerPair m_listener_pairs_storage[MaxListenersPerTalker];
};


class ACMPTalkerGroupHandlerBase : public Handler
{
public:
    ACMPTalkerGroupHandlerBase( Entity *entity );

    virtual void tick( jdksavdecc_timestamp_in_milliseconds timestamp ) override;

    virtual bool receivedPDU( Frame &frame ) override;

    virtual ACMPTalkerHandlerBase *getTalkerHandler( uint16_t talker_unique_id ) = 0;

    virtual ACMPTalkerHandlerBase const *getTalkerHandler( uint16_t talker_unique_id ) const = 0;

    virtual uint16_t getTalkerStreamSourceCount() const = 0;
};

template <uint16_t TalkerStreamSourceCount,
          uint16_t MaxListenersPerTalker>
class ACMPTalkerGroupHandler : public ACMPTalkerGroupHandlerBase
{
public:
    ACMPTalkerGroupHandler( Entity *entity )
        : ACMPTalkerGroupHandlerBase( entity )
    {
        for( uint16_t i=0; i<TalkerStreamSourceCount; ++i )
        {
            m_talkers_storage[i].setEntity(entity);
        }
    }

    virtual ACMPTalkerHandlerBase *getTalkerHandler( uint16_t talker_unique_id ) override
    {
        return &m_talkers_storage[talker_unique_id];
    }

    virtual ACMPTalkerHandlerBase const *getTalkerHandler( uint16_t talker_unique_id ) const override
    {
        return &m_talkers_storage[talker_unique_id];
    }

    virtual uint16_t getTalkerStreamSourceCount() const override
    {
        return TalkerStreamSourceCount;
    }

private:
    ACMPTalkerHandler<MaxListenersPerTalker> m_talkers_storage[TalkerStreamSourceCount];
};

}
