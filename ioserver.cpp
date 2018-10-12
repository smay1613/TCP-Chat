#include "ioserver.h"
#include <iostream>
//#include <algorithm>
//#include <memory>
#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sstream>

IOServer::IOServer() :
                        m_ioWorker (nullptr)
{

}

int IOServer::getRoomFromUser(const sockets::SlaveSocket& userSocket)
{
    std::string message {"Hello! Please choose your room [0-3]: "};
    send(userSocket.descriptor(), message.c_str(), message.size(), MSG_NOSIGNAL);

    int roomNumber {-1};
    std::stringstream room;
    do {
        char buffer[8] {};
        ssize_t bytesRead = recv(userSocket.descriptor(), buffer, 8, MSG_NOSIGNAL);
        if (bytesRead < 1) {
            std::cerr << errno << " ";
            std::cerr << "Error reading data from client " << userSocket.descriptor() << std::endl;
            return -1;
        }
        room << buffer;
    } while (room >> roomNumber && roomNumber < 0 && roomNumber > 3);


    std::cout << "Client " << userSocket.descriptor() << " choosed " << roomNumber << " room." << std::endl;

    return roomNumber;
}

void IOServer::setMasterSocket(sockets::MasterSocket* socket)
{
    m_serverSocket.reset(socket);
}


void IOServer::processConnections(int eventsCount) // TODO: GIVE EVENT DESCRIPTOR TO WORKER
{
}

void IOServer::acceptConnections()
{
    epoll_event masterEvent;
    std::vector<epoll_event> events(MAX_CONNECTIONS);
    masterEvent.data.fd = m_serverSocket->descriptor();
    masterEvent.events = EPOLLIN;
    while (true) {
        int eventsCount = epoll_wait(m_serverSocket->descriptor(),
                                     &events[0],
                                     MAX_CONNECTIONS,
                                     -1);
        for (int i = 0; i < eventsCount; ++i) {
            std::cout << "CONNECTED!" <<std::endl;
        }
        sockaddr clientAddress;
        socklen_t sizeOfClientAddress = sizeof(clientAddress);

        int descriptor = accept(masterEvent.data.fd,
                                &clientAddress,
                                &sizeOfClientAddress);

        std::shared_ptr<sockets::SlaveSocket> slaveSocket (new sockets::SlaveSocket(descriptor));
        slaveSocket->setNonblock();

        std::string ip = inet_ntoa(((sockaddr_in *) &clientAddress)->sin_addr);

//        m_clients[descriptor] = std::make_pair(ip, slaveSocket);
        std::cout <<  ip + " connected!\n" << std::endl;
//        if (acceptDescriptor < 0) {
//            std::cerr << "Error while accepting! " << errno << std::endl;
//        }
        int workerNumber = getRoomFromUser(*slaveSocket);
        std::cout << workerNumber << std::endl;
    }
}

void IOServer::start()
{
    int workersCount = 4; // 4 for start
    createWorkers(workersCount);
}

void IOServer::createWorkers(const int count)
{
    int pid = fork(); // main will accept connections, child will create workers and then exit

    if (pid < 0) {
        std::cerr << "Error while fork for master and workers" << std::endl;
        return;
    }

    if (pid == 0) { // we are in workercreator
        for (int i = 0; i < count; i++) { // create count workers
            int pipeSockets[2];

            if (socketpair(AF_LOCAL, SOCK_STREAM, 0, pipeSockets) < 0) {
                std::cerr << "Cannot create socketpair!" << std::endl;
            }

            int wPid = fork();
            if (wPid == 0) { // in worker
                std::cout << "Created worker " << " " << getpid() << " with parent " << getppid() << std::endl;
                close(pipeSockets[0]);
                initWorker(pipeSockets[1]);
            } else if (wPid < 0){
                std::cerr << "Error while creating worker " << i << std::endl;
            } else { // we are in workercreator (parent)
                close(pipeSockets[1]); // closing unused socket, used by worker
                m_workersSockets.push_back(pipeSockets[0]);  // add unix socket for communication between workercreator and workers
            }
        }

        for(int i = 0; i < count; i++) { //worker creator wait for all childrens
            pid_t children = wait(nullptr);
            std::cout << "Children " << children << " died."  << std::endl;
        }
        _exit(0); // close workercreator if no childrens left
    } else { // we are in main and we'll accept external connections
        acceptConnections(); // TODO: MAIN WILL PASS FDS TO WORKERS
    }
}

void IOServer::initWorker(const int channelDescriptor)
{
    m_ioWorker = std::make_unique<IOWorker>(channelDescriptor); // creating worker
    for(;;);
    // TODO: WORKER START LISTENING ITS SOCKETPAIR AND DOING SOMETHING IN BLOCKING MODE
    // ADD SOCKETPAIR AND SETTING DESCRIPTORS FOR SERVER AND WORKER
    // WORKER SHOULD READ UNIX SOCKET TO GET SLAVE DESCRIPTOR, PROCESS IT AND SEND RESULT TO ALL CLIENTS (HOW TO STORE CLIENTS? WILL IT BE CHAT ROOM?)
}
