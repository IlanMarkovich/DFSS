#include "Communicator.h"


Communicator::Communicator()
{

    /* Create socket */
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        throw new SocketCreationException();
    }

    /* clear the memory allocated */
    memset((sockaddr*)&dns_Server_addr, 0, sizeof (dns_Server_addr));

    dns_Server_addr.sin_family = AF_INET;   // IPv4 address family
    dns_Server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  // Give the local machine address
    dns_Server_addr.sin_port = htons (port); // Port at which server listens to the requests

    /* Bind the IP address and the port number to create the socket */
    if (bind (fd, (sockaddr*)&dns_Server_addr, sizeof (dns_Server_addr)) == -1) 
    {
        throw SocketBindException();
    }
}

void Communicator::listen()
{
    int threadno = 0;
    /* Infinte server loop */
    while (true)
    {
        int recvlen = 0;
        char buf[MESSAGE_SIZE]; // Hold buffer sent in udp packet


        req *r = new req;  // allocate memory for the soon to be request
        bzero (r, sizeof (req));  // Clear memory
        r->addlen = sizeof(r->clientaddr);

        /* waiting to recieve the requests from client at port */
        recvlen = recvfrom (fd, buf, MESSAGE_SIZE, 0, (sockaddr*) &r->clientaddr, &r->addlen);

        /* Filling the parameter values of the threaded function */
        r->des = fd;
        strcpy (r->str, buf);

        memset (buf, 0, sizeof (buf)); // clear buffer

        bind_user(r);
    }
}

void Communicator::bind_user(req* r)
{
    
}