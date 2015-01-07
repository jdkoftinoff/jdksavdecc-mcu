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

#include "JDKSAvdeccMCU_PlatformOptions.hpp"

#define JDKSAVDECCMCU_BARE_METAL 1

#ifndef JDKSAVDECCMCU_ARDUINO
#if defined( __AVR__ )
#define JDKSAVDECCMCU_ARDUINO 1
#else
#define JDKSAVDECCMCU_ARDUINO 0
#endif
#endif

#ifndef JDKSAVDECCMCU_ENABLE_STDIO
#define JDKSAVDECCMCU_ENABLE_STDIO 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_STDLIB
#define JDKSAVDECCMCU_ENABLE_STDLIB 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_MEMORY
#define JDKSAVDECCMCU_ENABLE_MEMORY 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_FLOAT
#define JDKSAVDECCMCU_ENABLE_FLOAT 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_VECTOR
#define JDKSAVDECCMCU_ENABLE_VECTOR 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_STRING
#define JDKSAVDECCMCU_ENABLE_STRING 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_IOSTREAM
#define JDKSAVDECCMCU_ENABLE_IOSTREAM 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETWIZNET 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_PCAP
#define JDKSAVDECCMCU_ENABLE_PCAP 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_PCAPFILE
#define JDKSAVDECCMCU_ENABLE_PCAPFILE 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETPCAPFILE 0
#endif
#ifndef JDKSAVDECCMCU_MAX_RAWSOCKETS
#define JDKSAVDECCMCU_MAX_RAWSOCKETS 2
#endif
#ifndef JDKSAVDECCMCU_ENABLE_MDNSREGISTER
#define JDKSAVDECCMCU_ENABLE_MDNSREGISTER 0
#endif
#ifndef JDKSAVDECCMCY_ENABLE_HTTP
#define JDKSAVDECCMCY_ENABLE_HTTP 0
#endif

#if JDKSAVDECCMCU_ENABLE_STDIO
#include <stdio.h>
#endif
#if JDKSAVDECCMCU_ENABLE_STDLIB
#include <cstdlib>
#endif
#if JDKSAVDECCMCU_ENABLE_FLOAT
#include <cfloat>
#endif
#if JDKSAVDECCMCU_ENABLE_VECTOR
#include <vector>
#endif
#if JDKSAVDECCMCU_ENABLE_STRING
#include <string>
#endif
#if JDKSAVDECCMCU_ENABLE_IOSTREAM
#include <iostream>
#endif
#if JDKSAVDECCMCU_ENABLE_MEMORY
#include <memory>
#endif

#include "jdksavdecc.h"

#if JDKSAVDECCMCU_ARDUINO
#include <SPI.h>

namespace JDKSAvdeccMCU
{
inline jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
{
    return millis();
}
}
#elif defined( JDKSAVDECCMCU_PLATFORM_GET_TIME_IN_MILLISECONDS )
namespace JDKSAvdeccMCU
{
jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
{
    return (jdksavdecc_timestamp_in_milliseconds)
        JDKSAVDECCMCU_PLATFORM_GET_TIME_IN_MILLISECONDS();
}
}
#else
namespace JDKSAvdeccMCU
{
jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds();
}

#endif

#endif
