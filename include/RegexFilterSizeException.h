#pragma once

#include "ServerException.h"

class RegexFilterSizeException : public ServerException
{
    public:
        virtual const char* what() override;
};