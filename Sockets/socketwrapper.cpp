#include "Sockets/socketwrapper.h"
#include <iostream>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

sockets::SocketWrapper::SocketWrapper() : m_isOpened(true)
{
    m_descriptor = socket(AF_INET,
                      SOCK_STREAM,
                      0);
    int option = 1;
    setsockopt(m_descriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (m_descriptor == -1) {
        std::cerr << "Socket creation failed" << std::endl;
    } else {
        std::cout << "Socket creation success" << std::endl;
    }
}

sockets::SocketWrapper::SocketWrapper(const int descriptor) :
                                                            m_descriptor(descriptor),
                                                            m_isOpened(true)
{
}

sockets::SocketWrapper::~SocketWrapper()
{
    bool shutdownFailed = shutdown(m_descriptor, SHUT_RDWR);

    if (shutdownFailed) {
        std::cerr << "Socket shutdown failed!" << std::endl;
    }

    bool closeFailed = close(m_descriptor);

    if (closeFailed) {
        std::cerr << "Closing socket failed." << std::endl;
    } else {
        std::cout << "Socket closed successfully!" << std::endl;
    }

    m_isOpened = false;
}

int sockets::SocketWrapper::descriptor() const
{
    return m_descriptor;
}

bool sockets::SocketWrapper::isOpened() const
{
    return m_isOpened;
}

void sockets::SocketWrapper::setNonblock() const
{
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(m_descriptor, F_GETFL, 0)))
        flags = 0;
    fcntl(m_descriptor, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    ioctl(m_descriptor, FIONBIO, &flags);
#endif
}
