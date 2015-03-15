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
#include "JDKSAvdeccMCU/Helpers.hpp"
#include "JDKSAvdeccMCU/Frame.hpp"

namespace JDKSAvdeccMCU
{

struct RegisteredController
{
    /// Controller's entity_id
    /// The entity id is FF:FF:FF:FF:FF:FF:FF:FF If the the slot is not
    /// in use
    Eui64 m_entity_id;

    /// Controller's MAC address
    Eui48 m_mac_address;
};

class RegisteredControllers
{
  public:
    virtual uint16_t getControllerCount() const = 0;
    virtual RegisteredController *getController( uint16_t i ) = 0;
    virtual RegisteredController const *getController( uint16_t i ) const = 0;
    virtual bool findController( Eui64 entity_id ) const = 0;
    virtual bool addController( Eui64 entity_id, Eui48 mac_address ) = 0;
    virtual void removeController( Eui64 entity_id ) = 0;
};

template <uint16_t MaxControllers>
class RegisteredControllersStorage : public RegisteredControllers
{
  public:
    RegisteredControllersStorage() : m_num_controllers( 0 ) {}

    virtual uint16_t getControllerCount() const override { return m_num_controllers; }

    virtual RegisteredController *getController( uint16_t i ) override { return &m_controller[i]; }
    virtual RegisteredController const *getController( uint16_t i ) const override { return &m_controller[i]; }
    virtual bool findController( Eui64 entity_id ) const override
    {
        bool r = false;
        for ( uint16_t i = 0; i < m_num_controllers; ++i )
        {
            if ( m_controller[i].m_entity_id == entity_id )
            {
                r = true;
            }
        }
        return r;
    }

    virtual bool addController( Eui64 entity_id, Eui48 mac_address ) override
    {
        bool r = false;

        // Don't add the controller more than once
        if ( findController( entity_id ) )
        {
            // already added
            r = true;
        }
        else
        {
            if ( m_num_controllers < MaxControllers )
            {
                m_controller[m_num_controllers].m_entity_id = entity_id;
                m_controller[m_num_controllers].m_mac_address = mac_address;
                r = true;
            }
        }
        return r;
    }
    virtual void removeController( Eui64 entity_id ) override
    {
        for ( uint16_t i = 0; i < m_num_controllers; ++i )
        {
            // find the controller
            if ( m_controller[i].m_entity_id == entity_id )
            {
                // found it; is it the last one in the list?
                if ( i == m_num_controllers - 1 )
                {
                    // yes, just erase it
                    m_controller[i].m_entity_id.clear();
                    m_controller[i].m_mac_address.clear();
                }
                else
                {
                    // it is not the last one in the list, so swap it with the last one in the list
                    using namespace std;
                    swap( m_controller[i], m_controller[m_num_controllers - 1] );
                }
                m_num_controllers--;
                break;
            }
        }
    }

  private:
    uint16_t m_num_controllers;
    RegisteredController m_controller[MaxControllers];
};
}
