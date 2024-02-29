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

    std::cout << "Enter 'help' or '?' to see avaliable commands" << std::endl;

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
    std::cout << "help / ? : Print all commands avaliable for the user to use";
    std::cout << "exit : Exit the firewall and close all resources related correctly" << std::endl;
    std::cout << "show : Show all security features of the firewall, and show their current status (ON/OFF)" << std::endl;
    std::cout << "change : Change the status of a certain security feature" << std::endl;
    std::cout << "blacklist / bl : Add or remove a certain URL in a blacklist, meaning this URL cannot be accessed while the firewall is on" << std::endl;
    std::cout << "whitelist / wl : Add or remove a certain URL in a whitelist, meaning this URL can always be access while firewall is on, and no security feature will activate while trying to access it" << std::endl;
    std::cout << "view : View a certain data list which is stored in the database" << std::endl;
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
    catch (const std::exception& e)
    {
        std::cerr << "Feature name not found! Be sure to type the feature name exactly as shown." << std::endl;
    }
}

void Server::blacklist()
{
    std::string url;
    std::cout << std::endl << "Enter URL: ";
    std::cin >> url;

    std::lock_guard<std::mutex> dbLock(dbMutex);
    m_communicator.getDatabaseManager().blacklistUrl(url);
}

void Server::whitelist()
{
    std::string url;
    std::cout << std::endl << "Enter URL: ";
    std::cin >> url;

    std::lock_guard<std::mutex> dbLock(dbMutex);
    m_communicator.getDatabaseManager().whitelistUrl(url);
}

void Server::viewDatabase()
{
    std::cout <<  std::endl << "Enter the number of data you want to see:" << std::endl;
    std::cout << "1. Blacklist : list of blacklisted URLs" << std::endl;
    std::cout << "2. Whitelist : list of whitelisted URLs" << std::endl;
    std::cout << "3. Cache : list of the current queries which are cached and used for filtering if not turned off" << std::endl;
    std::cout << "4. Logs : view the list of logs of queries which were used in the past" << std::endl;

    int choice = 0;
    std::cout << "Enter data list number: ";
    std::cin >> choice;
    std::cout << std::endl;

    if(choice < 0 || choice > 4)
    {
        std::cerr << "Please enter a number from the data lists shown above!" << std::endl;
        return;
    }

    std::vector<std::string> dataList;

    {
        std::lock_guard<std::mutex> dbLock(dbMutex);
        DatabaseManager& db = m_communicator.getDatabaseManager();

        switch(choice)
        {
            case 1:
                dataList = db.getDataList(FILTER_DB, "Blacklist-URLs");
                break;
            case 2:
                dataList = db.getDataList(FILTER_DB, "Whitelist-URLs");
                break;
            case 3:
                dataList = db.getDataList(FILTER_DB, "Cache");
                break;
            case 4:
                dataList = db.getDataList(LOG_DB, "Logs");
                break;
        }
    }

    if(choice != 4)
    {
        for(std::string data : dataList)
        {
            std::cout << data << std::endl;
        }
    }
}
