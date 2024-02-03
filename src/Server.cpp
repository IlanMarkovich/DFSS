#include "Server.h"

#include <thread>



// Static variables initialization
std::mutex Server::dbMutex;

// C'tor
Server::Server() : m_communicator() {}

// Public Methods

void Server::run()
{
    // start dnscrypt program
    system(DNSSCRYPT_START);

    std::thread listeningThread(&Communicator::listen, &m_communicator);
    string cmd;

    while(cmd != "exit")
    {
        std::cout << "Enter Command: ";
        std::cin >> cmd;

        if(cmd != "exit")
            handleCommand(cmd);
    }

    m_communicator.stopListening();
    listeningThread.join();

}

// Private Methods

void Server::handleCommand(const string & cmd)
{
    string url;
    std::cout << std::endl << "Enter URL: ";
    std::cin >> url;

    if(cmd == "bl")
    {
        std::lock_guard<std::mutex> dbLock(dbMutex);
        m_communicator.getDatabaseManager().blacklistUrl(url);
    }
    else if(cmd == "wl")
    {
        std::lock_guard<std::mutex> dbLock(dbMutex);
        m_communicator.getDatabaseManager().whitelistUrl(url);
    }
}
