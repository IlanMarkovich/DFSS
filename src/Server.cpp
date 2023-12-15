#include "Server.h"

#include <thread>

Server::Server() : m_communicator()
{
    
}

void Server::run()
{
    std::thread listeningThread(&Communicator::listen, &m_communicator);
    string cmd;

    while(cmd != "exit")
    {
        std::cout << "Enter Command: ";
        std::cin >> cmd;
    }

    m_communicator.stopListening();
    listeningThread.join();
}
