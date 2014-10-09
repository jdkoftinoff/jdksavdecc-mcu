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
#include "JDKSAvdeccMCU_RawNetIO.hpp"

#ifdef JDKSAVDECCMCU_ENABLE_RAW

#include <pcap.h>

namespace JDKSAvdeccMCU
{

RawNetIO::RawNetIO( uint16_t ethertype, const char *interface_name, const uint8_t join_multicast[] ) {}

RawNetIO::~RawNetIO() {}

void RawNetIO::initialize() {}

const jdksavdecc_eui48 &RawNetIO::getMACAddress() const {}

uint16_t RawNetIO::receiveRawNet( uint8_t *data, uint16_t max_len ) {}

bool RawNetIO::sendRawNet(
    const uint8_t *data, uint16_t len, const uint8_t *data1, uint16_t len1, const uint8_t *data2, uint16_t len2 )
{
}

bool RawNetIO::sendReplyRawNet(
    const uint8_t *data, uint16_t len, const uint8_t *data1, uint16_t len1, const uint8_t *data2, uint16_t len2 )
{
}
}

#else

const char *jdksavdeccmcu_rawnetio_file = __FILE__;

#endif
