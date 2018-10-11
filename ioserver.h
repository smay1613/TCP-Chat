#ifndef IOSERVER_H
#define IOSERVER_H
#include "ioworker.h"
#include <sys/epoll.h>
#include <Sockets/mastersocket.h>
#include <Sockets/slavesocket.h>
#include <vector>
#include <unordered_map>
#include <memory>

class IOServer
{
public:
    IOServer();
    // TODO: ADD DESTRUCTOR

    void addToEpoll(const sockets::SocketWrapper &descriptor) const;
    void setMasterEvent(const sockets::SocketWrapper &socket);

    int listenEvents();
    void proccessEvents(const int eventsCount);

    void notifyClients(std::string message);

    void acceptConnections();

    void start();

private:
    void createWorkers(int count);
    void initWorker();

    sockets::SlaveSocket m_epollSocket;
    epoll_event m_masterEvent;

    std::vector<epoll_event> m_events;

    std::unordered_map<int, std::pair<std::string,
                                      std::shared_ptr<sockets::SlaveSocket>>> m_clients;

    std::unique_ptr<IOWorker> m_ioWorker;
    int m_pipeChannelDescriptor;

    const int MAX_EVENTS = 16;
    const int BUFFER_LENGTH = 256;
};

#endif // IOSERVER_H
