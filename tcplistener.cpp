#include "tcplistener.h"
#include <iostream>
//#include <vector>
//#include <algorithm>
//#include <iterator>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

TCPListener::TCPListener() {
//    std::cout << "BOOM!";
    if (initialize(8000, "")) {
        std::cout << "Server initialized successfully!" << std::endl;
    }
    if (start()) {
        std::cout << "Server started successfully!" << std::endl;
    }
//    std::cout << "AAAAAAAAA";
    m_masterSocket.setNonblock();
    m_ioWorker.setMasterEvent(m_masterSocket);
}

TCPListener::TCPListener(unsigned short port, std::string ip)
{
    if (initialize(port, ip)) {
        std::cout << "Server initialized successfully!" << std::endl;
    }
    if (start()) {
        std::cout << "Server started successfully!" << std::endl;
    }
}

bool TCPListener::initialize(unsigned short port = 8000, std::string ip = "")
{
    m_sockAddress.sin_family = AF_INET;
    m_sockAddress.sin_port = htons(port);
    if (!ip.empty()) {
        inet_pton(AF_INET, ip.c_str(), &m_sockAddress);
    } else {
        m_sockAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }

    int bindResult = bind(m_masterSocket.descriptor(),
                           (sockaddr*)(&m_sockAddress),
                           sizeof(m_sockAddress));

    if (bindResult < 0) {
        std::cerr << "Can't initialize server, because bind was failed" << std::endl;
    }

    return bindResult == 0;
}

bool TCPListener::start()
{
    bool listenFailed = listen(m_masterSocket.descriptor(),
                               SOMAXCONN); // 128

    if (listenFailed) {
        std::cerr << "Can't start server, because listen was failed" << std::endl;
    }

    return !listenFailed;
}

void TCPListener::acceptData()
{
    while (true) {
        int eventsCount = m_ioWorker.listenEvents();
        m_ioWorker.proccessEvents(eventsCount);
    }
}
