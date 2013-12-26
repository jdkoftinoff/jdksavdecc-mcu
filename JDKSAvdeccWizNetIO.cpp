
#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccWizNet.h"

namespace JDKSAvdecc {

bool WizNetIO::CanReceiveRawNet() { }

uint16_t WizNetIO::ReceiveRawNet( uint8_t *base_ptr, uint16_t max_len ) { }

bool WizNetIO::StartSendRawNet() { }

void WizNetIO::SendRawNetBytes( uint8_t const *ptr, uint16_t len ) { }

void WizNetIO::SendRawNetOctet( uint8_t val ) { }

void WizNetIO::SendRawNetDoublet( uint16_t val ) { }

void WizNetIO::SendRawNetQuadlet( uint32_t val ) { }

void WizNetIO::SendRawNetEui48( jdksavdecc_eui48 val ) { }

void WizNetIO::SendRawNetEui64( jdksavdecc_eui64 val ) { }

void WizNetIO::FinishSendRawNet() { }


}
