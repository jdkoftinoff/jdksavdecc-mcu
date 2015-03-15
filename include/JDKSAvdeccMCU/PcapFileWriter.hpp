#pragma once

/*
Copyright (c) 2014, Jeff Koftinoff
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "JDKSAvdeccMCU/World.hpp"
#include "JDKSAvdeccMCU/PcapFile.hpp"

#if JDKSAVDECCMCU_ENABLE_PCAPFILE == 1

namespace JDKSAvdeccMCU
{
class PcapFileWriter
{
  public:
    PcapFileWriter( std::string const &filename );
    virtual ~PcapFileWriter();

    void WritePacket( PcapFilePacket const &packet );
    void WritePacket( uint64_t time_in_micros, PcapFilePacket const &packet );
    void WritePacket( uint8_t const da[6], uint8_t const sa[6], uint16_t ethertype, PcapFilePacket const &packet_payload );
    void WritePacket( uint64_t time_in_micros,
                      uint8_t const da[6],
                      uint8_t const sa[6],
                      uint16_t ethertype,
                      PcapFilePacket const &packet_payload );

  private:
    PcapFile m_file;
    std::string m_filename;
};
}
#endif
