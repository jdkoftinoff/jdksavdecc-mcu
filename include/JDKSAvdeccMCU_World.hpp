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

#ifndef JDKSAVDECCMCU_BARE_METAL
#if defined( __AVR__ )
#define JDKSAVDECCMCU_ARDUINO 1
#define JDKSAVDECCMCU_BARE_METAL 1
#ifndef JDKSAVDECCMCU_ENABLE_FLOAT
#define JDKSAVDECCMCU_ENABLE_FLOAT 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET 1
#ifndef JDKSAVDECCMCU_ENABLE_PCAP
#define JDKSAVDECCMCU_ENABLE_PCAP 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_PCAPFILE
#define JDKSAVDECCMCU_ENABLE_PCAPFILE 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETLINUX
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETLINUX 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETMACOSX
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETMACOSX 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETWIN32
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETWIN32 0
#endif
#ifndef JDKSAVDECC_CPP_NO_IOSTREAM
#define JDKSAVDECC_CPP_NO_IOSTREAM
#endif
#ifndef JDKSAVDECC_CPP_NO_STDIO
#define JDKSAVDECC_CPP_NO_STDIO
#endif
#endif
#else
#define JDKSAVDECCMCU_BARE_METAL 0
#endif
#endif

#if JDKSAVDECCMCU_BARE_METAL == 0
#ifndef JDKSAVDECCMCU_ENABLE_FLOAT
#define JDKSAVDECCMCU_ENABLE_FLOAT 1
#endif
#ifndef JDKSAVDECCMCU_ARDUINO
#define JDKSAVDECCMCU_ARDUINO 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_PCAP
#define JDKSAVDECCMCU_ENABLE_PCAP 1
#endif
#ifndef JDKSAVDECCMCU_ENABLE_PCAPFILE
#define JDKSAVDECCMCU_ENABLE_PCAPFILE 1
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE 1
#endif
#if defined( __linux__ )
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETLINUX 1
#endif
#if defined( __APPLE__ ) && ( JDKSAVDECCMCU_ENABLE_PCAP == 1 )
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETMACOSX 1
#endif
#if defined( _WIN32 ) && ( JDKSAVDECCMCU_ENABLE_PCAP == 1 )
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETWIN32 1
#endif
#endif

#include "jdksavdecc.h"
#include "jdksavdecc_aem_descriptor.h"

namespace JDKSAvdeccMCU
{
}

#if defined( __AVR__ )
#include <SPI.h>

inline jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
{
    return millis();
}
#elif defined( __APPLE__ ) || defined( __linux__ )
#include <vector>
#include <memory>
#include <sys/time.h>
#include <iostream>
#include <iomanip>
namespace JDKSAvdeccMCU
{
inline jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
{
    timeval tv;

    gettimeofday( &tv, 0 );
    return jdksavdecc_timestamp_in_milliseconds( tv.tv_usec / 1000 )
           + jdksavdecc_timestamp_in_milliseconds( tv.tv_sec * 1000 );
}
}

#elif defined( _WIN32 )
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
namespace JDKSAvdeccMCU
{
inline jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
{
    return jdksavdecc_timestamp_in_milliseconds( GetTickCount() );
}
}
#endif

void jdksavdeccmcu_debug_log( const char *str, uint16_t v );
