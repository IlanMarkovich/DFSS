#include <iostream>

#include "DatabaseManager.h"

// If this run is NOT a test
#ifndef TEST
int main()
{
    return 0;
}
#else
int main()
{   
    DatabaseManager dbManager;
    std::cout << "Has www.google.com: " << dbManager.searchUrl("www.google.com") << std::endl;

    return 0;
}
#endif
