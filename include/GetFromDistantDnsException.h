#pragma once

#include "MyException.h"

class GetFromDistantDnsException : public MyException
{
    public:
        virtual const char* what() override;
};