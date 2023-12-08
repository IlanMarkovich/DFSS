#include "Communicator.h"


Communicator::Communicator()
{

    /* Create socket */
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        throw SocketCreationException();
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
        r->data = ByteHelper::charArrTobytes(buf, recvlen);

        memset (buf, 0, sizeof (buf)); // clear buffer

        bind_user(r);
    }
}

void Communicator::bind_user(req* r)
{
    bytes response = this->DomainIPFetcher(r->data);

    // Send the response back to the client
    sendto(fd, response.data(), response.size(), 0, (struct sockaddr*)&r->clientaddr, sizeof(r->clientaddr));
}


bytes Communicator::DomainIPFetcher(bytes& input)
{
    const char* dns_server = "8.8.8.8";
    const int dns_port = 53;
    char response[4096];  // Adjust the size as needed

    // Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        throw SocketCreationException();
    }

    // Set up the sockaddr_in structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(dns_port);
    inet_pton(AF_INET, dns_server, &(server_addr.sin_addr)); // const ip so it will always work

    // Send DNS query
    ssize_t sent_bytes = sendto(sockfd, input.data(), input.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (sent_bytes == -1) {
        throw SendToDistantDnsException();
    }

    // Receive DNS response
    ssize_t received_bytes = recv(sockfd, response, sizeof(response), 0);
    if (received_bytes == -1) {
        throw GetFromDistantDnsException();
    }

    // Close the socket
    close(sockfd);
    return ByteHelper::charArrTobytes(response, received_bytes);
}