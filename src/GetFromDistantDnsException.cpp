#include "GetFromDistantDnsException.h"

const char* GetFromDistantDnsException::what()
{
    return "Error: Getting a DNS answer to a distant DNS failed";
}