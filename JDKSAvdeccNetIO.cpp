
#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccNetIO.h"

namespace JDKSAvdecc {

NetIO *net;

NetIO::NetIO() {
    net=this;
}
NetIO::~NetIO() {
}

}


