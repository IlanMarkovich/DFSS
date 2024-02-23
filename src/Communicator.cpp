#include "Communicator.h"

#include "Filter.h"

#include <thread>

// C'tor
Communicator::Communicator()
 : _listen(true)
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

    // Set a timeout to the socket, so that when 'recvfrom' is used it will have a maximum time
    // before it will stop trying to get input from the listening socket
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
}

// Getters

DatabaseManager & Communicator::getDatabaseManager()
{
    return _dbManager;
}

// Public Methods

void Communicator::listen()
{
    while (_listen.load())
    {
        int recvlen = 0;
        char buf[MESSAGE_SIZE]; // Hold buffer sent in udp packet

        req *r = new req;  // allocate memory for the soon to be request
        bzero (r, sizeof (req));  // Clear memory
        r->addlen = sizeof(r->clientaddr);

        /* waiting to recieve the requests from client at port */
        recvlen = recvfrom (fd, buf, MESSAGE_SIZE, 0, (sockaddr*) &r->clientaddr, &r->addlen);
        
        if(recvlen > 0)
        {
            /* Filling the parameter values of the threaded function */
            r->des = fd;
            r->data = std::vector<unsigned char>(buf, buf + recvlen);
            r->dnsMsg = std::make_unique<DnsMessage>(r->data);

            bind_user(r);
        }

        memset (buf, 0, sizeof (buf)); // clear buffer
        delete r;
    }
}

void Communicator::stopListening()
{
    _listen.store(false);
}

std::vector<unsigned char> Communicator::DNS_ResponseFetcher(const std::vector<unsigned char>& input)
{
    const char* dns_server = "1.1.1.1";
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
    return std::vector<unsigned char>(response, response + received_bytes);
}

// Private Methods

void Communicator::bind_user(req* r)
{
    Filter filter(*(r->dnsMsg.get()), _dbManager);

    // If the DNS message isn't valid
    if(filter.getFilterResult())
    {
        return;
    }

    std::vector<unsigned char> response = Communicator::DNS_ResponseFetcher(r->data);

    if(!SOP_validation(response))
        return;

    // Send the response back to the client
    sendto(fd, response.data(), response.size(), 0, (struct sockaddr*)&r->clientaddr, sizeof(r->clientaddr));
}

bool Communicator::SOP_validation(const std::vector<unsigned char>& response)
{
    DnsMessage responseMsg(response);
    std::vector<unsigned char> ip = responseMsg.getResponse_RRset<DNS_A_Answer>()[0].get_IP_address();

    // Checks if the ip address from the resposne isn't an IP address which cannot be used in the internet
    return !(ip[0] == 10 || (ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31) || (ip[0] == 192 && ip[1] == 168) ||
        ip[0] == 127 || (ip[0] == 169 && ip[1] == 254) || (ip[0] >= 224 && ip[0] <= 239));
}
