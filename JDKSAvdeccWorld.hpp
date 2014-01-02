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

#include "jdksavdecc-c/include/jdksavdecc.h"
#include "jdksavdecc-c/include/jdksavdecc_print.h"
#include "jdksavdecc-c/include/jdksavdecc_pdu_print.h"

#ifdef __ARDUINO__

#else
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#endif

#if defined(__ARDUINO__)
typedef uint32_t jdksavdecc_timestamp_in_milliseconds;

inline jdksavdecc_timestamp_in_milliseconds GetTimeInMs() {
    return millis();
}
#elif defined(__APPLE__) || defined(__linux__)
typedef uint64_t jdksavdecc_timestamp_in_milliseconds;
inline jdksavdecc_timestamp_in_milliseconds GetTimeInMs() {
    timeval tv;

    gettimeofday(&tv,0);
    return uint64_t(tv.tv_usec/1000 + tv.tv_sec*1000);
}
#elif defined(WIN32)
typedef uint64_t jdksavdecc_timestamp_in_milliseconds;
inline jdksavdecc_timestamp_in_milliseconds GetTimeInMs() {
    return uint64_t(GetTickCount());
}
#endif


extern "C" {
void avr_debug_log(const char *str, uint16_t v );
}


