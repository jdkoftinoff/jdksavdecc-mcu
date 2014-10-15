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

#if JDKSAVDECCMCU_ENABLE_PCAPFILE == 1

#include "JDKSAvdeccMCU_PcapFile.hpp"

namespace JDKSAvdeccMCU
{

#ifdef _WIN32

#if defined( _MSC_VER ) || defined( _MSC_EXTENSIONS )
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

int gettimeofday( struct timeval *tv, struct timezone * );

int gettimeofday( struct timeval *tv, struct timezone * )
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag;
    if ( NULL != tv )
    {
        GetSystemTimeAsFileTime( &ft );
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;
        /*converting file time to unix epoch*/
        tmpres /= 10; /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)( tmpres / 1000000UL );
        tv->tv_usec = (long)( tmpres % 1000000UL );
    }
    return 0;
}
#endif

uint64_t get_current_time_in_microseconds()
{
    uint64_t t = 0;
    struct timeval tv;
    if ( gettimeofday( &tv, 0 ) == 0 )
    {
        return ( uint64_t )( ( tv.tv_sec * 1000000 ) + ( tv.tv_usec ) );
    }
    return t;
}
}

#else
const char *jdksavdeccmcu_pcapfile_file = __FILE__;

#endif

