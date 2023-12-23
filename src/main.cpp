#include <iostream>
#include "Server.h"
#include "ServerException.h"

#include "DatabaseManager.h"

// If this run is NOT a test
#ifndef TEST
int main()
{
    try
    {
        Server server;
        server.run();
    }
    catch(ServerException& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
#else
int main()
{   
    return 0;
}
#endif
