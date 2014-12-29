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

namespace JDKSAvdeccMCU
{

class HttpRequest
{
  public:
    virtual ~HttpRequest() {}

    virtual void clear();

    void setCONNECT( std::string const &path,
                     std::vector<std::string> const &headers );

    void setGET( std::string const &host,
                 std::string const &port,
                 std::string const &path,
                 std::vector<std::string> const &headers );

    void setPOST( std::string const &host,
                  std::string const &port,
                  std::string const &path,
                  std::vector<std::string> const &headers,
                  std::string const &content_type,
                  std::vector<uint8_t> const &content );

    virtual void flattenHeaders( std::string *dest );

    std::string m_method;
    std::string m_path;
    std::string m_version;
    std::vector<std::string> m_headers;
    std::vector<uint8_t> m_content;
};

class HttpResponse
{
  public:
    virtual ~HttpResponse() {}

    virtual void clear();

    int m_code;
    std::string m_code_string;
    std::vector<std::string> m_headers;
    std::vector<uint8_t> m_content;
};

class HttpServerHandler
{
  public:
    HttpServerHandler() {}
    virtual ~HttpServerHandler() {}

    ///
    /// \brief onIncomingHttpRequest
    ///
    ///
    virtual bool onIncomingHttpRequest( HttpRequest const &request ) = 0;
};

class HttpServerParser
{
  public:
    HttpServerParser( HttpRequest *request, HttpServerHandler *handler )
        : m_request( request ), m_handler( handler )
    {
    }

    virtual ~HttpServerParser() {}

    virtual void clear();

    virtual ssize_t onIncomingHttpData( uint8_t const *data, ssize_t len ) = 0;

  protected:
    HttpRequest *m_request;
    HttpServerHandler *m_handler;
};

class HttpServerParserSimple : protected HttpServerParser
{
  public:
    HttpServerParserSimple( HttpRequest *request, HttpServerHandler *handler )
        : HttpServerParser( request, handler )
    {
    }

    virtual ~HttpServerParserSimple() {}

    virtual void clear();

    virtual ssize_t onIncomingHttpData( uint8_t const *data, ssize_t len );

  protected:
    enum
    {
        ParsingMethod,
        ParsingPath,
        ParsingVersion,
        ParsingHeaderLine,
        ParsingContentUntilClose,
        ParsingFinished
    } m_parse_state;

    std::string m_cur_line;
};
}
