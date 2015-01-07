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

#if defined( _WIN32 )
#define JDKSAVDECCMCU_BARE_METAL 0

#ifndef JDKSAVDECCMCU_ARDUINO
#define JDKSAVDECCMCU_ARDUINO 0
#endif
#ifndef JDKSAVDECCMCU_ENABLE_FLOAT
#define JDKSAVDECCMCU_ENABLE_FLOAT 1
#endif
#ifndef JDKSAVDECCMCU_ENABLE_VECTOR
#define JDKSAVDECCMCU_ENABLE_VECTOR 1
#endif
#ifndef JDKSAVDECCMCU_ENABLE_STRING
#define JDKSAVDECCMCU_ENABLE_STRING 1
#endif
#ifndef JDKSAVDECCMCU_ENABLE_IOSTREAM
#define JDKSAVDECCMCU_ENABLE_IOSTREAM 1
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
#ifndef JDKSAVDECCMCU_MAX_RAWSOCKETS
#define JDKSAVDECCMCU_MAX_RAWSOCKETS 32
#endif
#ifndef JDKSAVDECCMCU_ENABLE_MDNSREGISTER
#define JDKSAVDECCMCU_ENABLE_MDNSREGISTER 0
#endif
#if JDKSAVDECCMCU_ENABLE_PCAP
#define JDKSAVDECCMCU_ENABLE_RAWSOCKETWIN32 1
#endif
#ifndef JDKSAVDECCMCU_ENABLE_HTTP
#define JDKSAVDECCMCU_ENABLE_HTTP 1
#endif

#include <WS2tcpip.h>
#include <winsock2.h>
#include <Windows.h>
#include <Iphlpapi.h>
#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "Iphlpapi.lib" )

#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <string>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cfloat>

#include "jdksavdecc.h"

namespace JDKSAvdeccMCU
{
inline jdksavdecc_timestamp_in_milliseconds getTimeInMilliseconds()
{
    return jdksavdecc_timestamp_in_milliseconds( GetTickCount() );
}
}

#endif
