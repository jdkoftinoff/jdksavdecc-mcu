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

