#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <cstring>
#include <stdio.h>
#include <arpa/inet.h>
#include <atomic>

#include "ByteHelper.h"
#include "GetFromDistantDnsException.h"
#include "SendToDistantDnsException.h"
#include "SocketCreationException.h"
#include "SocketBindException.h"

#include "DnsMessage.hpp"
#include "DatabaseManager.h"
#include "FileLoader.h"

#define PORT 53
#define MESSAGE_SIZE 256

// declare the struct sockaddr_in
struct sockaddr_in;


struct req
{
    int des;
    std::vector<unsigned char> data; // place holder for the data
    std::unique_ptr<DnsMessage> dnsMsg;
    socklen_t addlen;
    sockaddr_in clientaddr;
};

class Communicator
{
private:
    int fd;
    sockaddr_in dns_Server_addr;
    const int port = PORT;
    ThreadManager m_threadManager;
    DatabaseManager _dbManager;
    std::atomic<bool> _listen;
    FileLoader _fileLoader;

    void bind_user(req* r);
    bool SOP_validation(const std::vector<unsigned char>& response);
public:
    static bool hasDNSCrypt;

    // C tor
    Communicator();

    // Getters

    DatabaseManager& getDatabaseManager();

    // Methods

    void listen();
    void stopListening();
    static std::vector<unsigned char> DNS_ResponseFetcher(const std::vector<unsigned char>& input);
};
