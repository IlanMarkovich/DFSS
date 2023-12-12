#pragma once

#include "ServerException.h"

class SocketCreationException : public ServerException
{
    public:
        virtual const char* what() override;
};