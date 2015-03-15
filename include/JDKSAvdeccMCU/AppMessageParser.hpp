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
#include "JDKSAvdeccMCU/AppMessage.hpp"
#include "JDKSAvdeccMCU/AppMessageHandler.hpp"

namespace JDKSAvdeccMCU
{

///
/// \brief The AppMessageParser class
///
/// Consumes bytes one at a time and parses
/// AppMessages from the byte stream
///
class AppMessageParser
{
  public:
    ///
    /// \brief AppMessageParser
    /// Construct an AppMessageParser object
    ///
    AppMessageParser( AppMessageHandler &handler ) : m_octets_left_in_payload( 0 ), m_error_count( 0 ), m_handler( handler ) {}

    ///
    /// \brief clear
    ///
    /// Clear the current header parsing state and error count
    ///
    void clear()
    {
        m_header_buffer.setLength( 0 );
        m_error_count = 0;
        m_octets_left_in_payload = 0;
    }

    ///
    /// \brief parse parses one octet from a TCP stream
    /// and dispatch to an AppMessageHandler
    ///
    /// \param octet The incoming octet
    ///
    /// \return 0 on success, -1 on error
    ///
    int parse( uint8_t octet );

    ///
    /// \brief getErrorCount get the current error count
    /// \return error count
    ///
    size_t getErrorCount() const { return m_error_count; }

    ///
    /// \brief max_appdu_message_size The maximum size of an APPDU message
    /// including headers
    ///
    static const int max_appdu_message_size = JDKSAVDECC_APPDU_HEADER_LEN + JDKSAVDECC_APPDU_MAX_PAYLOAD_LENGTH;

  protected:
    ///
    /// \brief dispatchMsg
    ///
    /// Dispatch the AppMessage to the AppMessageHandler
    ///
    /// \param msg The AppMessage
    ///
    /// \return 0 on success, <0 on error
    ///
    int dispatchMsg( AppMessage const &msg );

    ///
    /// \brief parseHeader
    /// \param octet
    /// \return
    ///
    AppMessage *parseHeader( uint8_t octet );

    ///
    /// \brief validateHeader
    /// \return
    ///
    AppMessage *validateHeader();

    ///
    /// \brief parsePayload
    /// \param octet
    /// \return
    ///
    AppMessage *parsePayload( uint8_t octet );

    size_t m_octets_left_in_payload;
    size_t m_error_count;
    FixedBufferWithSize<JDKSAVDECC_APPDU_HEADER_LEN> m_header_buffer;
    AppMessage m_current_message;
    AppMessageHandler &m_handler;
};
}
