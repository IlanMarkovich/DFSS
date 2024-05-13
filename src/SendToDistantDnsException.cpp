#include "SendToDistantDnsException.h"

const char* SendToDistantDnsException::what()
{
    return "Error: Sending a DNS Request to a distant DNS failed";
}