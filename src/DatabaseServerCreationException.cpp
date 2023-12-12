#include "DatabaseServerCreationException.h"


const char* DatabaseServerCreationException::what()
{
    return "Error: failed to create a database server using mongod";
}
