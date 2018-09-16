#include "slavesocket.h"

sockets::SlaveSocket::SlaveSocket()
{

}

void sockets::SlaveSocket::setDescriptor(int descriptor)
{
    m_descriptor = descriptor;
}
