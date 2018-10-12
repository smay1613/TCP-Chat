#ifndef IOWORKER_H
#define IOWORKER_H
#include <unordered_map>
#include <vector>
#include <memory>
#include "Sockets/slavesocket.h"
#include <sys/epoll.h>

class IOWorker
{
public:
    IOWorker(int pipeChannelDescriptor);
    void notifyClients(std::string& message);

    void processEvents(int eventsCount);
    void addToEpoll(const sockets::SocketWrapper &socket) const;

    void setMasterEvent(const sockets::SocketWrapper &socket);
    int listenEvents();

private:
    int m_pipeChannelDescriptor;
    std::unordered_map<int, std::pair<std::string,
                                      std::shared_ptr<sockets::SlaveSocket>>> m_clients;
    epoll_event m_masterEvent;
    sockets::SlaveSocket m_epollSocket;
    std::vector<epoll_event> m_events;

    const int MAX_EVENTS = 1;
    const int BUFFER_LENGTH = 256;
};

#endif // IOWORKER_H
