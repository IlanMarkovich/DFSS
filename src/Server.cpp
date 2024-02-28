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
    bool hasDNSCrypt = m_communicator.getDatabaseManager().isFeatureTurnedOn("DNSCrypt");

    // Check if dnscrypt program was installed and if not install it
    if(hasDNSCrypt)
    {
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
    }

    std::thread listeningThread(&Communicator::listen, &m_communicator);
    string cmd;

    while(cmd != "exit")
    {
        std::cout << std::endl << "Enter Command: ";
        std::cin >> cmd;

        if(cmd != "exit" && _commands.count(cmd) != 0)
            _commands.at(cmd)();
    }

    m_communicator.stopListening();
    listeningThread.join();

    // stop dnscrypt program
    if(hasDNSCrypt)
        system(DNSSCRYPT_STOP);
}

// Private Methods

void Server::printHelp() const
{
    std::cout << "show security OR ssec : Show all security features of the firewall, and show their current status (ON/OFF)" << std::endl;
}

void Server::printSecurityFeatures()
{
    std::lock_guard<std::mutex> dbLock(dbMutex);
    DatabaseManager& db = m_communicator.getDatabaseManager();

    std::cout << std::endl << "Security Features:" << std::endl;
    std::cout << "Cache Filtering - " << getFeatureStatus(db.isFeatureTurnedOn("cache")) << std::endl;
    std::cout << "Blacklist & Whitelist Filtering - " << getFeatureStatus(db.isFeatureTurnedOn("blwl")) << std::endl;
    std::cout << "External Malicious URL Check - " << getFeatureStatus(db.isFeatureTurnedOn("external")) << std::endl;
    std::cout << "Phishing Filtering - " << getFeatureStatus(db.isFeatureTurnedOn("phishing")) << std::endl;
    std::cout << "DNSSEC Validation Checks - " << getFeatureStatus(db.isFeatureTurnedOn("DNSSEC")) << std::endl;
    std::cout << "DNSCrypt Protocol Usage - " << getFeatureStatus(db.isFeatureTurnedOn("DNSCrypt")) << std::endl;
    std::cout << "SOP Validation Check - " << getFeatureStatus(db.isFeatureTurnedOn("SOP")) << std::endl;
}

std::string Server::getFeatureStatus(bool status) const
{
    return status ? "ON" : "OFF";
}

void Server::changeFeatureStatus()
{
    std::cout << std::endl << "Features names:" << std::endl;
    std::cout << "Cache Filtering - cache" << std::endl;
    std::cout << "Blacklist & Whitelist Filtering - blwl" << std::endl;
    std::cout << "External Malicious URL Check - external" << std::endl;
    std::cout << "Phishing Filtering - phishing" << std::endl;
    std::cout << "DNSSEC Validation Checks - DNSSEC" << std::endl;
    std::cout << "DNSCrypt Protocol Usage - DNSCrypt" << std::endl;
    std::cout << "SOP Validation Check - SOP" << std::endl;

    std::string feature;
    std::cout << "Enter the feature name: ";
    std::cin >> feature;
    
    std::lock_guard<std::mutex> dbLock(dbMutex);

    try
    {
        m_communicator.getDatabaseManager().changeFeatureStatus(feature);
    }
    catch (std::exception& e)
    {
        std::cerr << "Feature name not found! Be sure to type the feature name exactly as shown." << std::endl;
    }
}

// string url;
// std::cout << std::endl << "Enter URL: ";
// std::cin >> url;

// if(cmd == "bl")
// {
//     std::lock_guard<std::mutex> dbLock(dbMutex);
//     m_communicator.getDatabaseManager().blacklistUrl(url);
// }
// else if(cmd == "wl")
// {
//     std::lock_guard<std::mutex> dbLock(dbMutex);
//     m_communicator.getDatabaseManager().whitelistUrl(url);
// }
