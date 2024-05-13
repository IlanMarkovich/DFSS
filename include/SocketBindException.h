#pragma once

#include "ServerException.h"

class SocketBindException : public ServerException
{
    public:
        virtual const char* what() override;
};