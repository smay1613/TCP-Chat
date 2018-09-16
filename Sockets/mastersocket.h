#ifndef MASTERSOCKET_H
#define MASTERSOCKET_H
#include "socketwrapper.h"

namespace sockets {

class MasterSocket : public sockets::SocketWrapper
{
public:
    MasterSocket();
};

}

#endif // MASTERSOCKET_H
