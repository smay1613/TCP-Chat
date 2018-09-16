#ifndef SOCKET_H
#define SOCKET_H

namespace sockets {

class SocketWrapper
{
public:
    SocketWrapper();
    SocketWrapper(const int descriptor);

    virtual ~SocketWrapper();

    int descriptor() const;
    bool isOpened() const;

    void setNonblock() const;

protected:
    int m_descriptor;
    bool m_isOpened;
};

}

#endif // SOCKET_H
