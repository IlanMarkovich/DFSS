#include "SocketBindException.h"

const char* SocketBindException::what()
{
    return "Error: Socket binding failed";
}