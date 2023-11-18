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
#include "SocketCreationException.h"
#include "SocketBindException.h"
#include "ThreadManager.h"

#define PORT 53
#define MESSAGE_SIZE 2048

// declare the struct sockaddr_in
struct sockaddr_in sin;


struct req
{
    int des;
    char str[MESSAGE_SIZE]; // place holder for the data
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

        void bind_user(req* r);

    public:
        // C tor
        Communicator();

        void listen();
        
        
};