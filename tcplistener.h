#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include <netinet/in.h>
#include <string>
#include "Sockets/mastersocket.h"
#include "ioworker.h"

//TODO: remove copy possibility

class TCPListener
{
public:
    TCPListener();
    TCPListener(unsigned short port, std::string ip);

    bool initialize(unsigned short port, std::string ip);
    bool start();

    void acceptData();

private:
    IOWorker m_ioWorker;

    sockets::MasterSocket m_masterSocket;
    sockaddr_in m_sockAddress;
};

#endif // TCPLISTENER_H
