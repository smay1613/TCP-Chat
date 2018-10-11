#include "ioserver.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/wait.h>

IOServer::IOServer() : m_ioWorker (nullptr),
                       m_pipeChannelDescriptor(-1)
{
    m_epollSocket.setDescriptor(epoll_create1(0)); // with no flags

    if (m_epollSocket.descriptor() < 0) {
        std::cerr << "Epoll creation failed!" << std::endl;
    } else {
        std::cout << "Epoll creation successfull!" << std::endl;
    }

    m_masterEvent.events = EPOLLIN;
    m_events.resize(static_cast<size_t>(MAX_EVENTS));
}

void IOServer::addToEpoll(const sockets::SocketWrapper& socket) const
{
    epoll_event event;
    event.data.fd = socket.descriptor();
    event.events = EPOLLIN;

    int result = epoll_ctl(m_epollSocket.descriptor(),
                           EPOLL_CTL_ADD,
                           socket.descriptor(),
                           &event);

    if (result < 0) {
        std::cerr << "Adding descriptor " << socket.descriptor() << " to epoll failed!" << std::endl;
    } else {
        std::cout << "Descriptor " << socket.descriptor() << " successfully added to epoll!" << std::endl;
    }
}

void IOServer::setMasterEvent(const sockets::SocketWrapper& socket)
{
    m_masterEvent.data.fd = socket.descriptor();
    addToEpoll(socket);
}

int IOServer::listenEvents()
{
    int eventsCount = epoll_wait(m_epollSocket.descriptor(),
                                 &m_events[0],
                                 MAX_EVENTS,
                                 -1);
    if (eventsCount < 0) {
        std::cerr << "Error occurs during epoll_wait" << std::endl;
        return 0;
    }

    return eventsCount;
}

void IOServer::proccessEvents(int eventsCount) // TODO: GIVE EVENT DESCRIPTOR TO WORKER
{
    for (int i = 0; i < eventsCount; i++) {
        if (m_events.at(i).data.fd == m_masterEvent.data.fd) { // branch for master socket
            sockaddr clientAddress;
            socklen_t sizeOfClientAddress = sizeof(clientAddress);

            int descriptor = accept(m_masterEvent.data.fd,
                                    &clientAddress,
                                    &sizeOfClientAddress);

            std::shared_ptr<sockets::SlaveSocket> slaveSocket (new sockets::SlaveSocket(descriptor));
            slaveSocket->setNonblock();

            std::string ip = inet_ntoa(((sockaddr_in *) &clientAddress)->sin_addr);

            m_clients[descriptor] = std::make_pair(ip, slaveSocket);
            notifyClients(m_clients[descriptor].first + " connected!\n");

            addToEpoll(*slaveSocket);
        } else {                                               // branch for slaves
            std::vector<char> buffer(BUFFER_LENGTH);

            int eventDescriptor = m_events.at(i).data.fd;
            int bytesRead = recv(eventDescriptor,
                                 &buffer[0],
                                 BUFFER_LENGTH,
                                 MSG_NOSIGNAL);

            if (bytesRead < 1) {
                std::cerr << "Error while reading data from " << eventDescriptor << std::endl;
                notifyClients(m_clients[eventDescriptor].first + " disconnected!\n");
                m_clients.erase(eventDescriptor);
            } else {
                notifyClients(m_clients[eventDescriptor].first + ": " + std::string(buffer.begin(), buffer.end()));
            }
        }
    }
}

void IOServer::notifyClients(std::string message)
{
    if (message.empty()) {
        std::cerr << "You can't send empty message";
        return;
    }
    for (auto client : m_clients) {
            send(client.second.second->descriptor(),
                 &message[0],
                 message.size(),
                 MSG_NOSIGNAL);
    }
}

void IOServer::acceptConnections()
{
    while (true) {
        int eventsCount = listenEvents();
        proccessEvents(eventsCount);
    }
}

void IOServer::start()
{
    int workersCount = 4; // 4 cores
    createWorkers(workersCount);
}

void IOServer::createWorkers(int count)
{
    int pid = fork(); // main will accept connections, child will create workers and then exit

    if (pid < 0) {
        std::cerr << "Error while fork for master and workers" << std::endl;
        return;
    }

    if (pid == 0) { // we are in workercreator
        for (int i = 0; i < count; i++) { // create count workers
            int wPid = fork();
            if (wPid == 0) { // in worker
                std::cout << "Created worker " << " " << getpid() << " with parent " << getppid() << std::endl;
                m_ioWorker = std::make_unique<IOWorker>(); // creating worker
                initWorker();
            } else if (wPid < 0){
                std::cerr << "Error while creating worker " << i << std::endl;
            }
        }

        for(int i = 0; i < count; i++) { //worker creator wait for all childrens
            wait(nullptr);
            std::cout << "CHILDREN DIED" <<std::endl;
        }
        _exit(0); // close workercreator if no childrens left
    } else { // we are in main and we'll accept external connections
        acceptConnections(); // TODO: MAIN WILL PASS FDS TO WORKERS
    }
}

void IOServer::initWorker()
{
    for(;;);
    // TODO: WORKER START LISTENING ITS SOCKETPAIR AND DOING SOMETHING IN BLOCKING MODE
    // ADD SOCKETPAIR AND SETTING DESCRIPTORS FOR SERVER AND WORKER
    // WORKER SHOULD READ UNIX SOCKET TO GET SLAVE DESCRIPTOR, PROCESS IT AND SEND RESULT TO ALL CLIENTS (HOW TO STORE CLIENTS? WILL IT BE CHAT ROOM?)
}
