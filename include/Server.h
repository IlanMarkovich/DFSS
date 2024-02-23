#pragma once

#include "Communicator.h"
#include <mutex>

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

public:
    Server();
    void run();

private:
    void handleCommand(const string& cmd);
};
