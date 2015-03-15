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

#if JDKSAVDECCMCU_ENABLE_HTTP

namespace JDKSAvdeccMCU
{

///
/// \brief The HttpRequest class
///
/// Contains the details of an HTTP Request
///
class HttpRequest
{
  public:
    virtual ~HttpRequest() {}

    ///
    /// \brief clear Clears all fields
    ///
    virtual void clear();

    ///
    /// \brief setCONNECT
    ///
    /// Set the request to be a CONNECT method
    ///
    /// \param path
    /// \param headers
    ///
    void setCONNECT( std::string const &path, std::vector<std::string> const &headers );

    ///
    /// \brief setGET
    ///
    /// Set the request to be a GET method
    ///
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    ///
    void
        setGET( std::string const &host, std::string const &port, std::string const &path, std::vector<std::string> const &headers )
    {
        set( "GET", host, port, path, headers );
    }

    ///
    /// \brief setHEAD
    ///
    /// Set the request to be a HEAD method
    ///
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    ///
    void setHEAD( std::string const &host,
                  std::string const &port,
                  std::string const &path,
                  std::vector<std::string> const &headers )
    {
        set( "HEAD", host, port, path, headers );
    }

    ///
    /// \brief setOPTIONS
    ///
    /// Set the request to be a OPTIONS request
    ///
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    ///
    void setOPTIONS( std::string const &host,
                     std::string const &port,
                     std::string const &path,
                     std::vector<std::string> const &headers )
    {
        set( "OPTIONS", host, port, path, headers );
    }

    ///
    /// \brief setDELETE
    ///
    /// Set the request to be a DELETE request
    ///
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    ///
    void setDELETE( std::string const &host,
                    std::string const &port,
                    std::string const &path,
                    std::vector<std::string> const &headers )
    {
        set( "DELETE", host, port, path, headers );
    }

    ///
    /// \brief setPOST
    ///
    /// Set the request to be a POST request
    ///
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    /// \param content_type
    /// \param content
    ///
    void setPOST( std::string const &host,
                  std::string const &port,
                  std::string const &path,
                  std::vector<std::string> const &headers,
                  std::string const &content_type,
                  std::vector<uint8_t> const &content )
    {
        set( "POST", host, port, path, headers, content_type, content );
    }

    ///
    /// \brief setPUT
    ///
    /// Set the request to be a PUT request
    ///
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    /// \param content_type
    /// \param content
    ///
    void setPUT( std::string const &host,
                 std::string const &port,
                 std::string const &path,
                 std::vector<std::string> const &headers,
                 std::string const &content_type,
                 std::vector<uint8_t> const &content )
    {
        set( "PUT", host, port, path, headers, content_type, content );
    }

    ///
    /// \brief set
    ///
    /// Set the request to be an arbitrary method with no content
    ///
    /// \param method
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    ///
    void set( std::string const &method,
              std::string const &host,
              std::string const &port,
              std::string const &path,
              std::vector<std::string> const &headers );

    ///
    /// \brief set
    ///
    /// Set the request to be an arbitrary method with content
    ///
    /// \param method
    /// \param host
    /// \param port
    /// \param path
    /// \param headers
    /// \param content_type
    /// \param content
    ///
    void set( std::string const &method,
              std::string const &host,
              std::string const &port,
              std::string const &path,
              std::vector<std::string> const &headers,
              std::string const &content_type,
              std::vector<uint8_t> const &content );

    ///
    /// \brief flattenHeaders
    ///
    /// Flatten all the header parameters into one buffer suitable
    /// for transmission via socket
    ///
    /// \param dest pointer to std::string which will contain the
    /// entire header for the request in one block
    ///
    virtual void flatten( std::string *dest ) const;

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

    virtual void flatten( std::vector<uint8_t> *dest ) const;

    void addHeader( std::string const &line ) { m_headers.push_back( line ); }

    void addHeader( std::string const &name, std::string const &value ) { m_headers.push_back( name + ": " + value ); }

    void setContent( std::string const &s )
    {
        m_content.clear();
        m_content.resize( s.length() );
        memcpy( reinterpret_cast<uint8_t *>( m_content.data() ), s.c_str(), s.length() );
    }

    void setContent( std::vector<uint8_t> const &v ) { m_content = v; }

    std::string m_version;
    std::string m_status_code;
    std::string m_reason_phrase;
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
    virtual bool onIncomingHttpRequest( HttpRequest const &request );

    virtual bool onIncomingHttpConnectRequest( HttpRequest const &request ) { return false; }

    virtual bool onIncomingHttpHeadRequest( HttpRequest const &request ) { return false; }

    virtual bool onIncomingHttpGetRequest( HttpRequest const &request ) { return false; }

    virtual bool onIncomingHttpPutRequest( HttpRequest const &request ) { return false; }

    virtual bool onIncomingHttpPostRequest( HttpRequest const &request ) { return false; }

    virtual bool onIncomingHttpDeleteRequest( HttpRequest const &request ) { return false; }

    virtual bool onIncomingHttpOptionsRequest( HttpRequest const &request ) { return false; }

    virtual bool onIncomingHttpTraceRequest( HttpRequest const &request ) { return false; }
};

class HttpClientHandler
{
  public:
    HttpClientHandler() {}
    virtual ~HttpClientHandler() {}

    virtual bool onIncomingHttpResponse( HttpResponse const &response ) = 0;
};

class HttpServerParser
{
  public:
    HttpServerParser( HttpRequest *request, HttpServerHandler *handler ) : m_request( request ), m_handler( handler ) {}

    virtual ~HttpServerParser() {}

    virtual void clear();

    virtual ssize_t onIncomingHttpData( uint8_t const *data, ssize_t len ) = 0;

  protected:
    HttpRequest *m_request;
    HttpServerHandler *m_handler;
};

class HttpServerParserSimple : public HttpServerParser
{
  public:
    HttpServerParserSimple( HttpRequest *request, HttpServerHandler *handler ) : HttpServerParser( request, handler ) {}

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

class HttpClientParser
{
  public:
    HttpClientParser( HttpResponse *response, HttpClientHandler *handler ) : m_response( response ), m_handler( handler ) {}

    virtual ~HttpClientParser() {}

    virtual void clear();

    virtual ssize_t onIncomingHttpData( uint8_t const *data, ssize_t len ) = 0;

  protected:
    HttpResponse *m_response;
    HttpClientHandler *m_handler;
};

class HttpClientParserSimple : public HttpClientParser
{
  public:
    HttpClientParserSimple( HttpResponse *response, HttpClientHandler *handler ) : HttpClientParser( response, handler ) {}

    virtual ~HttpClientParserSimple() {}

    virtual void clear();

    virtual ssize_t onIncomingHttpData( uint8_t const *data, ssize_t len );

  protected:
    enum
    {
        ParsingVersion,
        ParsingStatusCode,
        ParsingReasonPhrase,
        ParsingHeaderLine
    } m_parse_state;

    std::string m_cur_line;
};
}

#endif
