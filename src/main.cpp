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
    dbManager.pingDatabase();

    return 0;
}
#endif
