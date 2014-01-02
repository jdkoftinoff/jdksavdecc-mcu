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

#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccHelpers.h"
#include "JDKSAvdeccFrame.h"

namespace JDKSAvdecc {

#if 0
void FormAEMSetControl( jdksavdecc_eui48 const &dest_mac,
                        jdksavdecc_eui48 const &src_mac,
                        jdksavdecc_eui64 const &my_controller_entity_id,
                        jdksavdecc_eui64 const &destination_entity_id,
                        bool unsolicited,
                        uint16_t descriptor_index,
                        uint8_t const *value_data,
                        uint16_t value_data_length,
                        uint8_t *buf ) {

    FrameBase pdu(buf,dest_mac,src_mac,JDKSAVDECC_AVTP_ETHERTYPE);  // DA, SA, EtherType, ADPDU = 82 bytes

    // control_data_length field is N + value_length
    uint16_t control_data_length = JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN -
            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + value_data_length;

    // AECPDU common control header
    pdu.PutOctet( 0x80 + JDKSAVDECC_SUBTYPE_AECP); // cd=1, subtype=0x7b (AECP)
    pdu.PutOctet( 0x00 + JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );  // sv=0, version=0, message_type = AEM_COMMAND
    pdu.PutOctet( ((JDKSAVDECC_AEM_STATUS_SUCCESS) << 3) + ((control_data_length>>8)&x7) ); // Send success code. top 3 bits of control_data_length

    pdu.PutOctet( control_data_length & 0xff ); // lower 8 bits of control_data_length

    pdu.PutEUI64( destination_entity_id ); // entity_id of the device we are setting
    pdu.PutEUI64( my_controller_entity_id ); // controller_id

    pdu.PutDoublet( m_sequence_id );
    pdu.PutDoublet( JDKSAVDECC_AEM_COMMAND_SET_CONTROL );
    pdu.PutDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
    pdu.PutDoublet( m_target_descriptor_index );
    pdu.PutBuf( m_value, m_value_length );

    m_net.SendRawNet( pdu.GetBuf(), pdu.GetLength() );
    m_sequence_id++;

}
#endif

}



