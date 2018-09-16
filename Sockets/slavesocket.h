#ifndef SLAVESOCKET_H
#define SLAVESOCKET_H
#include "mastersocket.h"

namespace sockets {

class SlaveSocket : public SocketWrapper
{
public:
    using SocketWrapper::SocketWrapper;
    SlaveSocket();

    void setDescriptor(int descriptor);
};

}
#endif // SLAVESOCKET_H
