#pragma once

#include "ServerException.h"

class GetFromDistantDnsException : public ServerException
{
    public:
        virtual const char* what() override;
};