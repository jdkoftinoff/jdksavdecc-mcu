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

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_Http.hpp"

#if JDKSAVDECCMCU_ENABLE_HTTP

namespace JDKSAvdeccMCU
{

static void append_to_vector( std::vector<uint8_t> *dest, std::string const &s )
{
    for ( std::string::const_iterator i = s.begin(); i != s.end(); ++i )
    {
        dest->push_back( (uint8_t)*i );
    }
}

static void append_to_vector( std::vector<uint8_t> *dest,
                              std::vector<uint8_t> const &b )
{
    for ( std::vector<uint8_t>::const_iterator i = b.begin(); i != b.end();
          ++i )
    {
        dest->push_back( *i );
    }
}

void HttpRequest::clear()
{
    m_method.clear();
    m_path.clear();
    m_version.clear();
    m_headers.clear();
    m_content.clear();
}

void HttpRequest::setCONNECT( const std::string &path,
                              const std::vector<std::string> &headers )
{
    m_method = "CONNECT";
    m_path = path;
    m_version = "HTTP/1.1";

    m_headers.clear();

    for ( std::vector<std::string>::const_iterator i = headers.begin();
          i != headers.end();
          ++i )
    {
        m_headers.push_back( *i );
    }

    m_content.clear();
}

void HttpRequest::set( const std::string &method,
                       const std::string &host,
                       const std::string &port,
                       const std::string &path,
                       const std::vector<std::string> &headers )
{
    m_method = method;
    m_path = path;
    m_version = "HTTP/1.1";
    m_headers.clear();

    std::string s;

    if ( host.length() > 0 )
    {
        s.clear();
        s.append( "Host: " );
        s.append( host );
        if ( port.length() > 0 )
        {
            s.append( ":" );
            s.append( port );
        }
        m_headers.push_back( s );
    }

    for ( std::vector<std::string>::const_iterator i = headers.begin();
          i != headers.end();
          ++i )
    {
        m_headers.push_back( *i );
    }

    m_content.clear();
}

void HttpRequest::set( const std::string &method,
                       const std::string &host,
                       const std::string &port,
                       const std::string &path,
                       const std::vector<std::string> &headers,
                       const std::string &content_type,
                       const std::vector<uint8_t> &content )
{
    m_method = method;
    m_path = path;
    m_version = "HTTP/1.1";
    m_headers.clear();

    std::string s;

    if ( host.length() > 0 )
    {
        s.clear();
        s.append( "Host: " );
        s.append( host );
        if ( port.length() > 0 )
        {
            s.append( ":" );
            s.append( port );
        }
        m_headers.push_back( s );
    }

    if ( content_type.length() > 0 )
    {
        s.clear();
        s.append( "Content-Type: " );
        s.append( content_type );
        m_headers.push_back( s );

        if ( content.size() > 0 )
        {
            s.clear();
            s.append( "Content-Length: " );

            {
                char lenascii[32];
                sprintf( lenascii, "%d\r\n", (int)content.size() );
                s.append( lenascii );
            }
            m_headers.push_back( s );
        }
    }

    for ( std::vector<std::string>::const_iterator i = headers.begin();
          i != headers.end();
          ++i )
    {
        m_headers.push_back( *i );
    }

    m_content = content;
}

void HttpRequest::flatten( std::string *dest ) const
{
    dest->clear();
    dest->append( m_method );
    dest->append( " " );
    dest->append( m_path );
    dest->append( " " );
    dest->append( m_version );
    dest->append( "\r\n" );

    for ( std::vector<std::string>::const_iterator i = m_headers.begin();
          i != m_headers.end();
          ++i )
    {
        dest->append( *i );
        dest->append( "\r\n" );
    }
    dest->append( "\r\n" );
}

void HttpResponse::clear()
{
    m_version.clear();
    m_status_code.clear();
    m_reason_phrase.clear();
    m_headers.clear();
    m_content.clear();
}

void HttpResponse::flatten( std::vector<uint8_t> *dest ) const
{
    dest->clear();
    append_to_vector( dest, m_version );
    dest->push_back( ' ' );
    append_to_vector( dest, m_status_code );
    dest->push_back( ' ' );
    append_to_vector( dest, m_reason_phrase );
    dest->push_back( '\r' );
    dest->push_back( '\n' );

    for ( std::vector<std::string>::const_iterator i = m_headers.begin();
          i != m_headers.end();
          ++i )
    {
        append_to_vector( dest, *i );
        dest->push_back( '\r' );
        dest->push_back( '\n' );
    }
    dest->push_back( '\r' );
    dest->push_back( '\n' );
    append_to_vector( dest, m_content );
}

void HttpServerParserSimple::clear()
{
    HttpServerParser::clear();
    m_parse_state = ParsingMethod;
    m_cur_line.clear();
}

ssize_t HttpServerParserSimple::onIncomingHttpData( const uint8_t *data,
                                                    ssize_t len )
{
    ssize_t r = 0;

    // did we receive EOF
    if ( len == 0 )
    {
        // if we were in ParsingContentUntilClose state, then we are done
        if ( m_parse_state == ParsingContentUntilClose )
        {
            // call the handler
            if ( m_handler->onIncomingHttpRequest( *m_request ) )
            {
                // the handler returned true, so we will return 0 here
                r = 0;
            }
            else
            {
                // the handler returned false, so we will error out here
                r = -1;
            }
        }
        else
        {
            // If this happens during any other state, then this is an error
            r = -1;
        }
    }
    else
    {
        bool stop = false;

        // parse through as much of the data block as possible
        for ( r = 0; r < len; ++r )
        {
            // Abort parsing if we need to here
            if ( stop )
            {
                break;
            }

            // get the current octet as a char
            char c = (char)data[r];

            switch ( m_parse_state )
            {
            case ParsingMethod:
                // the HTTP method is delimited by a space
                if ( c == ' ' )
                {
                    m_parse_state = ParsingPath;
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the method
                    m_request->m_method.push_back( c );
                }
                else
                {
                    // any non-printing char is an error here
                    stop = true;
                    r = -1;
                }
                break;
            case ParsingPath:
                // the HTTP path is delimited by a space
                if ( c == ' ' )
                {
                    m_parse_state = ParsingVersion;
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the path
                    m_request->m_path.push_back( c );
                }
                else
                {
                    // any non-printing char is an error here
                    stop = true;
                    r = -1;
                }
                break;
            case ParsingVersion:
                // The HTTP Version is delimited by LF
                if ( c == '\n' )
                {
                    m_parse_state = ParsingHeaderLine;
                    m_cur_line.clear();
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the version
                    m_request->m_version.push_back( c );
                }
                else if ( c != '\r' )
                {
                    // any non-printing char besides CR is an error here
                    stop = true;
                    r = -1;
                }
                break;
            case ParsingHeaderLine:
                if ( c == '\n' )
                {
                    // The header line is delimited by LF
                    if ( m_cur_line.length() == 0 )
                    {
                        // The HTTP header is delimited by a blank line
                        // If the method is POST or PUT then we have content
                        // to parse too
                        if ( m_request->m_method == "POST"
                             || m_request->m_method == "PUT" )
                        {
                            m_parse_state = ParsingContentUntilClose;
                            m_request->m_content.clear();
                            // TODO: we can extract content-length
                            // field from header and use a different
                            // state which tracks content length
                        }
                        else
                        {
                            // For other methods, we are done now.
                            stop = true;
                            m_request->m_content.clear();

                            // give the request to the handler
                            if ( !m_handler->onIncomingHttpRequest(
                                     *m_request ) )
                            {
                                // the handler returned false, so we will
                                // error out here
                                r = -1;
                            }
                        }
                    }
                    else
                    {
                        // save the formed line
                        m_request->m_headers.push_back( m_cur_line );
                        m_cur_line.clear();
                    }
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the line
                    m_cur_line.push_back( c );
                }
                else if ( c != '\r' )
                {
                    // any non-printing char besides CR is an error here
                    stop = true;
                    r = -1;
                }
                break;
            case ParsingContentUntilClose:
                m_request->m_content.push_back( data[r] );
                break;
            case ParsingFinished:
                stop = true;
                break;
            }
        }
    }
    return r;
}

void HttpServerParser::clear() { m_request->clear(); }

void HttpClientParser::clear() { m_response->clear(); }

void HttpClientParserSimple::clear()
{
    HttpClientParser::clear();
    m_cur_line.clear();
    m_parse_state = ParsingVersion;
}

ssize_t HttpClientParserSimple::onIncomingHttpData( const uint8_t *data,
                                                    ssize_t len )
{
    ssize_t r = 0;

    // did we receive EOF
    if ( len == 0 )
    {
        // If this happens during any other state, then this is an error
        r = -1;
    }
    else
    {
        bool stop = false;

        // parse through as much of the data block as possible
        for ( r = 0; r < len; ++r )
        {
            // Abort parsing if we need to here
            if ( stop )
            {
                break;
            }

            // get the current octet as a char
            char c = (char)data[r];

            switch ( m_parse_state )
            {
            case ParsingVersion:
                // The HTTP Version is delimited by space
                if ( c == ' ' )
                {
                    m_parse_state = ParsingStatusCode;
                    m_response->m_status_code.clear();
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the version
                    m_response->m_version.push_back( c );
                }
                else if ( c != '\r' )
                {
                    // any non-printing char besides CR is an error here
                    stop = true;
                    r = -1;
                }
                break;
            case ParsingStatusCode:
                // the HTTP status code is delimited by a space
                if ( c == ' ' )
                {
                    m_parse_state = ParsingReasonPhrase;
                    m_response->m_reason_phrase.clear();
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the status code
                    m_response->m_status_code.push_back( c );
                }
                else
                {
                    // any non-printing char is an error here
                    stop = true;
                    r = -1;
                }
                break;
            case ParsingReasonPhrase:
                // the HTTP reason phrase is delimited by a '\n'
                if ( c == '\n' )
                {
                    m_parse_state = ParsingHeaderLine;
                    m_cur_line.clear();
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the reason phrase
                    m_response->m_reason_phrase.push_back( c );
                }
                else if ( c != '\r' )
                {
                    // any non-printing char is an error here
                    stop = true;
                    r = -1;
                }
                break;
            case ParsingHeaderLine:
                if ( c == '\n' )
                {
                    // The header line is delimited by LF
                    if ( m_cur_line.length() == 0 )
                    {
                        // We got all the headers
                        stop = true;
                        m_response->m_content.clear();

                        // give the request to the handler
                        if ( !m_handler->onIncomingHttpResponse( *m_response ) )
                        {
                            // the handler returned false, so we will
                            // error out here
                            r = -1;
                        }
                    }
                    else
                    {
                        // save the formed line
                        m_response->m_headers.push_back( m_cur_line );
                        m_cur_line.clear();
                    }
                }
                else if ( isprint( c ) )
                {
                    // only append printable chars to the line
                    m_cur_line.push_back( c );
                }
                else if ( c != '\r' )
                {
                    // any non-printing char besides CR is an error here
                    stop = true;
                    r = -1;
                }
                break;
            }
        }
    }
    return r;
}

bool HttpServerHandler::onIncomingHttpRequest( const HttpRequest &request )
{
    bool r = false;
    if ( request.m_version.substr( 0, 7 ) == "HTTP/1." )
    {
        if ( request.m_method == "GET" )
        {
            r = onIncomingHttpGetRequest( request );
        }
        else if ( request.m_method == "HEAD" )
        {
            r = onIncomingHttpHeadRequest( request );
        }
        else if ( request.m_method == "POST" )
        {
            r = onIncomingHttpPostRequest( request );
        }
        else if ( request.m_method == "PUT" )
        {
            r = onIncomingHttpPostRequest( request );
        }
        else if ( request.m_method == "DELETE" )
        {
            r = onIncomingHttpDeleteRequest( request );
        }
        else if ( request.m_method == "OPTIONS" )
        {
            r = onIncomingHttpOptionsRequest( request );
        }
        else if ( request.m_method == "CONNECT" )
        {
            r = onIncomingHttpConnectRequest( request );
        }
        else if ( request.m_method == "TRACE" )
        {
            r = onIncomingHttpTraceRequest( request );
        }
    }
    return r;
}
}
#else
extern const char *jdksavdeccmcu_http_file = __FILE__;
#endif
