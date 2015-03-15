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
#include "JDKSAvdeccMCU/Handler.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief The HandlerGroup class
///
/// The HandlerGroupBase class maintains a list of Handler pointers
/// Dispatches Tick() and ReceivedPDU() calls to all Handlers
/// It does not contain the storage of the handlers
/// No bounds checking is done
///
class HandlerGroup : public Handler
{
  protected:
    uint16_t m_num_items;
    uint16_t m_max_items;
    Handler **m_item;
    uint32_t m_rx_count;
    uint32_t m_handled_count;
    Frame *m_frame;

  public:
    ///
    /// \brief HandlerGroup construct a HandlerGroup object
    /// \param item_storage pointer to array of pointers to Handler objects
    /// \param max_items maximum number of pointers that the array can hold
    ///
    HandlerGroup( Frame *frame, Handler **item_storage, uint16_t max_items );

    ///
    /// \brief add Add a handler to the list
    /// \param v pointer to a Handler object
    /// \return true on success, false if there is no room
    ///
    bool add( Handler *v );

    ///
    /// \brief isFull test if the handler list is full or would be given
    /// additional items
    /// \param additional number of items
    /// \return true if it is or would be full
    ///
    bool isFull( uint16_t additional = 0 ) const { return ( m_num_items + additional ) >= m_max_items; }

    ///
    /// \brief canPut test if the handler list has room for additional items
    /// \param additional count of additional items
    /// \return true if there is room
    ///
    bool canPut( uint16_t additional = 1 ) const { return ( m_num_items + additional ) <= m_max_items; }

    ///
    /// \brief getRxCount get the count of received packets
    /// \return count
    ///
    uint32_t getRxCount() const { return m_rx_count; }

    ///
    /// \brief getHandledCount get the count of handled reeeived packets
    /// \return count
    ///
    uint32_t getHandledCount() const { return m_handled_count; }

    ///
    /// \brief tick
    /// Send Tick() messages to all encapsulated Handlers
    /// and poll incoming network for PDU's and dispatch them
    /// \param timestamp
    ///
    virtual void tick( jdksavdecc_timestamp_in_milliseconds timestamp ) override;

    ///
    /// \brief receivedPDU Notification of received raw PDU.
    /// Send ReceivedPDU message to each handler until one returns true.
    /// \param incoming_socket The socket that the frame was received on
    /// \param frame reference to received Frame object which is mutable
    /// \return true if the message was handled
    ///
    virtual bool receivedPDU( RawSocket *incoming_socket, Frame &frame ) override;
};

///
/// \brief The HandlerGroupWithSize class
///
/// HandlerGroup is a HandlerGroupBase and contains
/// the storage of the contained Handler pointers.
/// The HandlerGroup is templatelized by the MaxItem count.
///
template <uint16_t MaxItems>
class HandlerGroupWithSize : public HandlerGroup
{
  private:
    Handler *m_item_storage[MaxItems];

  public:
    HandlerGroupWithSize( Frame *frame ) : HandlerGroup( frame, m_item_storage, MaxItems ) {}
};
}
