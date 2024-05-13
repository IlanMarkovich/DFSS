#pragma once

#include "Communicator.h"
#include <mutex>
#include <map>
#include <functional>

#define DNSSCRYPT_START "sudo /opt/dnscrypt/dnscrypt-proxy -service start"
#define DNSSCRYPT_STOP "sudo /opt/dnscrypt/dnscrypt-proxy -service stop"
#define PATH "/opt/dnscrypt"
#define DNSSCRYPT_INSTALL "sudo /opt/dnscrypt/dnscrypt-proxy -service install"

class Server
{
public:
    static std::mutex dbMutex;
private:
    Communicator m_communicator;
    const std::map<std::string, std::function<void()>> _commands = {
        { "help", [this](){ printHelp(); } },
        { "?", [this](){ printHelp(); } },
        { "show", [this](){ printSecurityFeatures(); } },
        { "change", [this](){ changeFeatureStatus(); } },
        { "blacklist", [this](){ blacklist(); } },
        { "bl", [this](){ blacklist(); } },
        { "whitelist", [this](){ whitelist(); } },
        { "wl", [this](){ whitelist(); } },
        { "view", [this](){ viewDatabase(); } },
        { "log", [this](){ logData(); } }
    };

public:
    Server();
    void run();

private:
    void printHelp() const;
    void printSecurityFeatures();
    std::string getFeatureStatus(bool status) const;
    void changeFeatureStatus();
    void blacklist();
    void whitelist();
    void viewDatabase();
    void logData();
};
