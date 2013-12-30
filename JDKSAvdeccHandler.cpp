#include "JDKSAvdeccWorld.h"
#include "JDKSAvdeccHandler.h"

namespace JDKSAvdecc {

Handler::~Handler() {
}

void Handler::Tick( uint32_t time_in_millis ) {
}

bool Handler::ReceivedPDU( uint32_t time_in_millis, uint8_t *buf, uint16_t len ) {
    return false;
}




}



