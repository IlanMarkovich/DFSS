#pragma once

#include "Communicator.h"
#include <mutex>
#include <map>
#include <functional>

#define DNSSCRYPT_START "sudo /opt/dnscrypt-proxy/dnscrypt-proxy -service start"
#define DNSSCRYPT_STOP "sudo /opt/dnscrypt-proxy/dnscrypt-proxy -service stop"
#define PATH "/opt/dnscrypt-proxy"
#define DNSSCRYPT_INSTALL "sudo /opt/dnscrypt-proxy/dnscrypt-proxy -service install"

class Server
{
public:
    static std::mutex dbMutex;
private:
    Communicator m_communicator;
    const std::map<std::string, std::function<void()>> _commands = {
        { "help", [this](){ printHelp(); } },
        { "show security", [this](){ printSecurityFeatures(); } },
        { "ssec", [this](){ printSecurityFeatures(); } },
        { "change", [this](){ changeFeatureStatus(); } }
    };

public:
    Server();
    void run();

private:
    void printHelp() const;

    void printSecurityFeatures();
    std::string getFeatureStatus(bool status) const;

    void changeFeatureStatus();
};
