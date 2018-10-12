#ifndef IOSERVER_H
#define IOSERVER_H
#include "ioworker.h"
#include <sys/epoll.h>
#include <Sockets/socketwrapper.h>
#include <vector>
#include <memory>

class IOServer
{
public:
    IOServer();
    // TODO: ADD DESTRUCTOR

    int getRoomFromUser(const sockets::SlaveSocket &userSocket);
//    void addToEpoll(const sockets::SocketWrapper &descriptor) const;
    void setMasterSocket(sockets::MasterSocket *socket);

//    int listenEvents();
    void processConnections(const int eventsCount);
    void acceptConnections();

    void start();

private:
    void createWorkers(const int count);
    void initWorker(const int channelDescriptor);

    std::shared_ptr<sockets::MasterSocket> m_serverSocket;

    std::unique_ptr<IOWorker> m_ioWorker;
    std::vector<int> m_workersSockets;

    const int MAX_CONNECTIONS = 16;
//    std::pair<int, int> pipe;
};

#endif // IOSERVER_H
