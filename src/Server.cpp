#include "Server.h"


Server::Server() : m_communicator()
{
    
}

void Server::run()
{
    m_communicator.listen();
}
