#pragma once

#include "ServerException.h"

class SendToDistantDnsException : public ServerException
{
    public:
        virtual const char* what() override;
};