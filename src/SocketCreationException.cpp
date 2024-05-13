#include "SocketCreationException.h"

const char* SocketCreationException::what()
{
    return "Error: Socket creation failed";
}