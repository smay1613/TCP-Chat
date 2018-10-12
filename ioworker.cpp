#include "ioworker.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

IOWorker::IOWorker(int pipeChannelDescriptor) :
                   m_pipeChannelDescriptor(pipeChannelDescriptor)
{
    m_epollSocket.setDescriptor(epoll_create1(0)); // with no flags

    if (m_epollSocket.descriptor() < 0) {
        std::cerr << "Epoll creation failed in worker " << getpid() << std::endl;
    } else {
        std::cout << "Epoll creation successfull!" << std::endl;
    }

    m_masterEvent.events = EPOLLIN;
    m_events.resize(static_cast<size_t>(MAX_EVENTS));
}

void IOWorker::notifyClients(std::string& message)
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

void IOWorker::processEvents(int eventsCount)
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
//            notifyClients(m_clients[descriptor].first + " connected!\n");

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
//                notifyClients(m_clients[eventDescriptor].first + " disconnected!\n");
//                m_clients.erase(eventDescriptor);
            } else {
//                notifyClients(m_clients[eventDescriptor].first + ": " + std::string(buffer.begin(), buffer.end()));
            }
        }
    }
}

void IOWorker::addToEpoll(const sockets::SocketWrapper& socket) const
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

void IOWorker::setMasterEvent(const sockets::SocketWrapper& socket)
{
    m_masterEvent.data.fd = socket.descriptor();
    addToEpoll(socket);
}

int IOWorker::listenEvents()
{
    int eventsCount = epoll_wait(m_epollSocket.descriptor(),
                                 &m_events[0],
                                 MAX_EVENTS,
                                 -1);
    if (eventsCount < 0) {
        std::cerr << "Error occurs during epoll_wait" << std::endl;
    }

    return eventsCount;
}
