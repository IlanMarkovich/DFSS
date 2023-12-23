#include <iostream>
#include "Server.h"
#include "MyException.h"
#include "RegexFilter.h"
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
    try
    {
        std::string urlOne = "abcdefg";
        std::string urlTwo = "abcdefgxx";
        RegexFilter test(urlOne);
        std::cout << test.Filter(urlTwo);
    }
    catch(MyException& e)
    {

    }

    return 0;
}
#endif
