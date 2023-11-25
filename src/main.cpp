#include <iostream>
#include "Server.h"
#include "MyException.h"

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
    catch(MyException& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
#else
int main()
{   
    DatabaseManager dbManager;
    dbManager.pingDatabase();

    return 0;
}
#endif
