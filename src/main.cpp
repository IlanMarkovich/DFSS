#include <iostream>
#include "Server.h"
#include "MyException.h"

int main()
{
    try
    {
        Server server;
        server.run();
    }
    catch(MyException* e)
    {
        std::cerr << e->what() << '\n';
    }
    return 0;
}
