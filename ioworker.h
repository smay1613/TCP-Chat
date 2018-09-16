#ifndef IOWORKER_H
#define IOWORKER_H
#include <sys/epoll.h>
#include <Sockets/mastersocket.h>
#include <Sockets/slavesocket.h>
#include <vector>
#include <unordered_map>
#include <memory>

class IOWorker
{
public:
    IOWorker();

    void addToEpoll(const sockets::SocketWrapper &descriptor) const;
    void setMasterEvent(const sockets::MasterSocket &socket);

    int listenEvents();
    void proccessEvents(const int eventsCount);

    void notifyClients(std::string message);

private:
    sockets::SlaveSocket m_epollSocket;
    epoll_event m_masterEvent;

    std::vector<epoll_event> m_events;

    std::unordered_map<int, std::pair<std::string,
                                      std::shared_ptr<sockets::SlaveSocket>>> m_clients;

    const int MAX_EVENTS = 16;
    const int BUFFER_LENGTH = 1024;
};

#endif // IOWORKER_H
