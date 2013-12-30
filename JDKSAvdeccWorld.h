#pragma once

#include "jdksavdecc-c/include/jdksavdecc.h"
#include "jdksavdecc-c/include/jdksavdecc_print.h"
#include "jdksavdecc-c/include/jdksavdecc_pdu_print.h"

#ifdef __ARDUINO__

#else
#include <iostream>
#include <iomanip>
#endif

extern "C" {
void avr_debug_log(const char *str, uint16_t v );
}

#define JDKSAVDECC_FRAME_HEADER_DA_OFFSET (0)
#define JDKSAVDECC_FRAME_HEADER_SA_OFFSET (6)
#define JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET (12)
#define JDKSAVDECC_FRAME_HEADER_LEN (14)
#define JDKSAVDECC_FRAME_MAX_SIZE \
    (JDKSAVDECC_FRAME_HEADER_LEN + \
    JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + \
    JDKSAVDECC_AECP_MAX_CONTROL_DATA_LENGTH)


