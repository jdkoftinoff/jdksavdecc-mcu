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
#include "JDKSAvdeccMCU_AppMessage.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief The AppMessageHandler class
///
/// Dispatch received AppMessages to an appropriate
/// handler
///
class AppMessageHandler
{
  public:
    AppMessageHandler() {}
    virtual ~AppMessageHandler() {}

    ///
    /// \brief onAppNop
    ///
    /// Called when the APPDU is NOP
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppNop( AppMessage const &msg ) = 0;

    ///
    /// \brief onAppEntityIdRequest
    ///
    /// Called when the APPDU is ENTITY_ID_REQUEST
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppEntityIdRequest( AppMessage const &msg ) = 0;

    ///
    /// \brief onAppEntityIdResponse
    ///
    /// Called when the APPDU is ENTITY_ID_RESPONSE
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppEntityIdResponse( AppMessage const &msg ) = 0;

    ///
    /// \brief onAppLinkUp
    ///
    /// Called when the APPDU is LINK_UP
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppLinkUp( AppMessage const &msg ) = 0;

    ///
    /// \brief onAppLinkDown
    ///
    /// Called when the APPDU is LINK_DOWN
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppLinkDown( AppMessage const &msg ) = 0;

    ///
    /// \brief onAppAvdeccFromAps
    ///
    /// Called when the APPDU is AVDECC_FROM_APS
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppAvdeccFromAps( AppMessage const &msg ) = 0;

    ///
    /// \brief onAppAvdeccFromApc
    ///
    /// Called when the APPDU is AVDECC_TO_APC
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppAvdeccFromApc( AppMessage const &msg ) = 0;


    ///
    /// \brief onAppVendor
    ///
    /// Called when the APPDU is VENDOR
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppVendor( AppMessage const &msg ) = 0;

    ///
    /// \brief onUnknown
    ///
    /// Called when the APPDU message type or version is unknown
    ///
    /// \param msg The AppMessage
    ///
    virtual void onAppUnknown( AppMessage const &msg ) = 0;
};
}
