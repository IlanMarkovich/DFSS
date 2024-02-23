#include "Server.h"

#include <thread>
#include <sys/stat.h>

// Static variables initialization
std::mutex Server::dbMutex;

// C'tor
Server::Server() : m_communicator() {}

// Public Methods

void Server::run()
{
    //check if dnscrypt program was installed and if not install it

    // Structure which would store the metadata
    struct stat sb;

    if (!(stat(PATH, &sb) == 0 && !(sb.st_mode & S_IFDIR)))
    {
        std::string cmdMakeDirInOpt = "sudo mkdir ";
        cmdMakeDirInOpt += PATH;
        system(cmdMakeDirInOpt.c_str());
        cmdMakeDirInOpt = "mv  -v ./dnscrypt/* ";
        cmdMakeDirInOpt += PATH;
        system(cmdMakeDirInOpt.c_str());
        system(DNSSCRYPT_INSTALL);
    }

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

    // stop dnscrypt program
    system(DNSSCRYPT_STOP);
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
